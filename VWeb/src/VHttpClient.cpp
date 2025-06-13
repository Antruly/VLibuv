#pragma once
#include "VHttpClient.h"
#include <cassert>
#include <random>

VHttpClient::VHttpClient()
    : tcp_client_(new VTcpClient()), request_(new VHttpRequest(tcp_client_)),
      response_(new VHttpResponse(tcp_client_)), client_id(0),
      own_tcp_client_(true) {}

VHttpClient::VHttpClient(VTcpClient *tcp_client, bool is_own_tcp_client)
    : tcp_client_(tcp_client), request_(new VHttpRequest(tcp_client)),
      response_(new VHttpResponse(tcp_client)), client_id(0),
      own_tcp_client_(is_own_tcp_client) {}

VHttpClient::~VHttpClient() {
  if (own_tcp_client_ && tcp_client_ != nullptr) {
    tcp_client_->close();
    tcp_client_->waitCloseFinish();
    delete tcp_client_;
  }
  delete request_;
  request_ = nullptr;
  delete response_;
  response_ = nullptr;

  if (openssl_ != nullptr) {
    delete openssl_;
    openssl_ = nullptr;
  }
}

void VHttpClient::setId(int64_t id) { client_id = id; }

int64_t VHttpClient::getId() { return client_id; }

VHttpRequest *VHttpClient::getVHttpRequest() const { return request_; }

VHttpResponse *VHttpClient::getVHttpResponse() const { return response_; }

VTcpClient *VHttpClient::getVTcpClient() const { return tcp_client_; }

VOpenSsl *VHttpClient::getVOpenSsl() const { return openssl_; }

void VHttpClient::initCallback(bool isSsl) {

  assert(tcp_client_ != nullptr);
  this->getVTcpClient()->setCloseCb([this](VTcpClient *tcp) {
    if (http_client_close_cb) {
      http_client_close_cb(this, 0);
    }
  });

  if (isSsl) {
    assert(openssl_ != nullptr);
    openssl_->setSslConnectiondCb([this](VOpenSsl *ssl, int status) {
      if (status < 0) {
        error_message =
            "openssl_ Connectiond error status:" + std::to_string(status);
      }
      if (this->http_client_connectiond_cb)
        this->http_client_connectiond_cb(this, status);
    });
  } else {
    tcp_client_->setConnectiondCb([this](int status) {
      if (status < 0) {
        error_message =
            "tcpClient Connectiond error status:" + std::to_string(status);
      }
      if (this->http_client_connectiond_cb)
        this->http_client_connectiond_cb(this, status);
    });
  }
}

void VHttpClient::initSsl(const SSL_METHOD *method) {

  if (openssl_ == nullptr) {
    openssl_ = new VOpenSsl(method);
  } else if (openssl_ != nullptr && openssl_->getSslMethod() != method) {
    delete openssl_;
    openssl_ = nullptr;
    openssl_ = new VOpenSsl(method);
  }

  openssl_->initClient(tcp_client_);
  openssl_->initCallback();

  if (request_ != nullptr) {
    request_->setSslPoint(openssl_);
  }

  if (response_ != nullptr) {
    response_->setSslPoint(openssl_);
  }
}

std::string VHttpClient::getUrlFileName() {
  int pos = request_->getParsedURL().path.find_last_of("/");
  if (pos >= request_->getParsedURL().path.size()) {
    return std::string();
  }
  std::string filename(request_->getParsedURL().path.substr(pos + 1));
  return filename;
}

size_t VHttpClient::waitRecvRequest(const uint64_t &maxTimeout) {
  // 等待响应
  request_->waitRecvHeadersFinish(maxTimeout);

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return -1;
  }

  if (request_->isParser() && !request_->getKeepAlive()) {
    // 关闭连接
    tcp_client_->close();
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_DEFAULT);
    }
  }

  return request_->getBody().size();
}

size_t VHttpClient::waitRecvRequestBody(const uint64_t &maxTimeout) {
  // 等待响应
  request_->waitRecvBody(maxTimeout);

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return -1;
  }

  if (request_->isParser() && !request_->getKeepAlive()) {
    // 关闭连接
    tcp_client_->close();
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_DEFAULT);
    }
  }

  return request_->getBody().size();
}

