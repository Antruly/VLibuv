#include "VWebServer.h"


VWebServer::VWebServer() {
  _http_server = new VHttpServer();

  _http_server->setCloseCb([=](VTcpServer *tcpServer) { this->stop(); });
  _http_server->setNewHttpClientCb([=](VHttpClient *newClient) {
    newClient->setId(_client_id_index++);
    VHttpClientObject *clientObj = new VHttpClientObject();
    clientObj->client = newClient;
    clientObj->_last_active_time = VTimerInfo::getCurrentTimestamp();
    clientObj->client->getVHttpRequest()->setData(clientObj);
    clientObj->client->getVHttpResponse()->setData(clientObj);

    newClient->setCloseCb([=](VHttpClient *closeClient, int status) {
      VHttpClientObject *clientObj = reinterpret_cast<VHttpClientObject *>(
          closeClient->getVHttpRequest()->getData());
      clientObj->client_close = true;
    });
    if (!parseRequest(newClient)) {
      newClient->close();
      return;
    }

    if (newClient->getVHttpRequest()->getKeepAlive()) {
      {
        std::lock_guard<std::mutex> locked(_client_mutex);
        _request_clients[newClient] = clientObj;
      }

      this->runClientKeepAlive(clientObj);

    } else {
      newClient->close();
    }
    delete clientObj;
    clientObj = nullptr;
  });
}

VHttpServer *VWebServer::getVHttpServer() { return _http_server; }

int VWebServer::listenIpv4(const VString &addripv4, short port) {
  if (_http_server == nullptr) {
    Log->logDebugError("_http_server is null");
    return -1;
  }
  return _http_server->listenIpv4(addripv4.c_str(), port, 0);
}

int VWebServer::run() {
  if (_http_server == nullptr) {
    Log->logDebugError("_http_server is null");
    return -1;
  }

  VIdle runIdle(_http_server->getVTcpServer()->getVLoop());
  runIdle.start([=](VIdle *idle) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  });
  _running = true;
  while (_running) {
    _http_server->run(uv_run_mode::UV_RUN_NOWAIT);
    this->removeClosedClients();
  };
  if (_http_server != nullptr)
    runIdle.stop();
}

int VWebServer::stop() {
  _running = false;
  return 0;
}

void VWebServer::waitStoped() {
  while (_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  };
}

bool VWebServer::addWebInterface(VWebInterface *webInterface) {
  if (!webInterface)
    return false;

  // 自动识别静态路径
  if (webInterface->getRawPath().endsWith("/")) {
    webInterface->setStaticPath(true);
  }

  return _router.addRoute(webInterface->getRawPath(), webInterface);
}

std::map<VString, VWebInterface *> &VWebServer::getWebInterfaces() {
  return _router.getRoutes();
}

bool VWebServer::addClientCookie(VString cookieName, void *clientData) {
  _client_cookies[cookieName] = clientData;
  return true;
}

std::map<VString, void *> &VWebServer::getClientCookies() {
  return _client_cookies;
}

