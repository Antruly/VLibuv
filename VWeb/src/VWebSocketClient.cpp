#pragma once
#include "VWebSocketClient.h"
#include <cassert>
#include <random>

VWebSocketClient::VWebSocketClient()
    : VHttpClient(),
      websocket_parser_(new VWebSocketParser()),
      base64_(),
      websocket_body_(),
      websocket_url_parser_(),
      websocket_url_(),
      websocket_version_(WebSocketVersion::Version13),
      max_body_cache_length_(VWEB_HTTP_MAX_BODY_CACHE_LENGTH) {}

VWebSocketClient::VWebSocketClient(VTcpClient* tcp_client)
    : VHttpClient(tcp_client),
      websocket_parser_(new VWebSocketParser()),
      base64_(),
      websocket_body_(),
      websocket_url_parser_(),
      websocket_url_(),
      websocket_version_(WebSocketVersion::Version13),
      max_body_cache_length_(VWEB_HTTP_MAX_BODY_CACHE_LENGTH) {}

VWebSocketClient::~VWebSocketClient() {
  delete websocket_parser_;
  websocket_parser_ = nullptr;
}

VWebSocketParser* VWebSocketClient::getVWebSocketParser() const {
  return websocket_parser_;
}

void VWebSocketClient::initData() {
   websocket_body_.clear();
  websocket_url_parser_.protocol = "";
   websocket_url_parser_.host = "";
  websocket_url_parser_.port = 0;
  websocket_url_parser_.path = "";
   websocket_url_ = "";
   max_body_cache_length_ = VWEB_HTTP_MAX_BODY_CACHE_LENGTH;
   websocket_version_ = WebSocketVersion::Version13;
 parser_finish_ = false;
   send_close_ = false;
  

}

void VWebSocketClient::initWebsocketCallback(bool isSsl) {
  assert(websocket_parser_ != nullptr);
  assert(this->getVTcpClient() != nullptr);
  websocket_parser_->setFrameHeaderCallback(
      [this](VWebSocketParser* parser) -> int {
        if (parser_finish_)
          parser_finish_ = false;

        if (VWEBSOCKETPARSER_HAS_MASK(parser) &&
                websocket_client_parser_mask_finish_cb)
          websocket_client_parser_mask_finish_cb(
              this,
              parser->getWebSocketParser()->mask);
        return 0;
      });

  websocket_parser_->setFrameBodyCallback([this](VWebSocketParser* parser,
                                                 const char* data,
                                                 size_t len) -> int {
    if (websocket_body_.size() > max_body_cache_length_) {
      error_message =
          "VWebSocketClient body size is to long, max_body_cache_length_ = " +
          std::to_string(max_body_cache_length_) +
          "websocket_body_ size = " + std::to_string(websocket_body_.size());
      Log->logError("%s", error_message.c_str());
    } else {
      websocket_body_.appendData(data, len);
      if (websocket_client_recv_body_cb) {
        if (websocket_client_recv_body_cb(this, &websocket_body_)) {
          websocket_body_.clear();
        }
      }
    }

    return 0;
  });

  websocket_parser_->setFrameEndCallback(
      [this](VWebSocketParser* parser) -> int {
        if (VWEBSOCKETPARSER_HAS_FINAL(parser) &&
            websocket_client_parser_final_finish_cb) {
          websocket_client_parser_final_finish_cb(this, 0);
        }

        parser_mutex_.lock();
        if (websocket_client_parser_finish_cb) {
          websocket_client_parser_finish_cb(this, 0);
        }
        parser_finish_ = true;
        if (this->async_recv != nullptr)
          this->async_recv->send();
        parser_mutex_.unlock();

        if (VWEBSOCKETPARSER_HAS_CLOSE(parser) &&
            websocket_client_close_cb &&
            !send_close_) {
          websocket_client_close_cb(this, parser->getWebSocketParser()->flags);
          send_close_ = this->websocketSendClose();
          this->getVTcpClient()->close();
          this->getVTcpClient()->waitCloseFinish();
        }
        return 0;
      });

  

  if (isSsl) {
    assert(this->getVOpenSsl() != nullptr);
    this->getVOpenSsl()->setSslWriteCb([this](VOpenSsl* ssl, const VBuf* data,
                                              int status) {
      if (status < 0) {
        error_message =
            "ssl VWebSocketClient Write error status:" + std::to_string(status);
      }

      if (websocket_client_send_finish_cb)
        websocket_client_send_finish_cb(this, status);
    });

    this->getVOpenSsl()->setSslReadCb([this](VOpenSsl* ssl, const VBuf* data) {
      websocket_parser_->websocketParserExecute(data->getConstData(),
                                                data->size());
    });
  } else {
    this->getVTcpClient()->setWriteCb(
        [this](VTcpClient* tcp_client, const VBuf* data, int status) {
          if (status < 0) {
            error_message =
                "VWebSocketClient Write error status:" + std::to_string(status);
          }

          if (websocket_client_send_finish_cb)
            websocket_client_send_finish_cb(this, status);
        });

    this->getVTcpClient()->setReadCb(
        [this](VTcpClient* tcp_client, const VBuf* data) {
          websocket_parser_->websocketParserExecute(data->getConstData(),
                                                    data->size());
        });
  }
}