bool VHttpClient::sendResponse(int code, const VBuf &body) {
  if (tcp_client_ == nullptr || request_ == nullptr || response_ == nullptr) {
    return false;
  }

  response_->initCallback();

  response_->initSendCallback();

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return false;
  }
  if (response_->getStatusCode() <= 0 || code != 200)
    response_->setStatusCode(code);

  if (body.size() > 0) {
    if (response_->getUseGzip()) {
      response_->setBodyCompress(body);
    } else {
      response_->setBody(body);
    }
  }
  if (response_->getBody().size() > 0) {
    if (!response_->sendResponse(true)) {
      return false;
    }
  } else {

    if (!response_->sendResponse(false)) {
      return false;
    }
  }

  return true;
}

bool VHttpClient::sendResponseBody(const VBuf &body, bool isEnd) {
  cache_body_.clear();
  response_->setBody(body);
  if (response_->getUseChunked()) {
    if (!response_->sendResponseChunked(cache_body_, isEnd)) {
      return false;
    }
  } else {
    if (!response_->sendResponseBody(cache_body_, true)) {
      return false;
    }
  }

  tcp_client_->waitWriteFinish();
  return true;
}

// 同步请求

bool VHttpClient::sendRequest(const METHOD_TYPE &method, const std::string &url,
                              const HttpHeaders &headers, const VBuf &body,
                              const uint64_t &maxTimeout) {
  if (tcp_client_ == nullptr || request_ == nullptr || response_ == nullptr) {
    return false;
  }
  response_->initData();
  response_->initCallback();
  response_->initRecvCallback();

  request_->initCallback();
  request_->initSendCallback();

  request_->setMethod(method);
  if (&headers != nullptr) {
    for (auto header : headers) {
      request_->addHeader(header.first, header.second);
    }
  }
  if (&body != nullptr) {
    request_->setBody(body);
  }

  if (!connect(url)) {
    return false;
  }
  if (!tcp_client_->getVLoop()->isActive()) {
    tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
  } else {
    tcp_client_->waitConnectFinish();
  }

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return false;
  }

  if (!request_->sendRequest()) {
    return false;
  }

  if (!tcp_client_->getVLoop()->isActive()) {
    tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
  } else {
    tcp_client_->waitWriteFinish();
  }

  tcp_client_->clientReadStart();

  if (tcp_client_->getVLoop()->isActive()) {
    uint64_t timeout = 0;
    while (!response_->isParser() && timeout < maxTimeout) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      timeout++;
    }
  } else {
    // 等待响应
    request_->waitRecvHeadersFinish(maxTimeout);
  }

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return false;
  }

  if (!request_->getKeepAlive()) {
    // 关闭连接
    tcp_client_->close();
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_DEFAULT);
    }
  }

  return true;
}

bool VHttpClient::sendRequest(const std::string &url, const VBuf &body) {
  if (tcp_client_ == nullptr || request_ == nullptr || response_ == nullptr) {
    return false;
  }
  response_->initData();
  response_->initCallback();

  request_->initCallback();

  if (!connect(url)) {
    return false;
  }

  request_->initSendCallback();
  response_->initRecvCallback();

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return false;
  }

  if (body.size() > 0) {
    if (request_->getUseGzip()) {
      request_->setBodyCompress(body);
    } else {
      request_->setBody(body);
    }
  }
  if (request_->getBody().size() > 0) {
    if (!request_->sendRequest(true)) {
      return false;
    }
  } else {

    if (!request_->sendRequest(false)) {
      return false;
    }
  }

  return true;
}

bool VHttpClient::sendRequestBody(const VBuf &body, bool isEnd) {

  cache_body_.clear();
  request_->setBody(body);

  if (request_->getUseChunked()) {
    if (!request_->sendRequestChunked(cache_body_, isEnd)) {
      return false;
    }
  } else {
    if (!request_->sendRequestBody(cache_body_, true)) {
      return false;
    }
  }

  tcp_client_->waitWriteFinish();
  return true;
}

void VHttpClient::readStart() { tcp_client_->clientReadStart(); }

