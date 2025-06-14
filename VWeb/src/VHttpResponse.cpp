#pragma once
#include "VHttpResponse.h"
#include <VTimer.h>
#include <VTimerInfo.h>
#include <cassert>
#include <../VVersion.h>

VHttpResponse::VHttpResponse(VTcpClient *vtcp_client)
    : tcp_client_(vtcp_client), own_tcp_client_(false),
      http_parser_(new VHttpParser()), zlib_(new VZlib()), response_data_(),
      max_body_cache_length_(1024 * 1024 * 100), body_(), body_compress_() {
  this->initCallback();
  http_parser_->httpParserInit(http_parser_type::HTTP_RESPONSE);
  this->zlib_->initDecompressStream();
  this->initData();
}

VHttpResponse::~VHttpResponse() {
  response_data_.clear();
  body_.clear();
  body_compress_.clear();
  if (own_tcp_client_) {
    delete tcp_client_;
  }
  delete http_parser_;
  delete zlib_;
}

void VHttpResponse::initRecvCallback() {
  if (http_ssl_) {
    assert(openssl_ != nullptr);
    openssl_->setSslReadCb(
        [this](VOpenSsl *ssl, const VBuf *data) { this->parseResponse(data); });
  } else if (tcp_client_ != nullptr) {
    tcp_client_->setReadCb([this](VTcpClient *tcp_client, const VBuf *data) {
      this->parseResponse(data);
    });
  }
}

void VHttpResponse::initSendCallback() {
  if (http_ssl_) {
    assert(openssl_ != nullptr);
    openssl_->setSslWriteCb(
        [this](VOpenSsl *ssl, const VBuf *data, int status) {
          if (status < 0) {
            error_message =
                "ssl Response Write error status:" + std::to_string(status);
          }
        });
  } else if (tcp_client_ != nullptr) {
    tcp_client_->setWriteCb([this](VTcpClient *tcp_client, const VBuf *data,
                                   int status) {
      if (status < 0) {
        error_message = "Response Write error status:" + std::to_string(status);
      }
    });
  }
}