std::string VWebSocketClient::generateWebSocketKey() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 255);

  VBuf outBuf;
  VBuf randBuf;
  randBuf.resize(16);
  for (int i = 0; i < 16; ++i) {
    randBuf.getData()[i] = dis(gen);
  }
  base64_.encoding(randBuf, outBuf);
  return outBuf.toString();
}

void VWebSocketClient::setWebsocketConnectiondCb(
    std::function<void(VWebSocketClient*, int)> connectiond_cb) {
  websocket_client_connectiond_cb = connectiond_cb;
}

void VWebSocketClient::setWebsocketSendFinishCb(
    std::function<void(VWebSocketClient*, int)> send_finish_cb) {
  websocket_client_send_finish_cb = send_finish_cb;
}

void VWebSocketClient::setWebsocketParserFinishCb(
    std::function<void(VWebSocketClient*, int)> parser_finish_cb) {
  websocket_client_parser_finish_cb = parser_finish_cb;
}

void VWebSocketClient::setWebsocketParserMaskFinishCb(
    std::function<void(VWebSocketClient*, const char[4])>
        parser_mask_finish_cb) {
  websocket_client_parser_mask_finish_cb = parser_mask_finish_cb;
}

void VWebSocketClient::setWebsocketRecvBodyCb(
    std::function<bool(VWebSocketClient*, const VBuf*)> recv_body_cb) {
  websocket_client_recv_body_cb = recv_body_cb;
}

void VWebSocketClient::setWebsocketParserFinalFinishCb(
    std::function<void(VWebSocketClient*, int)> parser_final_finish_cb) {
  websocket_client_parser_final_finish_cb = parser_final_finish_cb;
}

void VWebSocketClient::setWebsocketCloseCb(
    std::function<void(VWebSocketClient*, int)> close_cb) {
  websocket_client_close_cb = close_cb;
}

void VWebSocketClient::setWebsocketParserHeaderCb(
    std::function<void(VWebSocketClient*, websocket_flags)> parser_header_cb) {
  websocket_client_parser_header_cb = parser_header_cb;
}

bool VWebSocketClient::writeData(const VBuf& sendBuf) {
  if (websocket_url_parser_.protocol == "wss") {
    assert(this->getVOpenSsl() != nullptr);
    VBuf sslBuf;
    return this->getVOpenSsl()->sslPackData(sendBuf, sslBuf);
  } else {
    this->getVTcpClient()->writeData(sendBuf);
    if (!this->getVTcpClient()->getVLoop()->isActive()) {
      this->getVTcpClient()->run(uv_run_mode::UV_RUN_ONCE);
    } else {
      this->getVTcpClient()->waitWriteFinish();
    }
    return true;
  }
}