size_t VHttpClient::waitRecvResponse(const uint64_t &maxTimeout) {
  // 等待响应
  response_->waitRecvHeadersFinish(maxTimeout);

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return 0;
  }

  if (response_->isParser() && !request_->getKeepAlive()) {
    // 关闭连接
    tcp_client_->close();
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_DEFAULT);
    }
  }

  return response_->getBody().size();
}

size_t VHttpClient::waitRecvResponseBody(const uint64_t &maxTimeout) {
  // 等待响应
  response_->waitRecvBody(maxTimeout);

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return 0;
  }

  if (response_->isParser() && !request_->getKeepAlive()) {
    // 关闭连接
    tcp_client_->close();
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_DEFAULT);
    }
  }

  return response_->getBody().size();
}

VBuf VHttpClient::getRecvResponseBody(size_t count) {
  VBuf retBuf;
  VBuf &recvBuf = response_->recvBody();

  if (count > recvBuf.size() || count == 0) {
    count = recvBuf.size();
    retBuf = recvBuf;
    recvBuf.clear();
  } else if (count < recvBuf.size()) {
    VBuf newBuf;
    retBuf.appendData(recvBuf.getConstData(), count);
    newBuf.appendData(recvBuf.getConstData() + count, recvBuf.size() - count);
    recvBuf = newBuf;
  } else {
    retBuf = recvBuf;
    recvBuf.clear();
  }
  return retBuf;
}

void VHttpClient::clearRecvResponseBody() { response_->recvBody().clear(); }

// 异步请求
void VHttpClient::sendRequestAsync(const METHOD_TYPE &method,
                                   const std::string &url,
                                   const HttpHeaders &headers, const VBuf &body,
                                   const uint64_t &maxTimeout) {}

void VHttpClient::setConnectiondCb(
    std::function<void(VHttpClient *, int)> connectiond_cb) {
  http_client_connectiond_cb = connectiond_cb;
}

void VHttpClient::setCloseCb(std::function<void(VHttpClient *, int)> close_cb) {
  http_client_close_cb = close_cb;
}

void VHttpClient::setRequestSendFinishCb(
    std::function<void(VHttpRequest *, int)> request_send_finish_cb) {
  request_->setRequestSendFinishCb(request_send_finish_cb);
}

void VHttpClient::setRequestParserFinishCb(
    std::function<void(VHttpRequest *, int)> request_parser_finish_cb) {
  request_->setRequestParserFinishCb(request_parser_finish_cb);
}

void VHttpClient::setResponseSendFinishCb(
    std::function<void(VHttpResponse *, int)> response_send_finish_cb) {
  response_->setResponseSendFinishCb(response_send_finish_cb);
}

void VHttpClient::setResponseParserFinishCb(
    std::function<void(VHttpResponse *, int)> response_parser_finish_cb) {
  response_->setResponseParserFinishCb(response_parser_finish_cb);
}

void VHttpClient::setRequestParserHeadersFinishCb(
    std::function<void(VHttpRequest *, HttpHeaders *)>
        request_parser_headers_finish_cb) {

  request_->setRequestParserHeadersFinishCb(request_parser_headers_finish_cb);
}

void VHttpClient::setRequestRecvBodyCb(
    std::function<bool(VHttpRequest *, const VBuf *)> request_recv_body_cb) {
  request_->setRequestRecvBodyCb(request_recv_body_cb);
}

void VHttpClient::setResponseParserHeadersFinishCb(
    std::function<void(VHttpResponse *, HttpHeaders *)>
        response_parser_headers_finish_cb) {
  response_->setResponseParserHeadersFinishCb(
      response_parser_headers_finish_cb);
}

void VHttpClient::setResponseRecvBodyCb(
    std::function<bool(VHttpResponse *, const VBuf *)> response_recv_body_cb) {
  response_->setResponseRecvBodyCb(response_recv_body_cb);
}

int VHttpClient::run(uv_run_mode md) { return tcp_client_->run(md); }

void VHttpClient::close() {
  if (tcp_client_ != nullptr) {
    tcp_client_->close();
  }
}