void VHttpResponse::initCallback() {
  if (http_parser_ != nullptr) {
    // Message begin callback
    http_parser_->setMessageBeginCallback([this](
                                              VHttpParser *http_parser) -> int {
      parser_finish_ = false;
      parser_header_finish_ = false;
      return 0; // Example return value, replace with your actual return value
    });

    // URL callback
    http_parser_->setUrlCallback([this](VHttpParser *http_parser,
                                        const char *data, size_t size) -> int {
      return 0; // Example return value, replace with your actual return value
    });

    // Status callback
    http_parser_->setStatusCallback([this](VHttpParser *http_parser,
                                           const char *data,
                                           size_t size) -> int {
      status_message = std::string(data, size);
      status_ = http_parser->getHttpParser()->status_code;
      return 0; // Example return value, replace with your actual return value
    });

    // Header field callback
    http_parser_->setHeaderFieldCallback([this](VHttpParser *http_parser,
                                                const char *data,
                                                size_t size) -> int {
      header_cache_ = std::string(data, size);

      return 0; // Example return value, replace with your actual return value
    });

    // Header value callback
    http_parser_->setHeaderValueCallback([this](VHttpParser *http_parser,
                                                const char *data,
                                                size_t size) -> int {
      std::string header_value = std::string(data, size);
      VString header_key_low = VString(header_cache_).toLower();
      VString header_value_low = VString(header_value).toLower();

      if (header_key_low.empty()) {
        return -1;
      } else if (header_key_low == "content-type") {
        contentType_ = header_value;
      } else if (header_key_low == "connection") {
        keep_alive_ = header_value_low == "keep-alive" ? true : false;
      } else if (header_key_low == "server") {
        server_ = header_value;
      } else if (header_key_low == "content-encoding") {
        if (header_value.find("gzip") != std::string::npos) {
          use_gzip_ = true;
        } else {
          use_gzip_ = false;
        }
      } else if (header_key_low == "content-language") {
        content_language_ = header_value;
      } else if (header_key_low == "location") {
        location_ = header_value;
      } else if (header_key_low == "content-length") {
        content_length_ = std::stoul(header_value);
      } else if (header_key_low == "transfer-encoding") {
        if (header_value_low.find("chunked") != std::string::npos) {
          use_chunked_ = true;
        } else {
          use_chunked_ = false;
        }
      } else if (header_key_low == "accept_ranges") {
        accept_ranges_ = header_value;
      } else if (header_key_low == "set-cookie") {
        Cookie cookie(http_parser->parseSetCookieHeader(header_value));
        cookies_.addCookie(cookie);
      }

      addHeader(header_cache_, header_value);
      header_cache_ = "";
      return 0; // Example return value, replace with your actual return value
    });

    // Headers complete callback
    http_parser_->setHeadersCompleteCallback([this](VHttpParser *http_parser)
                                                 -> int {
      if (http_ssl_) {
      }

      http_version_ =
          "HTTP/" + std::to_string(http_parser_->getHttpParser()->http_major) +
          "." + std::to_string(http_parser_->getHttpParser()->http_minor);

      parser_header_finish_ = true;
      if (http_response_parser_headers_finish_cb)
        http_response_parser_headers_finish_cb(this, &headers_);
      // Your headers_complete_cb logic here
      return 0; // Example return value, replace with your actual return value
    });

    // Body callback
    http_parser_->setBodyCallback([this](VHttpParser *http_parser,
                                         const char *data, size_t size) -> int {
      while (body_.size() + size > max_body_cache_length_ &&
             (this->getVTcpClient()->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) > 0) {
        error_message =
            "response body size is to long, max_body_cache_length_ = " +
            std::to_string(max_body_cache_length_) +
            "content_length_ = " + std::to_string(content_length_);
        Log->logError("%s", error_message.c_str());

        if (http_response_recv_body_cb) {
          if (http_response_recv_body_cb(this, &body_))
            body_.clear();
        }
      } 

      if (use_gzip_ && size > 0) {
        VBuf gzipData(data, size);
        if (!zlib_->gzipDecompressChunked(gzipData, body_)) {
          error_message = "gzipDecompress is error";
        }
      } else {
        body_.appendData(data, size);
      }  
     
      if (http_response_recv_body_cb) {
        if (http_response_recv_body_cb(this, &body_))
          body_.clear();
      }

      return 0; // Example return value, replace with your actual return value
    });

    // Message complete callback
    http_parser_->setMessageCompleteCallback([this](VHttpParser *http_parser)
                                                 -> int {
      if (use_gzip_)
        this->zlib_->closeDecompressStream();

      this->parser_finish_ = true;
      this->async_recv_ = true;

      if (http_response_parser_finish_cb) {
        http_response_parser_finish_cb(this, 0);
      }

      return 0; // Example return value, replace with your actual return value
    });

    // Chunk header callback
    http_parser_->setChunkHeaderCallback([this](
                                             VHttpParser *http_parser) -> int {
      // Your chunk_header_cb logic here
      return 0; // Example return value, replace with your actual return value
    });

    // Chunk complete callback
    http_parser_->setChunkCompleteCallback([this](VHttpParser *http_parser)
                                               -> int {
      if (http_response_chunk_complete_cb)
        http_response_chunk_complete_cb(this, 0);
      // Your chunk_complete_cb logic here
      return 0; // Example return value, replace with your actual return value
    });
  }
}

void VHttpResponse::initData() {

  status_message = "";
  http_version_ = "";
  server_ = "";
  header_cache_ = "";
  location_ = "";
  content_length_ = 0;
  status_ = 0;

  headers_.clear();
  body_.clear();
  body_compress_.clear();

  response_data_.clear();
  contentType_ = "";

  use_gzip_ = false;
  keep_alive_ = true;
  parser_finish_ = false;
  parser_header_finish_ = false;
  http_ssl_ = false;
}

void VHttpResponse::setSslPoint(VOpenSsl *ssl) { openssl_ = ssl; }

VOpenSsl *VHttpResponse::getSslPoint() const { return openssl_; }

VTcpClient *VHttpResponse::getVTcpClient() const { return tcp_client_; }

VHttpParser *VHttpResponse::getVHttpParser() const { return http_parser_; }

VZlib *VHttpResponse::getVZlib() const { return zlib_; }

int VHttpResponse::getStatusCode() const { return status_; }

void VHttpResponse::setStatusCode(int statusCode) { status_ = statusCode; }

std::string VHttpResponse::getHttpVersion() const { return http_version_; }

void VHttpResponse::setHttpVersion(const std::string &httpVersion) {
  http_version_ = httpVersion;
}

HttpHeaders VHttpResponse::getHeaders() const {
  return headers_;
}

void VHttpResponse::addHeader(const std::string &key,
                              const std::string &value) {
  headers_[key] = value;
}

const VBuf &VHttpResponse::getBody() const { return body_; }

VBuf &VHttpResponse::recvBody() { return body_; }

VBuf VHttpResponse::popBody() {
  VBuf recvBody(body_);
  body_.clear();
  return recvBody;
}

void VHttpResponse::setBody(const VBuf &body) { body_ = body; }