bool VWebServer::parseRequest(VHttpClient *recv_client) {

  std::string recvPerIP;
  int recvPerPort;
  recv_client->getVTcpClient()->getPeerAddrs(recvPerIP, recvPerPort);
  Log->logDebugInfo("new client connected %s:%d set id=%lld !\n",
                    recvPerIP.c_str(), recvPerPort, recv_client->getId());

  VHttpRequest *request = recv_client->getVHttpRequest();

  request->setRequestRecvBodyCb(
      [=](VHttpRequest *request, const VBuf *bodyData) -> bool {
        VHttpClientObject *cObj =
            reinterpret_cast<VHttpClientObject *>(request->getData());
        cObj->_last_active_time = VTimerInfo::getCurrentTimestamp();
        cObj->state = VHttpClientObject::ConnectionState::READING_BODY;
        VHttpResponse *response = cObj->client->getVHttpResponse();

        if (cObj->web_interface != nullptr &&
            cObj->web_interface->getRecvBodyCallBackup()) {
          cObj->web_interface->getRecvBodyCallBackup()(cObj, request, response,
                                                       bodyData);
          // 返回值true 是自动清理缓存body，当异步回调存在则清理
          return true;
        }
        return false;
      });

  request->setRequestParserFinishCb([=](VHttpRequest *request, int status) {
    VHttpClientObject *cObj =
        reinterpret_cast<VHttpClientObject *>(request->getData());
    cObj->_last_active_time = VTimerInfo::getCurrentTimestamp();
    cObj->state = VHttpClientObject::ConnectionState::IDLE;
    VHttpResponse *response = cObj->client->getVHttpResponse();

    if (cObj->web_interface != nullptr &&
        (cObj->web_interface->getParseFinishCallBackup() ||
         cObj->web_interface->isStaticPath())) {

      if (!cObj->web_interface) {
        // 处理404
        response->setHttpVersion(VWEB_HTTP_VERSION_1_1);
        response->setUseGzip(request->getUseGzip());
        response->setContentType("text/html; charset=utf-8");
        cObj->client->sendResponse(404, page_404_buf);
        return;
      }
      std::vector<METHOD_TYPE> methodTypes =
          cObj->web_interface->getMethodTypes();

      bool haveType = false;
      for (auto type : methodTypes) {
        if (type == request->getMethod()) {
          haveType = true;
          break;
        }
      }

      // 非法请求方法
      if (!haveType) {
        VString allowValue;
        int i = 0;
        for (auto type : methodTypes) {
          if (i++ > 0)
            allowValue += ",";
          allowValue += request->getVHttpParser()->getMethodName(type);
        }
        response->setHttpVersion(VWEB_HTTP_VERSION_1_1);
        response->setUseGzip(request->getUseGzip());
        response->setContentType("text/html; charset=utf-8");
        response->addHeader("Allow", allowValue);
        recv_client->sendResponse(404, page_404_buf);
        return;
      }

      // 处理静态文件
      if (cObj->web_interface->isStaticPath()) {
        this->handleStaticFile(recv_client, cObj->web_interface,
                               request->getParsedURL().path);
        return;
      }

      // 处理动态接口
      if (cObj->web_interface->getParseFinishCallBackup()) {
        cObj->web_interface->getParseFinishCallBackup()(cObj, request,
                                                        response);
      }
      return;
    }
  });
  request->setRequestParserHeadersFinishCb(
      [=](VHttpRequest *request, HttpHeaders *requestHeaders) {
        ParsedURL parsedUrl = request->getParsedURL();
        VWebInterface *webInterface = _router.resolve(parsedUrl.path);
        VHttpClientObject *cObj =
            reinterpret_cast<VHttpClientObject *>(request->getData());
        VHttpResponse *response = cObj->client->getVHttpResponse();
        cObj->web_interface = webInterface;
        cObj->_last_active_time = VTimerInfo::getCurrentTimestamp();
        cObj->state = VHttpClientObject::ConnectionState::READING_HEAD;
        response->resetParser();

        if (cObj->web_interface != nullptr &&
            cObj->web_interface->getParseHeadersFinishCallBackup()) {
          cObj->web_interface->getParseHeadersFinishCallBackup()(cObj, request,
                                                                 response);
        }

        return;
      });
  request->setRequestChunkHeaderCb([=](VHttpRequest *request, int status) {

  });
  request->setRequestChunkCompleteCb([=](VHttpRequest *request, int status) {

  });
  request->initRecvCallback();
  recv_client->readStart();
  recv_client->waitRecvRequest();

  return true;
}