bool VHttpClient::connect(const std::string &url) {
  request_->setUrl(url);
  ParsedURL parsedurl = request_->getParsedURL();
  std::string addrs;
  int port;
  if (parsedurl.protocol == "https") {
    this->initSsl();
    this->initCallback(true);
    this->request_->setHttpSsl(true);
    this->response_->setHttpSsl(true);
  } else {
    this->initCallback(false);
  }

  tcp_client_->getPeerAddrs(addrs, port);

  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) ==
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) {
    if (addrs != parsedurl.host) {
      request_->closeConnection();
      if (!tcp_client_->getVLoop()->isActive()) {
        tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
      } else {
        tcp_client_->waitCloseFinish();
      }

      if (!request_->connect(parsedurl.host, parsedurl.port)) {
        return false;
      }
    }

  } else if ((tcp_client_->getStatus() &
              VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_NONE) ==
             VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_NONE) {
    if (!request_->connect(parsedurl.host, parsedurl.port)) {
      return false;
    }
  } else if ((tcp_client_->getStatus() &
              VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_DISCONNECTED) ==
             VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_DISCONNECTED) {
    if (!request_->connect(parsedurl.host, parsedurl.port)) {
      return false;
    }
  } else if ((tcp_client_->getStatus() &
              VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) ==
             VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) {
    if (!request_->connect(parsedurl.host, parsedurl.port)) {
      return false;
    }
  } else {
    if (!request_->connect(parsedurl.host, parsedurl.port)) {
      return false;
    }
  }

  if (!tcp_client_->getVLoop()->isActive()) {
    while ((tcp_client_->getStatus() &
            VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTING) > 0) {
      tcp_client_->run(UV_RUN_NOWAIT);
      VCORE_MS_SLEEP(1);
    }

  } else {
    tcp_client_->waitConnectFinish();
  }
  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) > 0) {
    if (parsedurl.protocol == "https") {
      return this->openssl_->sslConnect();
    }
    return true;
  }
  return false;
}

void VHttpMultiPart::append(const VHttpPart &dataPart) {

  data.appendData("--", 2);
  data.appendData(getMuitiPartString().c_str(), getMuitiPartString().size());
  data.appendData("\r\n", 2);

  std::string key = std::string("Content-Disposition: form-data; name=\"") +
                    dataPart.key + "\"";
  if (!dataPart.file_name.empty()) {
    key += std::string(";filename=\"" + dataPart.file_name + "\"");
  }
  key += std::string("\r\n");
  data.appendData(key.c_str(), key.size());

  if (!dataPart.content_type.empty()) {
    std::string type = std::string("Content-Type: ") + dataPart.content_type +
                       std::string("\r\n");
    data.appendData(type.c_str(), type.size());
  }
  data.appendData("\r\n", 2);
  data.append(dataPart.value);
  data.appendData("\r\n", 2);
}

void VHttpMultiPart::appendFinally(const VHttpPart &dataPart) {
  append(dataPart);
  data.appendData("--", 2);
  data.appendData(getMuitiPartString().c_str(), getMuitiPartString().size());
  data.appendData("--", 2);
}

const VBuf &VHttpMultiPart::getConstData() const { return data; }

VHttpMultiPart::VHttpMultiPart() { init(); }

VHttpMultiPart::~VHttpMultiPart() {}

void VHttpMultiPart::init() {
  multi_boundary = VString("boundary_.") + generateBoundary() + VString("==");
  data.clear();
}

const VString VHttpMultiPart::getMuitiPartString() const {

  return multi_boundary;
}

VString VHttpMultiPart::generateBoundary() {
  // 生成一个足够长的随机字符串作为 boundary
  static const std::string charset =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const int length = 32; // 设置 boundary 的长度
  std::string boundary;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, charset.length() - 1);

  for (int i = 0; i < length; ++i) {
    boundary += charset[dis(gen)];
  }

  return boundary;
}

// 辅助函数：解析 Content-Disposition 参数

void VHttpMultiPart::parseContentDisposition(const VString &header_value,
                                             VString &name, VString &filename) {
  VString::Encoding encoding = header_value.detectEncoding();
  VString decoded_value = header_value.trimmedAuto();

  // 按分号分割参数
  std::vector<VString> params = decoded_value.split(';');
  for (const VString &param : params) {
    VString trimmed_param = param.trimmedAuto();
    if (trimmed_param.empty())
      continue;

    // 提取键值对（如 name="value"）
    size_t eq_pos = trimmed_param.find('=');
    if (eq_pos == VString::npos)
      continue;

    VString key = trimmed_param.substr(0, eq_pos).trimmedAuto();
    VString value = trimmed_param.substr(eq_pos + 1).trimmedAuto();

    // 去除引号（如 "value" -> value）
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
      value = value.substr(1, value.size() - 2);
    }

    if (key == "name") {
      name = value;
    } else if (key == "filename") {
      filename = value.toUtf8String(); // 统一转为UTF-8
    }
  }
}