bool VWebSocketClient::websocketConnect(const std::string& url) {
  initData();
  websocket_url_parser_ = websocket_parser_->parseWebSocketUrl(url);
  if (websocket_url_parser_.protocol != "ws" &&
      websocket_url_parser_.protocol != "wss") {
    error_message = "WebSocketURL is error :" + url;
    return false;
  }
  std::string strHttpUrl = "http" + std::string(url.c_str() + 2);

  this->getVHttpRequest()->setMethod(METHOD_TYPE::GET);

  this->getVHttpRequest()->addHeader("Upgrade", "websocket");
  this->getVHttpRequest()->addHeader("Connection", "Upgrade");
  this->getVHttpRequest()->addHeader("Sec-WebSocket-Key",
                                     this->generateWebSocketKey());
  this->getVHttpRequest()->addHeader(
      "Sec-WebSocket-Version",
      std::to_string(static_cast<int>(websocket_version_)));

  if (!this->sendRequest(strHttpUrl)) {
    error_message = "http request is error :" + strHttpUrl;
    return false;
  }

  VBuf recvBody;
  size_t index = this->waitRecvResponse();
  int statusCode = this->getVHttpResponse()->getStatusCode();
  HttpHeaders responseHeaders =
      this->getVHttpResponse()->getHeaders();

  // Log->logDebug("response Headers:%d\n", statusCode);
  /*for (auto header : responseHeaders) {
    Log->logDebug("%s: %s\n", header.first.c_str(), header.second.c_str());
  }*/

  // Log->logDebug("response Status:%d\n", statusCode);
  while (true) {
    switch (statusCode) {
      case 101: {
        this->getRecvResponseBody();
        this->initWebsocketCallback(websocket_url_parser_.protocol == "wss");

        if (websocket_client_connectiond_cb)
          websocket_client_connectiond_cb(this, 0);
      }
        return true;
      case 301:
      case 302:
      case 303:
      case 307:
      case 308: {
        // Log->logDebug("response Status:%d Location:%s\n",
        // statusCode,this->getVHttpResponse()->getLocation().c_str());
        int bRet = this->sendRequest(this->getVHttpResponse()->getLocation());
        if (!bRet) {
          error_message = "http request is error :" +
                          this->getVHttpResponse()->getLocation();
          break;
        }
        this->readStart();
        index = this->waitRecvResponse();
        statusCode = this->getVHttpResponse()->getStatusCode();
        responseHeaders = this->getVHttpResponse()->getHeaders();
      }
        continue;
      case 404: {
      } break;
      default:
        break;
    }
    break;
  }
  if (websocket_client_connectiond_cb)
    websocket_client_connectiond_cb(this, -1);
  return false;
}

bool VWebSocketClient::websocketSendText(const std::string& data,
                                         bool final,
                                         bool mask) {
  websocket_flags flags = WS_OP_TEXT;
  if (final) {
    flags = static_cast<websocket_flags>(flags | WS_FINAL_FRAME);
  }
  if (mask) {
    flags = static_cast<websocket_flags>(flags | WS_HAS_MASK);
  }

  VBuf vbuf(data);
  return this->websocketSend(vbuf, flags);
}

bool VWebSocketClient::websocketSendBinary(const VBuf& data,
                                           bool final,
                                           bool mask) {
  websocket_flags flags = WS_OP_BINARY;
  if (final) {
    flags = static_cast<websocket_flags>(flags | WS_FINAL_FRAME);
  }
  if (mask) {
    flags = static_cast<websocket_flags>(flags | WS_HAS_MASK);
  }

  return this->websocketSend(data, flags);
}

bool VWebSocketClient::websocketSendPing() {
  return this->websocketSend(static_cast<websocket_flags>(WS_OP_PING | WS_FINAL_FRAME));
}

bool VWebSocketClient::websocketSendPong() {
  return this->websocketSend(static_cast<websocket_flags>(WS_OP_PONG | WS_FINAL_FRAME));
}

bool VWebSocketClient::websocketSendClose() {
   return this->websocketSend(static_cast<websocket_flags>(WS_OP_CLOSE | WS_FINAL_FRAME));
}

bool VWebSocketClient::websocketSendContinue(const VBuf& data,
                                             bool final,
                                             bool mask) {
  websocket_flags flags = WS_OP_CONTINUE;
  if (final) {
    flags = static_cast<websocket_flags>(flags | WS_FINAL_FRAME);
  }
  if (mask) {
    flags = static_cast<websocket_flags>(flags | WS_HAS_MASK);
  }

  return this->websocketSend(data, flags);
}