void VHttpResponse::setBodyCompress(const VBuf &body) {   
    // Generate the request data packet
  if (use_gzip_) {
    // Gzip compress the body
    this->zlib_->gzipCompress(&body, body_compress_);
  }
  // Original body
  body_ = body;
}

void VHttpResponse::setContentType(const std::string &contentType) {
  contentType_ = contentType;
  this->addHeader("Content-Type", contentType_);
}

std::string VHttpResponse::getContentType() const { return contentType_; }

void VHttpResponse::setAcceptRanges(const std::string &acceptRanges) {
  accept_ranges_ = acceptRanges;
  this->addHeader("Accept-Ranges", accept_ranges_);
}

std::string VHttpResponse::getAcceptRanges() const { return accept_ranges_; }

void VHttpResponse::setKeepAlive(bool keepAlive) {
  keep_alive_ = keepAlive;
  this->addHeader("Connection", keep_alive_ ? "keep-alive" : "close");
}

bool VHttpResponse::getKeepAlive() const { return keep_alive_; }

void VHttpResponse::setUseGzip(bool useGzip) {
  use_gzip_ = useGzip;
  if (use_gzip_) {
    this->addHeader("Content-Encoding", "gzip");
    if (headers_["Content-Encoding"].empty());
  } else {
    if (headers_.find("Content-Encoding") != headers_.end() &&
        headers_["Content-Encoding"] == "gzip") {
      headers_.erase("Content-Encoding");
    } 
  }
}

bool VHttpResponse::getUseGzip() const { return use_gzip_; }
bool VHttpResponse::getUseChunked() const { return use_chunked_; }
void VHttpResponse::setUseChunked(bool useChunked) {
  use_chunked_ = useChunked;
  if (use_chunked_) {
    this->addHeader("Transfer-Encoding", "chunked");
  } else {
    if (headers_.find("Transfer-Encoding") != headers_.end()) {
      headers_.erase("Transfer-Encoding");
    }
  }
}
void VHttpResponse::setServer(const std::string &server) {
  server_ = server;
  addHeader("Server", server_);
}

void VHttpResponse::setLocation(std::string location) {
  location_ = location;
  this->addHeader("Location", location_);
}

std::string VHttpResponse::getLocation() const { return location_; }

std::string VHttpResponse::getServer() { return server_; }

size_t VHttpResponse::getContentLength() const { return content_length_; }

void VHttpResponse::setContentLength(const size_t &length) {
  content_length_ = length;
  this->addHeader("Content-Length", std::to_string(content_length_));
}

void VHttpResponse::setHttpSsl(bool isSsl) { http_ssl_ = isSsl; }

bool VHttpResponse::getHttpSsl() const { return http_ssl_; }

bool VHttpResponse::writeData(const VBuf &sendBuf) {
  if (http_ssl_) {
    assert(openssl_ != nullptr);
    VBuf sslBuf;
    return openssl_->sslPackData(sendBuf, sslBuf);
  } else {
    tcp_client_->writeData(sendBuf);
    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->run(uv_run_mode::UV_RUN_ONCE);
    } else {
      tcp_client_->waitWriteFinish();
    }
    return true;
  }
}

bool VHttpResponse::isSecure() const { return false; }

void VHttpResponse::parseResponse(const VBuf *buf) {
  response_data_.clone(*buf);
  if (http_parser_) {
    http_parser_->httpParserExecute(buf->getConstData(), buf->size());
  }
}

void VHttpResponse::resetParser() { this->initData(); }

bool VHttpResponse::sendResponse(bool isSendBody) {
  if (tcp_client_ == nullptr ||
      (tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) !=
          VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) {
    return false;
  }

  if (http_parser_ == nullptr) {
    return false;
  }

  this->initSendCallback();

  if (http_ssl_) {
    assert(openssl_ != nullptr);
    tcp_client_->clientReadStart();
    if (!openssl_->sslIsInitFinished() && !openssl_->sslConnect()) {
      error_message = openssl_->getErrorMassage();
      return false;
    }
  }

  if (isSendBody) {
    if (!this->sendResponseBegin(response_data_, false)) {
      return false;
    }
    if (!this->sendResponseHeader(response_data_, false)) {
      return false;
    }
    if (!this->sendResponseBody(response_data_, true)) {
      return false;
    }
  } else {
    if (!this->sendResponseBegin(response_data_, false)) {
      return false;
    }
    if (!this->sendResponseHeader(response_data_, true)) {
      return false;
    }
  }
  if (http_response_send_finish_cb)
    http_response_send_finish_cb(this, 0);
  return true;
}