// 新增方法：从 Content-Type 中解析 boundary

VString
VHttpMultiPart::parseBoundaryFromContentType(const VString &contentType) {
  VString lowerContentType = contentType.toLower();
  if (!lowerContentType.contains("multipart/")) {
    return ""; // 非 multipart 类型
  }

  // 提取 boundary 参数（支持 boundary= 或 BOUNDARY=）
  std::vector<VString> params = contentType.split(';');
  for (const VString &param : params) {
    VString trimmedParam = param.trimmedAuto();
    if (trimmedParam.startsWith("boundary=")) {
      VString boundaryValue = trimmedParam.substr(9).trimmedAuto();

      // 去除可能的引号（如 boundary="----abc"）
      if (boundaryValue.size() >= 2 && boundaryValue.front() == '"' &&
          boundaryValue.back() == '"') {
        boundaryValue = boundaryValue.substr(1, boundaryValue.size() - 2);
      }
      return boundaryValue;
    }
  }
  return "";
}

// 解析单个Part的头部和内容

void VHttpMultiPart::processPart(const VString &partData) {
  size_t header_end = partData.find("\r\n\r\n");
  if (header_end == VString::npos)
    return;

  VString headers_str = partData.substr(0, header_end);
  VString content = partData.substr(header_end + 4); // 内容部分

  VHttpPart part;
  part.content_type = "application/octet-stream"; // 默认值

  // 解析头部
  std::vector<VString> headers = headers_str.split("\r\n");
  for (const VString &header_line : headers) {
    size_t colon_pos = header_line.find(':');
    if (colon_pos == VString::npos)
      continue;

    VString header_name = header_line.substr(0, colon_pos).trimmedAuto();
    VString header_value = header_line.substr(colon_pos + 1).trimmedAuto();

    if (header_name == "Content-Disposition") {
      VString name, filename;
      parseContentDisposition(header_value, name, filename);
      part.key = name;
      part.file_name = filename;
    } else if (header_name == "Content-Type") {
      part.content_type = header_value;
    }
  }

  // 将内容存储为VBuf
  part.value = VBuf(content.data(), content.size());
  http_parts.push_back(part);
}

// 主解析方法（新增 contentType 参数）

bool VHttpMultiPart::parseFromBodyData(const VBuf &bodyData,
                                       const VString &contentType) {
  // 优先从 Content-Type 解析 boundary
  std::string boundary = parseBoundaryFromContentType(contentType);
  if (!boundary.empty()) {
    multi_boundary = boundary;
  } else if (multi_boundary.empty()) {
    return false; // 无可用 boundary
  }

  // 后续逻辑与原实现一致...
  VString body = VString(bodyData.getConstData(), bodyData.size());
  VString boundaryDelimiter = VString("--") + VString(multi_boundary.c_str());

  // 查找结束分隔符（--boundary--）
  VString endDelimiter = boundaryDelimiter + VString("--");
  size_t endPos = body.find(endDelimiter);
  if (endPos == VString::npos)
    return false;

  // 处理每个 Part
  size_t currentPos = 0;
  while (currentPos < endPos) {
    size_t partStart = body.find(boundaryDelimiter + "\r\n", currentPos);
    if (partStart == VString::npos)
      break;

    partStart += boundaryDelimiter.size() + 2; // 跳过分隔符和换行
    size_t partEnd = body.find(boundaryDelimiter, partStart);
    if (partEnd == VString::npos)
      partEnd = endPos;

    VString partData = body.substr(partStart, partEnd - partStart - 2);
    processPart(partData);

    currentPos = partEnd;
  }
  return true;
}

// 设置Boundary

void VHttpMultiPart::setBoundary(const VString &boundary) {
  multi_boundary = boundary;
}

std::vector<VHttpPart> &VHttpMultiPart::getHttpParts() { return http_parts; }