bool VWebSocketClient::websocketSend(websocket_flags flags) {
  return websocketSend(VBuf(), flags);
}

bool VWebSocketClient::websocketSend(const VBuf& data, websocket_flags flags) {
  char mask[4] = {0};
  if ((flags & WS_HAS_MASK) > 0) {
    websocket_parser_->generateMask(mask);
  }
  return this->websocketSend(data, flags, mask);
}

bool VWebSocketClient::websocketSend(const VBuf& data,
                                     websocket_flags flags,
                                     char mask[4]) {
  assert(websocket_parser_ != nullptr);
  VBuf frmaeData;
  frmaeData.resize(data.size() + VWEBSOCKETPARSER_MINI_CACHE_SIZE);
  size_t frmaeSize = websocket_parser_->websocketBuildFrame(
      frmaeData.getData(), flags, mask, data.getConstData(), data.size());
  frmaeData.resize(frmaeSize);

  return this->writeData(frmaeData);
}

uint64_t VWebSocketClient::websocketPing() {
  std::chrono::steady_clock::time_point pingTime =
      std::chrono::steady_clock::now();

  if (!websocketSendPing()) {
    return -1;
  }

  if (this->waitRecvWebsocketFrame() == -1) {
    return -1;
  }

  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  std::chrono::milliseconds delay =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - pingTime);
  return delay.count();
}

void VWebSocketClient::websocketClose() {
  send_close_ = this->websocketSendClose();

  if (send_close_)
  this->waitRecvWebsocketFrame();

  this->getVTcpClient()->close();
  this->getVTcpClient()->waitCloseFinish();
}

bool VWebSocketClient::isParser() {
  return parser_finish_;
}

void VWebSocketClient::waitRecvNewBody(const uint64_t& maxTimout) {
  if (this->getVTcpClient() == nullptr) {
    return;
  }
  if ((this->getVTcpClient()->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return;
  }
  parser_mutex_.lock();
  if (websocket_body_.size() > 0 && this->isParser()) {
    parser_mutex_.unlock();
    return;
  }

  assert(this->async_recv == nullptr);

  
  VLoop vloop;
  this->async_recv = new VAsync();
  VTimer vtimer(&vloop);
  uint64_t curruntTime = 0;
  vtimer.start(
      [this, &vloop, &curruntTime, &maxTimout](VTimer* vtimer) {
        curruntTime++;
        if (websocket_body_.size() > 0 || curruntTime > maxTimout ||
            ((this->getVTcpClient()->getStatus() &
              VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0)) {
          vtimer->stop();
          vloop.close();
        }
      },
      0ULL, 1ULL);
  VAsync* vasync = this->async_recv;
  this->async_recv->init(
      [this, &vtimer](VAsync* vasync) {
        vtimer.stop();
        vasync->close();
      },
      &vloop);

  parser_mutex_.unlock();
  vloop.run();
  this->async_recv = nullptr;
  delete vasync;
}

size_t VWebSocketClient::waitRecvWebsocketFrame(const uint64_t& maxTimeout) {
  if (!this->getVTcpClient()->getVLoop()->isActive()) {
    // this->getVTcpClient()->run(uv_run_mode::UV_RUN_ONCE);

    uint64_t timeout = 0;
    while (!this->isParser() && timeout < maxTimeout) {
      this->getVTcpClient()->run(uv_run_mode::UV_RUN_NOWAIT);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      timeout++;
      if (websocket_body_.size() > 0 && this->isParser())
        break;
    }

  } else {
    // 等待响应
    this->waitRecvNewBody(maxTimeout);
  }

  if ((this->getVTcpClient()->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    this->getVTcpClient()->close();
    return -1;
  }

  return websocket_body_.size();
}

VBuf VWebSocketClient::getRecvWebsocketBody(size_t count) {
  VBuf retBuf;
  VBuf& recvBuf = websocket_body_;

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

void VWebSocketClient::clearRecvWebsocketBody() {
  websocket_body_.clear();
}