bool VHttpResponse::sendResponseBegin(VBuf &sendBuf, bool isSend) {
  // Construct the HTTP response string
  std::string response = http_version_ + " " + std::to_string(status_) + " " +
                         getStatusMessage(status_) + "\r\n";

  sendBuf.appendData(response.c_str(), response.size());
  if (isSend) {
    return this->writeData(sendBuf);
    // tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpResponse::sendResponseHeader(VBuf &sendBuf, bool isSend) {
  std::string response;
  // Add the client identifier
  if (server_.empty()) {
    this->setServer("VHttpServer/" + std::string(VLIBUV_VERSION_STRING));
  }

  // Add content length information
  if (use_chunked_) {
    if (headers_.find("Content-Length") != headers_.end()) {
      headers_.erase("Content-Length");
      content_length_ = -1;
    }
    if (headers_.find("Transfer-Encoding") == headers_.end()) {
      this->addHeader("Transfer-Encoding", "chunked");
    }
  } else {
    content_length_ = -1;
    if (headers_.find("Content-Length") != headers_.end()) {
      content_length_ = std::stoul(headers_["Content-Length"]);
    } else if ((body_compress_.size() > 0)) {
      content_length_ = body_compress_.size();
    } else if ((body_.size() > 0)) {
      content_length_ = body_.size();
    }
    response += "Content-Length: " + std::to_string(content_length_) + "\r\n";
  }

  // Set other request headers
  for (const auto &header : headers_) {
    if (header.first == "Host")
      continue;
    else if (header.first == "Content-Length")
      continue;
    else if (header.first == "Content-Encoding")
      continue;
    else if (header.first == "Accept-Encoding") {
      continue;
    }
    response += header.first + ": " + header.second + "\r\n";
  }
  response += "Accept-Encoding: gzip\r\n";
  if (use_gzip_) {
    response += "Content-Encoding: gzip\r\n";
  }

  response += "\r\n";

  sendBuf.appendData(response.c_str(), response.size());
  if (isSend) {
    return this->writeData(sendBuf);
    // tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpResponse::sendResponseBody(VBuf &sendBuf, bool isSend) {
  if (use_chunked_) {
    return false;
  }
  // Generate the request data packet
  if (use_gzip_ && body_compress_.size() > 0) {
    // Gzip compress the body
    sendBuf.appendData(body_compress_.getConstData(), body_compress_.size());
  } else if (use_gzip_ && body_.size() > 0) {
    // Gzip compress the body
    this->zlib_->gzipCompress(&body_, body_compress_);
    sendBuf.appendData(body_compress_.getConstData(), body_compress_.size());
  } else {
    // Original body
    sendBuf.appendData(body_.getConstData(), body_.size());
  }

  if (isSend) {
    return this->writeData(sendBuf);
    // tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpResponse::sendResponseCRLF(VBuf &sendBuf, bool isSend) {

  std::string request = "\r\n";
  sendBuf.appendData(request.c_str(), request.size());
  return this->writeData(sendBuf);
  // tcp_client_->writeData(sendBuf);
  return true;
}

bool VHttpResponse::sendResponseChunked(VBuf &sendBuf, bool isEnd) {
	sendBuf.clear();
  if (body_.size() > 0 ||body_compress_.size() > 0) {
          // Generate the request data packet
          if (use_gzip_ && body_compress_.size() > 0) {
            // Gzip compress the body
            sendBuf.appendData(body_compress_.getConstData(),
                               body_compress_.size());
          } else if (use_gzip_ && body_.size() > 0) {
            // Gzip compress the body
            this->zlib_->gzipCompress(&body_, body_compress_);
            sendBuf.appendData(body_compress_.getConstData(),
                               body_compress_.size());
          } else {
            // Original body
            sendBuf.appendData(body_.getConstData(), body_.size());
          }

    std::string request = std::to_string(sendBuf.size()) + "\r\n";
    sendBuf.appendData(request.c_str(), request.size());
    sendBuf.appendData("\r\n", 2);
    tcp_client_->writeNewData(sendBuf);
    body_.clear();
    body_compress_.clear();
  }

  if (isEnd) {
    return this->sendResponseChunkedEnd();
  }
  return true;
}

bool VHttpResponse::sendResponseChunkedEnd() {
  const static VBuf endBuf("0\r\n\r\n", 5);
  tcp_client_->writeNewData(endBuf);
  if (http_response_send_finish_cb) {
    http_response_send_finish_cb(this, 0);
  }
  return true;
}

bool VHttpResponse::isParser() { return parser_finish_; }

bool VHttpResponse::isHeaderParser() { return parser_header_finish_; }

std::string VHttpResponse::getStatusMessage(int statusCode) {
  switch (statusCode) {
  case 100:
    return "Continue";
  case 101:
    return "Switching Protocols";
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 202:
    return "Accepted";
  case 203:
    return "Non-Authoritative Information";
  case 204:
    return "No Content";
  case 205:
    return "Reset Content";
  case 206:
    return "Partial Content";
  case 300:
    return "Multiple Choices";
  case 301:
    return "Moved Permanently";
  case 302:
    return "Found";
  case 303:
    return "See Other";
  case 304:
    return "Not Modified";
  case 305:
    return "Use Proxy";
  case 307:
    return "Temporary Redirect";
  case 400:
    return "Bad Request";
  case 401:
    return "Unauthorized";
  case 402:
    return "Payment Required";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 406:
    return "Not Acceptable";
  case 407:
    return "Proxy Authentication Required";
  case 408:
    return "Request Timeout";
  case 409:
    return "Conflict";
  case 410:
    return "Gone";
  case 411:
    return "Length Required";
  case 412:
    return "Precondition Failed";
  case 413:
    return "Payload Too Large";
  case 414:
    return "URI Too Long";
  case 415:
    return "Unsupported Media Type";
  case 416:
    return "Range Not Satisfiable";
  case 417:
    return "Expectation Failed";
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  case 502:
    return "Bad Gateway";
  case 503:
    return "Service Unavailable";
  case 504:
    return "Gateway Timeout";
  case 505:
    return "HTTP Version Not Supported";
  default:
    return "Unknown Status";
  }
}

void VHttpResponse::waitRecvHeadersFinish(const uint64_t &maxTimout) {
  if (this->tcp_client_ == nullptr) {
    return;
  }
  if ((this->tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return;
  }
  if (this->isHeaderParser()) {
    return;
  }

  uint64_t timeout = VTimerInfo::getCurrentTimestamp();
  uint64_t timeSp = 0;
  while (!this->isHeaderParser()) {

    if (timeSp % 10 == 0 &&
        VTimerInfo ::getCurrentTimestamp() - timeout > maxTimout) {
      break;
    }

    if (!tcp_client_->getVLoop()->isActive())
      tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);

    if (body_.size() > 0 ||
        ((this->tcp_client_->getStatus() &
          VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0)) {
      break;
    }
    timeSp++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
size_t VHttpResponse::waitRecvBody(const uint64_t &maxTimout) {
  if (this->tcp_client_ == nullptr) {
    return body_.size();
  }
  if ((this->tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return body_.size();
  }
  if (this->isParser()) {
    return body_.size();
  }

  uint64_t timeout = VTimerInfo::getCurrentTimestamp();
  uint64_t timeSp = 0;
  while (!this->isParser()) {

    if (timeSp % 10 == 0 &&
        VTimerInfo ::getCurrentTimestamp() - timeout > maxTimout) {
      break;
    }

    if (!tcp_client_->getVLoop()->isActive())
      tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);

    if (body_.size() > 0 ||
        ((this->tcp_client_->getStatus() &
          VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0)) {
      break;
    }
    timeSp++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return body_.size();
}
void VHttpResponse::setData(void *data) { this->vdata = data; }
void *VHttpResponse::getData() { return vdata; }
void VHttpResponse::setResponseSendFinishCb(
    std::function<void(VHttpResponse *, int)> response_send_finish_cb) {
  http_response_send_finish_cb = response_send_finish_cb;
}

void VHttpResponse::setResponseParserFinishCb(
    std::function<void(VHttpResponse *, int)> response_parser_finish_cb) {
  http_response_parser_finish_cb = response_parser_finish_cb;
}

void VHttpResponse::setResponseParserHeadersFinishCb(
    std::function<void(VHttpResponse *, HttpHeaders *)>
        response_parser_headers_finish_cb) {
  http_response_parser_headers_finish_cb = response_parser_headers_finish_cb;
}

void VHttpResponse::setResponseRecvBodyCb(
    std::function<bool(VHttpResponse *, const VBuf *)> response_recv_body_cb) {
  http_response_recv_body_cb = response_recv_body_cb;
}

void VHttpResponse::setResponseChunkHeaderCb(
    std::function<void(VHttpResponse *, int)> response_chunk_header_cb) {
  http_response_chunk_header_cb = response_chunk_header_cb;
}

void VHttpResponse::setResponseChunkCompleteCb(
    std::function<void(VHttpResponse *, int)> response_chunk_complete_cb) {
  http_response_chunk_complete_cb = response_chunk_complete_cb;
}