void VWebServer::handleIdleState(VHttpClientObject *clientObj) {
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void VWebServer::handleReadingHead(VHttpClientObject *clientObj) {}

void VWebServer::handleReadingBody(VHttpClientObject *clientObj) {}

void VWebServer::runClientKeepAlive(VHttpClientObject *clientObj,
                                    uint64_t maxTimeout) {
  // 启动超时定时器
  if (clientObj == nullptr || clientObj->client == nullptr) {
    return;
  }
  clientObj->_timeout_timer =
      new VTimer(clientObj->client->getVTcpClient()->getVLoop());
  clientObj->_timeout_timer->setData(clientObj);
  clientObj->_timeout_timer->start(
      [=](VTimer *timer) {
        auto obj = static_cast<VHttpClientObject *>(timer->getData());
        if (VTimerInfo::getCurrentTimestamp() - obj->_last_active_time >
            maxTimeout) {
          obj->client->close();
        }
      },
      maxTimeout, 0);

  // 事件循环处理
  while (!clientObj->client_close) {
    if (clientObj->client != nullptr && !clientObj->client_close) {
      clientObj->client->getVTcpClient()->run(uv_run_mode::UV_RUN_NOWAIT);
    }

    // 状态驱动处理
    switch (clientObj->state) {
    case VHttpClientObject::ConnectionState::IDLE:
      handleIdleState(clientObj);
      break;
    case VHttpClientObject::ConnectionState::READING_HEAD:
      handleReadingHead(clientObj);
      break;
    case VHttpClientObject::ConnectionState::READING_BODY:
      handleReadingBody(clientObj);
      break;
    case VHttpClientObject::ConnectionState::SENDING:
      // 发送状态由异步回调处理
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(0));
  }
  clientObj->_timeout_timer->stop();
  delete clientObj->_timeout_timer;
  clientObj->_timeout_timer = nullptr;
}

void VWebServer::removeClosedClients() {
  std::lock_guard<std::mutex> locked(_client_mutex);
  for (auto it = _request_clients.begin(); it != _request_clients.end();) {
    if (!it->second->client_close) {
      it = _request_clients.erase(it);
    } else {
      ++it;
    }
  }
}

// 新增静态文件处理方法
void VWebServer::handleStaticFile(VHttpClient *client,
                                  VWebInterface *staticInterface,
                                  const VString &rawPath) {
  client->getVHttpResponse()->setHttpVersion(VWEB_HTTP_VERSION_1_1);
  client->getVHttpResponse()->setUseGzip(
      client->getVHttpRequest()->getUseGzip());
  client->getVHttpResponse()->setKeepAlive(true);
  client->getVHttpResponse()->addHeader("Date", getGmtTime());

  // 获取完整路径
  VString fullPath =
      VString::UrlDecode(staticInterface->getRootPath() + rawPath);

  std::string stdfullpath = fullPath;

  // 安全检查：防止路径穿越
  if (fullPath.find("..") != VString::npos) {
    client->getVHttpResponse()->setContentType("text/html; charset=utf-8");
    client->sendResponse(403, page_403_buf);
    return;
  }

  // 读取文件数据
  VBuf fileData;
  if (!VFile::readFileData(fullPath, fileData)) {
    client->getVHttpResponse()->setContentType("text/html; charset=utf-8");
    client->sendResponse(404, page_404_buf);
    return;
  }

  client->getVHttpResponse()->setContentType(this->getMimeType(fullPath));
  client->sendResponse(200, fileData);
  Log->logDebug("tcp id:%d path:%s size:%d", client->getId(), rawPath.c_str(),
                fileData.size());
  std::cout << rawPath << std::endl;

  // client->close();
}

std::map<VString, VString> VWebServer::parseRouteParams(const VString &pattern,
                                                        const VString &path) {
  return std::map<VString, VString>();
}

void VWebServer::addRequestFilter(std::function<bool(VHttpRequest *)> filter) {
  // 等待实现过滤器
}

void VWebServer::setErrorHandler(
    std::function<void(VHttpResponse *, int)> handler) {
  // 等待实现过滤器
}

VString VWebServer::getGmtTime() {
  // 获取当前系统时间（UTC 时间）
  std::time_t now = std::time(nullptr);
  std::tm *gmt_time = std::gmtime(&now); // 转换为 GMT 时间

  // 定义格式化输出
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

  return VString(buffer);
}

VString VWebServer::getMimeType(const VString &filePath) {
  std::string newfilePath = filePath;
  size_t dotPos = newfilePath.rfind('.');
  if (dotPos == VString::npos)
    return "application/octet-stream";

  VString ext = newfilePath.substr(dotPos + 1);
  std::unordered_map<std::string, VString> mimeTypes = {
      // 文本类型
      {"html", "text/html; charset=utf-8"},
      {"css", "text/css"},
      {"txt", "text/plain"},
      {"csv", "text/csv"},
      {"xml", "application/xml"},
      {"json", "application/json"},

      // 编程相关
      {"js", "text/javascript"}, // 最新标准推荐使用 text/ 前缀
      {"wasm", "application/wasm"},

      // 图像类型
      {"png", "image/png"},
      {"jpg", "image/jpeg"},
      {"jpeg", "image/jpeg"},
      {"gif", "image/gif"},
      {"svg", "image/svg+xml"},
      {"ico", "image/x-icon"},
      {"webp", "image/webp"},

      // 视频类型
      {"mp4", "video/mp4"},
      {"webm", "video/webm"},
      {"mov", "video/quicktime"},
      {"avi", "video/x-msvideo"},
      {"mkv", "video/x-matroska"},

      // 音频类型
      {"mp3", "audio/mpeg"},
      {"wav", "audio/wav"},
      {"ogg", "audio/ogg"},
      {"aac", "audio/aac"},
      {"flac", "audio/flac"},
      {"weba", "audio/webm"},

      // 压缩文件
      {"zip", "application/zip"},
      {"tar", "application/x-tar"},
      {"gz", "application/gzip"},
      {"7z", "application/x-7z-compressed"},
      {"rar", "application/x-rar-compressed"},
      {"bz2", "application/x-bzip2"},

      // 可执行文件
      {"exe", "application/octet-stream"},
      {"dmg", "application/x-apple-diskimage"},
      {"deb", "application/vnd.debian.binary-package"},
      {"msi", "application/x-msdownload"},
      {"apk", "application/vnd.android.package-archive"},

      // 文档类型
      {"pdf", "application/pdf"},
      {"doc", "application/msword"},
      {"docx", "application/"
               "vnd.openxmlformats-officedocument.wordprocessingml.document"},
      {"xls", "application/vnd.ms-excel"},
      {"xlsx",
       "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
      {"ppt", "application/vnd.ms-powerpoint"},
      {"pptx", "application/"
               "vnd.openxmlformats-officedocument.presentationml.presentation"},
      {"odt", "application/vnd.oasis.opendocument.text"},

      // 字体文件
      {"ttf", "font/ttf"},
      {"otf", "font/otf"},
      {"woff", "font/woff"},
      {"woff2", "font/woff2"},

      // 其他类型
      {"epub", "application/epub+zip"},
      {"swf", "application/x-shockwave-flash"}};

  auto it = mimeTypes.find(ext);
  return it != mimeTypes.end() ? it->second : "application/octet-stream";
}
