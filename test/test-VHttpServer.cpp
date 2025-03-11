// VHttpProxyServer.h
#pragma once
#include "VHttpClientObject.h"
#include "VHttpServer.h"
#include <string>

class VHttpProxyServer {
public:
  explicit VHttpProxyServer(const std::string &backend_url)
      : http_server_(),backend_url_(backend_url) {

    http_server_.setNewHttpClientCb(
        [=](VHttpClient *client) { handleNewClient(client); });
  }
  int listenIpv4(const char *addripv4, int port, int flags = 0) {
   return http_server_.listenIpv4(addripv4, port, flags);
  }
  int run(uv_run_mode md = uv_run_mode::UV_RUN_DEFAULT) {
    return http_server_.run(md);
  }

protected:
  void runClientKeepAlive(VHttpClientObject *clientObj,
                          VHttpClientObject *backendObj, uint64_t maxTimeout) {
    // 启动超时定时器
    if (clientObj == nullptr || clientObj->client == nullptr ||
        backendObj == nullptr || backendObj->client == nullptr) {
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
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        },
        maxTimeout, 0);

    backendObj->_timeout_timer =
        new VTimer(backendObj->client->getVTcpClient()->getVLoop());
    backendObj->_timeout_timer->setData(backendObj);
    backendObj->_timeout_timer->start(
        [=](VTimer *timer) {
          auto obj = static_cast<VHttpClientObject *>(timer->getData());
          if (VTimerInfo::getCurrentTimestamp() - obj->_last_active_time >
              maxTimeout) {
            obj->client->close();
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        },
        maxTimeout, 0);

    // 事件循环处理
    while (!clientObj->client_close || !backendObj->client_close) {
      if (clientObj->client != nullptr && !clientObj->client_close) {
        clientObj->client->getVTcpClient()->run(uv_run_mode::UV_RUN_NOWAIT);
        if (backendObj->client != nullptr && backendObj->client_close) {
          clientObj->client->close();
        }
      }

      if (backendObj->client != nullptr && !backendObj->client_close) {
        backendObj->client->getVTcpClient()->run(uv_run_mode::UV_RUN_NOWAIT);
        if (clientObj->client != nullptr && clientObj->client_close) {
          backendObj->client->close();
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    clientObj->_timeout_timer->stop();
    delete clientObj->_timeout_timer;
    clientObj->_timeout_timer = nullptr;

    backendObj->_timeout_timer->stop();
    delete backendObj->_timeout_timer;
    backendObj->_timeout_timer = nullptr;
  }
  void handleNewClient(VHttpClient *client) {
    // 创建到后端服务器的连接
    VHttpClient *backend = new VHttpClient();
    VHttpClientObject *clientObj = new VHttpClientObject();
    clientObj->client = client;
    client->getVHttpRequest()->setData(clientObj);
    client->getVHttpResponse()->setData(clientObj);
    VHttpClientObject *backendObj = new VHttpClientObject();
    backendObj->client = backend;
    backend->getVHttpRequest()->setData(backendObj);
    backend->getVHttpResponse()->setData(backendObj);

    // 设置后端连接回调
    setupBridge(client, backend);
    client->getVHttpRequest()->initRecvCallback();
    backend->readStart();
    client->readStart();

    backend->getVHttpResponse()->waitRecvHeadersFinish();

    runClientKeepAlive(clientObj, backendObj, 30000);

    delete clientObj;
    delete backendObj;
  }

  void setupBridge(VHttpClient *client, VHttpClient *backend) {
    // 客户端 -> 后端 请求转发
    client->getVHttpRequest()->setRequestParserHeadersFinishCb(
        [this, client, backend](VHttpRequest *req, HttpHeaders *headers) {
          auto obj = static_cast<VHttpClientObject *>(req->getData());
          obj->_last_active_time = VTimerInfo::getCurrentTimestamp();
          client->getVHttpResponse()->resetParser();
          backend->getVHttpRequest()->resetParser();

          
       
          for (auto kv : *headers) {
            
            if (VString(kv.first).toLower() == VString("location")) {
              std::string recvPerIP;
              int recvPerPort;
              client->getVTcpClient()->getLocalAddrs(recvPerIP, recvPerPort);
              VString replace = VString::Format(
                  "http%s://%s:%d",
                  client->getVHttpRequest()->getHttpSsl() ? "s" : "",
                  recvPerIP.c_str(), recvPerPort);
              VString value =
                  VString(kv.second).replaceAll(backend_url_, replace);
              backend->getVHttpRequest()->addHeader(kv.first, value);
            } else {
              backend->getVHttpRequest()->addHeader(kv.first, kv.second);
            }
            //Log->logDebug("客户端 -> 后端 %s:%s", kv.first.c_str(), kv.second.c_str());
          }
          backend->getVHttpRequest()->setHttpVersion(req->getHttpVersion());
          backend->getVHttpRequest()->setMethod(req->getMethod());
          backend->getVHttpRequest()->setUseGzip(req->getUseGzip());
          backend->getVHttpRequest()->setUseChunked(req->getUseChunked());
         
          VString newRawUrl = req->getRawUrl();

          //Log->logInfo("%s%s", backend_url_.c_str(), req->getRawUrl().c_str());
          if (!backend->sendRequest(backend_url_ + newRawUrl.stdString())) {
            backend->close();
          }
        });

    client->getVHttpRequest()->setRequestRecvBodyCb(
        [client, backend](VHttpRequest *req, const VBuf *body) -> bool {
          auto obj = static_cast<VHttpClientObject *>(req->getData());
          obj->_last_active_time = VTimerInfo::getCurrentTimestamp();
          if (!backend->sendRequestBody(*body)) {
            backend->close();
          }
          return true;
        });

    // 后端 -> 客户端 响应转发
    backend->getVHttpResponse()->setResponseParserHeadersFinishCb(
        [client, backend](VHttpResponse *res, HttpHeaders *headers) {
          auto obj = static_cast<VHttpClientObject *>(res->getData());
          obj->_last_active_time = VTimerInfo::getCurrentTimestamp();

          for (auto kv : *headers) {
            client->getVHttpResponse()->addHeader(kv.first, kv.second);
          }

          client->getVHttpResponse()->setHttpVersion(res->getHttpVersion());
          client->getVHttpResponse()->setStatusCode(res->getStatusCode());
          client->getVHttpResponse()->setUseGzip(res->getUseGzip());
          client->getVHttpResponse()->setUseChunked(res->getUseChunked());
         

          if (!client->sendResponse(res->getStatusCode())) {
            client->close();
          }

        });

    backend->getVHttpResponse()->setResponseRecvBodyCb(
        [client, backend](VHttpResponse *res, const VBuf *body) -> bool {
          auto obj = static_cast<VHttpClientObject *>(res->getData());
          obj->_last_active_time = VTimerInfo::getCurrentTimestamp();

          if (!client->sendResponseBody(*body))
          {
            client->close();
          }
          return true;
        });
    backend->getVHttpResponse()->setResponseParserFinishCb(
        [client, backend](VHttpResponse *res, int) {
          auto obj = static_cast<VHttpClientObject *>(res->getData());
          obj->_last_active_time = VTimerInfo::getCurrentTimestamp();

          if (backend->getVHttpResponse()->getUseChunked()) {
            if (!client->sendResponseBody(VBuf(), true)) {
              client->close();
            }
          }
          
          return;
        });
    

    // 双向关闭监听
    client->setCloseCb([client, backend](VHttpClient * client, int) {
      auto obj = static_cast<VHttpClientObject *>(
          client->getVHttpRequest()->getData());
      obj->client_close = true;

    });

    backend->setCloseCb([client, backend](VHttpClient *backend, int) {
      auto obj = static_cast<VHttpClientObject *>(
          backend->getVHttpResponse()->getData());
      obj->client_close = true;
    });
  }

private:
  VHttpServer http_server_;
  std::string backend_url_;
};

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <listen_ip> <listen_port> <backend_url>\n"
              << "Example: " << argv[0]
              << " 0.0.0.0 8075 http://127.0.0.1:8085\n";
    return 1;
  }

  // 创建代理服务器
  VHttpProxyServer proxy(argv[3]);

  // 启动监听
  if (proxy.listenIpv4(argv[1], std::stoi(argv[2]), 0) != 0) {
    std::cerr << "Failed to start server on " << argv[1] << ":" << argv[2]
              << std::endl;
    return 2;
  }

  std::cout << "Proxy server running at " << argv[1] << ":" << argv[2]
            << "\nForwarding to " << argv[3] << std::endl;

  return proxy.run();
}