﻿#pragma once
#include "VHttpClient.h"

VHttpClient::VHttpClient()
    : tcp_client_(new VTcpClient()),
      request_(new VHttpRequest(tcp_client_)),
      response_(new VHttpResponse(tcp_client_)),
      own_http_client_(true) {}

VHttpClient::VHttpClient(VTcpClient* tcp_client)
    : tcp_client_(tcp_client),
      request_(new VHttpRequest(tcp_client)),
      response_(new VHttpResponse(tcp_client)),
      own_http_client_(false) {}

VHttpClient::~VHttpClient() {
  if (own_http_client_ && tcp_client_ != nullptr) {
    delete tcp_client_;
  }
  delete request_;
  delete response_;
}

VHttpRequest* VHttpClient::getVHttpRequest() {
  return request_;
}

VHttpResponse* VHttpClient::getVHttpResponse() {
  return response_;
}

VTcpClient* VHttpClient::getVTcpClient() {
  return tcp_client_;
}

void VHttpClient::initCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setConnectiondCb([this](int status) {
      if (status < 0) {
        error_message =
            "tcpClient Connectiond error status:" + std::to_string(status);
      }
      if (this->http_client_connectiond_cb)
        this->http_client_connectiond_cb(status);

    });
  }
}

std::string VHttpClient::getUrlFileName() {
  int pos = request_->getUrl().find_last_of("/");
  if (pos >= request_->getUrl().size()) {
    return std::string();
  }
  std::string filename(request_->getUrl().substr(pos + 1));
  return filename;
}




// 同步请求

bool VHttpClient::sendRequest(
    const METHOD_TYPE& method,
    const std::string& url,
    const std::map<std::string, std::string>& headers,
    const VBuf& body,
    const uint64_t& maxTimeout) {
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

   if (!tcp_client_->getVLoop()->isActive()) {
    //tcp_client_->run(uv_run_mode::UV_RUN_ONCE);

    uint64_t timeout = 0;
    while (!response_->isParser() && timeout < maxTimeout) {
      tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);
      std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
      timeout++;
    }


  } else {
     // 等待响应
     response_->waitRecvFinish(maxTimeout);
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

bool VHttpClient::sendRequest(const std::string& url) {
  if (tcp_client_ == nullptr || request_ == nullptr || response_ == nullptr) {
    return false;
  }
  response_->initData();
  response_->initCallback();
  response_->initRecvCallback();

  request_->initCallback();
  request_->initSendCallback();

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

  if (!request_->sendRequest(false)) {
    return false;
  }

  if (!tcp_client_->getVLoop()->isActive()) {
    tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
  } else {
    tcp_client_->waitWriteFinish();
  }

  return true;
}

bool VHttpClient::sendBody(const VBuf& body) {

  cache_body_.clear();
  request_->setBody(body);
  if (!request_->sendRequestBody(cache_body_, true)) {
    return false;
  }

  if (!tcp_client_->getVLoop()->isActive()) {
    tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
  } else {
    tcp_client_->waitWriteFinish();
  }
  return true;
}

void VHttpClient::readStart() {
  tcp_client_->clientReadStart();
}

size_t VHttpClient::waitRecvResponse(const uint64_t& maxTimeout) {

  if (!tcp_client_->getVLoop()->isActive()) {
    // tcp_client_->run(uv_run_mode::UV_RUN_ONCE);

    uint64_t timeout = 0;
    while (!response_->isParser() &&
           timeout < maxTimeout) {
      tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      timeout++;
      if (response_->getBody().size() > 0)
        break;
    }

  } else {
    // 等待响应
    response_->waitRecvFinish(maxTimeout);
  }


  if ((tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    tcp_client_->close();
    return -1;
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
  VBuf& recvBuf = response_->recvBody();

  if (count > recvBuf.size() || count == 0) {
    count = recvBuf.size();
    retBuf = recvBuf;
    recvBuf.clear();
  } else if (count < recvBuf.size()) {
    VBuf newBuf;
    retBuf.appandData(recvBuf.getConstData(), count);
    newBuf.appandData(recvBuf.getConstData() + count, recvBuf.size() - count);
    recvBuf = newBuf;
  } else {
    retBuf = recvBuf;
    recvBuf.clear();
  }
  return retBuf;
}

void VHttpClient::clearRecvResponseBody() {
  response_->recvBody().clear();
}

// 异步请求
void VHttpClient::sendRequestAsync(
    const METHOD_TYPE& method,
    const std::string& url,
    const std::map<std::string, std::string>& headers,
    const VBuf& body,
    const uint64_t& maxTimeout) {



}


void VHttpClient::setConnectiondCb(std::function<void(int)> connectiond_cb) {
  http_client_connectiond_cb = connectiond_cb;
}

void VHttpClient::setRequestSendFinishCb(
    std::function<void(int)> request_send_finish_cb) {
  http_client_request_send_finish_cb = request_send_finish_cb;
  request_->setRequestSendFinishCb(request_send_finish_cb);
}

void VHttpClient::setRequestParserFinishCb(
    std::function<void(int)> request_parser_finish_cb) {
  http_client_request_parser_finish_cb = request_parser_finish_cb;
  request_->setRequestParserFinishCb(request_parser_finish_cb);
}

void VHttpClient::setResponseSendFinishCb(
    std::function<void(int)> response_send_finish_cb) {
  http_client_response_send_finish_cb = response_send_finish_cb;
  response_->setResponseSendFinishCb(response_send_finish_cb);
}

void VHttpClient::setResponseParserFinishCb(
    std::function<void(int)> response_parser_finish_cb) {
  http_client_response_parser_finish_cb = response_parser_finish_cb;
  response_->setResponseParserFinishCb(response_parser_finish_cb);
}



bool VHttpClient::connect(const std::string& url) {
  request_->setUrl(url);
  ParsedURL parsedurl = request_->getParsedURL();
  std::string addrs;
  int port;
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

  return true;
}
