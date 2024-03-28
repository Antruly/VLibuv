#pragma once
#include "VHttpResponse.h"
#include <cassert>
#include <VTimer.h>

VHttpResponse::VHttpResponse(VTcpClient* vtcp_client)
    : tcp_client_(vtcp_client),
      own_tcp_client(false),
      http_parser_(new VHttpParser()),
      zlib_(new VZlib()),
      response_data_(),
      max_body_cache_length_(1024 * 1024 * 100),
      body_() {
  this->initCallback();
  http_parser_->httpParserInit(http_parser_type::HTTP_RESPONSE);
}

VHttpResponse::~VHttpResponse() {
  response_data_.clear();
  body_.clear();
  if (own_tcp_client) {
    delete tcp_client_;
  }
  delete http_parser_;
  delete zlib_;
}

void VHttpResponse::initRecvCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setReadCb([this](VTcpClient* tcp_client, const VBuf* data) {
      this->parseResponse(data);
    });
  }
}

void VHttpResponse::initSendCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setWriteCb([this](VTcpClient* tcp_client, const VBuf* data,
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
                                              VHttpParser* http_parser) -> int {
      this->initData();
      this->zlib_->initDecompressStream();
      return 0;  // Example return value, replace with your actual return value
    });

    // URL callback
    http_parser_->setUrlCallback([this](VHttpParser* http_parser,
                                        const char* data, size_t size) -> int {
      return 0;  // Example return value, replace with your actual return value
    });

    // Status callback
    http_parser_->setStatusCallback([this](VHttpParser* http_parser,
                                           const char* data,
                                           size_t size) -> int {
      status_message = std::string(data, size);
      status_ = http_parser->getHttpParser()->status_code;
      return 0;  // Example return value, replace with your actual return value
    });

    // Header field callback
    http_parser_->setHeaderFieldCallback([this](VHttpParser* http_parser,
                                                const char* data,
                                                size_t size) -> int {
      header_cache_ = std::string(data, size);

      return 0;  // Example return value, replace with your actual return value
    });

    // Header value callback
    http_parser_->setHeaderValueCallback([this](VHttpParser* http_parser,
                                                const char* data,
                                                size_t size) -> int {
      std::string header_value = std::string(data, size);
      if (header_cache_.empty()) {
        return -1;
      } else if (header_cache_ == "Content-Type") {
        contentType_ = header_value;
      } else if (header_cache_ == "Connection") {
        keep_alive_ = header_value == "keep-alive" ? true : false;
      } else if (header_cache_ == "Server") {
        server_ = header_value;
      } else if (header_cache_ == "Content-Encoding") {
        if (header_value.find("gzip") != std::string::npos) {
          use_gzip_ = true;
        } else {
          use_gzip_ = false;
        }
      } else if (header_cache_ == "Content-Language") {
        content_language_ = header_value;
      } else if (header_cache_ == "Location") {
        location_ = header_value;
      } else if (header_cache_ == "Content-Length") {
        content_length_ = std::stoul(header_value);
      } else if (header_cache_ == "Transfer-Encoding") {
        if (header_value.find("chunked") != std::string::npos) {
          use_chunked_ = true;
        } else {
          use_chunked_ = false;
        }
      } else if (header_cache_ == "Accept_Ranges") {
        accept_ranges_ = header_value;
      } 
      

      addHeader(header_cache_, header_value);
      header_cache_ = "";
      return 0;  // Example return value, replace with your actual return value
    });

    // Headers complete callback
    http_parser_->setHeadersCompleteCallback([this](VHttpParser* http_parser)
                                                 -> int {
      // Your headers_complete_cb logic here
      return 0;  // Example return value, replace with your actual return value
    });

    // Body callback
    http_parser_->setBodyCallback([this](VHttpParser* http_parser,
                                         const char* data, size_t size) -> int {
      if (body_.size() > max_body_cache_length_) {
        error_message =
            "response body size is to long, max_body_cache_length_ = " +
            std::to_string(max_body_cache_length_) +
            "content_length_ = " + std::to_string(content_length_);
        printf("%s", error_message.c_str());
      } else {
        if (use_gzip_ && size > 0) {
          VBuf gzipData(data, size);
          if (!zlib_->gzipDecompressChunked(gzipData, body_)) {
            error_message = "gzipDecompress is error";
          } 
        } else {
          body_.appandData(data, size); 
        }
      }
     

      return 0;  // Example return value, replace with your actual return value
    });

    // Message complete callback
    http_parser_->setMessageCompleteCallback([this](VHttpParser* http_parser)
                                                 -> int {
      if (http_ssl) {
      } else {
      }
      if (body_.size() > max_body_cache_length_) {
      } else {
          if (use_gzip_ && body_.size() > 0) {
          VBuf gzipData;
            if (!zlib_->gzipDecompressChunked(gzipData, body_, true)) {
            error_message = "gzipDecompress finish is error";
          } 
        
        } 
      }


      http_version_ =
          "HTTP/" + std::to_string(http_parser_->getHttpParser()->http_major) +
          "." + std::to_string(http_parser_->getHttpParser()->http_minor);
      parser_finish = true;
      if (this->async_recv != nullptr)
        this->async_recv->send();

      this->zlib_->closeDecompressStream();
      return 0;  // Example return value, replace with your actual return value
    });

    // Chunk header callback
    http_parser_->setChunkHeaderCallback([this](
                                             VHttpParser* http_parser) -> int {
      // Your chunk_header_cb logic here
      return 0;  // Example return value, replace with your actual return value
    });

    // Chunk complete callback
    http_parser_->setChunkCompleteCallback([this](VHttpParser* http_parser)
                                               -> int {
      // Your chunk_complete_cb logic here
      return 0;  // Example return value, replace with your actual return value
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

  response_data_.clear();
  contentType_ = "";

  use_gzip_ = false;
  keep_alive_ = true;
  parser_finish = false;
  http_ssl = false;
}

void VHttpResponse::setSslPoint(VOpenSsl* ssl) {
  openssl_ = ssl;
}

VOpenSsl* VHttpResponse::getSslPoint() {
  return openssl_;
}

VTcpClient* VHttpResponse::getVTcpClient() const {
  return tcp_client_;
}

VHttpParser* VHttpResponse::getVHttpParser() const {
  return http_parser_;
}

VZlib* VHttpResponse::getVZlib() const {
  return zlib_;
}

int VHttpResponse::getStatusCode() const {
  return status_;
}

void VHttpResponse::setStatusCode(int statusCode) {
  status_ = statusCode;
}

std::string VHttpResponse::getHttpVersion() const {
  return http_version_;
}

void VHttpResponse::setHttpVersion(const std::string& httpVersion) {
  http_version_ = httpVersion;
}

std::map<std::string, std::string> VHttpResponse::getHeaders() const {
  return headers_;
}

void VHttpResponse::addHeader(const std::string& key,
                              const std::string& value) {
  headers_[key] = value;
}

const VBuf& VHttpResponse::getBody()const{
  return body_;
}

VBuf& VHttpResponse::recvBody() {
  return body_;
}

void VHttpResponse::setBody(const VBuf& body) {
  body_ = body;
}

void VHttpResponse::setContentType(const std::string& contentType) {
  contentType_ = contentType;
  this->addHeader("Content-Type", contentType_);
}

std::string VHttpResponse::getContentType() const {
  return contentType_;
}

void VHttpResponse::setAcceptRanges(const std::string& acceptRanges) {
  accept_ranges_ = acceptRanges;
  this->addHeader("Accept-Ranges", accept_ranges_);
}

std::string VHttpResponse::getAcceptRanges() const {
  return accept_ranges_;
}

void VHttpResponse::setKeepAlive(bool keepAlive) {
  keep_alive_ = keepAlive;
  this->addHeader("Connection", keep_alive_ ? "keep_alive" : "close");
}

bool VHttpResponse::getKeepAlive() const {
  return keep_alive_;
}

void VHttpResponse::setUseGzip(bool useGzip) {
  use_gzip_ = useGzip;
  if (use_gzip_) {
    if (headers_["Accept-Encoding"].empty())
      this->addHeader("Accept-Encoding", "gzip");
    else
      this->addHeader("Accept-Encoding", headers_["Accept-Encoding"] + ",gzip");
  } else {
    if (headers_.find("Accept-Encoding") != headers_.end() &&
        headers_["Accept-Encoding"] == "gzip") {
      headers_.erase("Accept-Encoding");
    } else {
      headers_["Accept-Encoding"] =
          static_cast<VString>(headers_["Accept-Encoding"])
              .ReplaceAll(",gzip", "");
      headers_["Accept-Encoding"] =
          static_cast<VString>(headers_["Accept-Encoding"])
              .ReplaceAll("gzip,", "");
      headers_["Accept-Encoding"] =
          static_cast<VString>(headers_["Accept-Encoding"])
              .ReplaceAll("gzip", "");
    }
  }
 
}

bool VHttpResponse::getUseGzip() const {
  return use_gzip_;
}
bool VHttpResponse::getUseChunked() const {
  return use_chunked_;
}
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
void VHttpResponse::setServer(const std::string& server) {
  server_ = server;
  addHeader("Server", server_);
}

void VHttpResponse::setLocation(std::string location) {
  location_ = location;
  this->addHeader("Location", location_);
}

std::string VHttpResponse::getLocation() const {
  return location_;
}

std::string VHttpResponse::getServer() {
  return server_;
}

size_t VHttpResponse::getContentLength() const {
  return content_length_;
}

void VHttpResponse::setContentLength(const size_t& length) {
  content_length_ = length;
  this->addHeader("Content-Length", std::to_string(content_length_));
}

bool VHttpResponse::isSecure() const {
  return false;
}

void VHttpResponse::parseResponse(const VBuf* buf) {
  response_data_.clone(*buf);
  if (http_parser_) {
    http_parser_->httpParserExecute(buf->getConstData(), buf->size());
  }
}

bool VHttpResponse::sendResponse() {
  if (tcp_client_ == nullptr ||
      (tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) !=
          VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) {
    // tcp_client_->connect(url_parser_.host.c_str(), url_parser_.port);
    return false;
  }

  if (http_parser_ == nullptr) {
    return false;
  }

  if (http_version_.empty())
   http_version_ = "HTTP/1.1";

  std::string response = http_version_ + " " + std::to_string(status_) + " " +
                         getStatusMessage(status_) + "\r\n";

  // Add the client identifier
  if (server_.empty()) {
    this->setServer("VHttpServer/1.0");
  }

  // Set other request headers
  for (const auto& header : headers_) {
    response += header.first + ": " + header.second + "\r\n";
  }
  response += "\r\n";

  // Generate the request data packet
  if (use_gzip_) {
    // Gzip compress the body
    VBuf gzip_compressed_data;
    this->zlib_->gzipCompress(&body_, gzip_compressed_data);

    response_data_.resize(response.size() + gzip_compressed_data.size());
    memcpy(response_data_.getData(), response.c_str(), response.size());
    memcpy(response_data_.getData() + response.size(),
           gzip_compressed_data.getConstData(), gzip_compressed_data.size());
  } else {
    // Original body
    response_data_.clear();
    response_data_.appandData(response.c_str(), response.size());
    response_data_.appandData(body_.getConstData(), body_.size());
  }
  this->initSendCallback();
  // Send the HTTP request
  tcp_client_->writeData(response_data_);

  return true;
}

bool VHttpResponse::isParser() {
  return parser_finish;
}

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

void VHttpResponse::waitRecvFinish(const uint64_t& maxTimout) {
  if (this->tcp_client_ == nullptr) {
    return;
  }
 if ((this->tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return;
  }

  assert(this->async_recv != nullptr);

  VLoop vloop;
  this->async_recv = new VAsync();
  VTimer vtimer(&vloop);
  uint64_t curruntTime = 0;
  vtimer.start(
      [this, &vloop, &curruntTime, &maxTimout](VTimer* vtimer) {
        curruntTime++;
        if (body_.size() >0 || curruntTime >
                maxTimout || ((this->tcp_client_->getStatus() &
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
  vloop.run();
  this->async_recv = nullptr;
  delete vasync;

}
void VHttpResponse::setResponseSendFinishCb(
    std::function<void(int)> response_send_finish_cb) {
  http_response_send_finish_cb = response_send_finish_cb;
}

void VHttpResponse::setResponseParserFinishCb(
    std::function<void(int)> response_parser_finish_cb) {
  http_response_parser_finish_cb = response_parser_finish_cb;
}
