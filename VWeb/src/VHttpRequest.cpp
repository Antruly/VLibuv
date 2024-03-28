#include "VHttpRequest.h"

VHttpRequest::VHttpRequest()
    : tcp_client_(new VTcpClient()),
      own_tcp_client(true),
      http_parser_(new VHttpParser()),
      getaddrinfo_(new VGetaddrinfo()),
      zlib_(new VZlib()),
      method_(METHOD_TYPE::OPTIONS),
      request_data_(),
      max_body_cache_length_(VWEB_HTTP_MAX_BODY_CACHE_LENGTH),
      body_() {
  this->initCallback();
  http_parser_->httpParserInit(http_parser_type::HTTP_REQUEST);
}

VHttpRequest::VHttpRequest(VTcpClient* vtcp_client)
    : tcp_client_(vtcp_client),
      own_tcp_client(false),
      http_parser_(new VHttpParser()),
      getaddrinfo_(new VGetaddrinfo()),
      zlib_(new VZlib()),
      method_(METHOD_TYPE::OPTIONS),
      request_data_(),
      max_body_cache_length_(VWEB_HTTP_MAX_BODY_CACHE_LENGTH),
      body_() {
  this->initCallback();
  http_parser_->httpParserInit(http_parser_type::HTTP_REQUEST);
}

VHttpRequest::~VHttpRequest() {
  request_data_.clear();
  body_.clear();
  if (own_tcp_client)
  {
    delete tcp_client_;
  }
  delete getaddrinfo_;
  delete http_parser_;
  delete zlib_;
}
void VHttpRequest::initRecvCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setReadCb([this](VTcpClient* tcp_client, const VBuf* data) {
      this->parseRequest(data);
    });
  }
}

void VHttpRequest::initSendCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setWriteCb([this](VTcpClient* tcp_client, const VBuf* data,
                                   int status) {
      if (status < 0) {
        error_message = "Request Write error status:" + std::to_string(status);
      }
    });
   

  }
}
void VHttpRequest::initCallback() {
    
 if (http_parser_ != nullptr) {
    // Message begin callback
    http_parser_->setMessageBeginCallback([this](
                                              VHttpParser* http_parser) -> int {
      this->initData();
      return 0;  // Example return value, replace with your actual return value
    });

    // URL callback
    http_parser_->setUrlCallback([this](VHttpParser* http_parser,
                                        const char* data, size_t size) -> int {
      url_raw_ = std::string(data, size);
      return 0;  // Example return value, replace with your actual return value
    });

    // Status callback
    http_parser_->setStatusCallback([this](VHttpParser* http_parser,
                                           const char* data,
                                           size_t size) -> int {
      
      //status_ = std::stoi(std::string(data, size));
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
      } else if (header_cache_ == "Host") {
        host_ = header_value;
      } else if (header_cache_ == "Accept") {
        accept_ = header_value;
      } else if (header_cache_ == "Content-Type") {
        contentType_ = header_value;
      } else if (header_cache_ == "Connection") {
        keep_alive_ = header_value == "keep-alive" ? true : false;
      } else if (header_cache_ == "Upgrade-Insecure-Requests") {
      } else if (header_cache_ == "User-Agent") {
        user_agent_ = header_value;
      } else if (header_cache_ == "Accept-Encoding") {
        if (header_value.find("gzip") != std::string::npos) {
          use_gzip_ = true;
        } else {
          use_gzip_ = false;
        }
      } else if (header_cache_ == "Accept-Language") {
        accept_language_ = header_value;
      } else if (header_cache_ == "Location") {
        location_ = header_value;
      } else if (header_cache_ == "Referer") {
        referer_ = header_value;
      } else if (header_cache_ == "Content-Length") {
        content_length_ = std::stoul(header_value);
      } else if (header_cache_ == "Transfer-Encoding") {
        if (header_value.find("chunked") != std::string::npos) {
          use_chunked_ = true;
        } else {
          use_chunked_ = false;
        }
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
            "request body size is to long, max_body_cache_length_ = " +
            std::to_string(max_body_cache_length_) +
                        "content_length_ = " + std::to_string(content_length_);
        printf("%s", error_message.c_str());
      } else {
        body_.appandData(data, size);
      }

      return 0;  // Example return value, replace with your actual return value
    });

    // Message complete callback
    http_parser_->setMessageCompleteCallback([this](VHttpParser* http_parser)
                                                 -> int {
      if (http_ssl) {
        url_ = "https://" + host_ + url_raw_;
      } else {
        url_ = "http://" + host_ + url_raw_;
      }
      http_version_ =
          "HTTP/" + std::to_string(http_parser_->getHttpParser()->http_major) +
          "." + std::to_string(http_parser_->getHttpParser()->http_minor);
      url_parser_ = http_parser_->parseUrl(url_);
      method_ = static_cast<METHOD_TYPE>(http_parser_->getHttpParser()->method);
      parser_finish = true;
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

void VHttpRequest::initData() {
  http_version_ = "";
  method_ = METHOD_TYPE::OPTIONS;
  url_ = "";
  url_raw_ = "";
  host_ = "";
  referer_ = "";
  user_agent_ = "";
  header_cache_ = "";
  location_ = "";

  url_parser_.protocol = "";
  url_parser_.host = "";
  url_parser_.port = 0;
  url_parser_.path = "";
  url_parser_.query = "";

  content_length_ = 0;

  headers_.clear();
  body_.clear();

  request_data_.clear();
  contentType_ = "";
  accept_ = "";

  use_gzip_ = false;
  use_chunked_ = false;
  keep_alive_ = true;
  parser_finish = false;
  http_ssl = false;
}

void VHttpRequest::setSslPoint(VOpenSsl* ssl) {
    openssl_ = ssl;
}

VOpenSsl* VHttpRequest::getSslPoint() {
  return openssl_;
}

VTcpClient* VHttpRequest::getVTcpClient() const {
  return tcp_client_;
}

VHttpParser* VHttpRequest::getVHttpParser() const {
  return http_parser_;
}

std::string VHttpRequest::getHttpVersion() const {
  return http_version_;
}

void VHttpRequest::setHttpVersion(const std::string& httpVersion) {
  http_version_ = httpVersion;
}

std::string VHttpRequest::getMethodName() const {
  return http_parser_->getMethodName(method_);
}

METHOD_TYPE VHttpRequest::getMethod() const {
  return method_;
}

void VHttpRequest::setMethodName(const std::string& method) {
  method_ = http_parser_->getMethodType(method);
}

void VHttpRequest::setMethod(const METHOD_TYPE& method) {
  method_ = method;
}

std::string VHttpRequest::getUrl() const {
  return url_;
}

void VHttpRequest::setUrl(const std::string& url) {
  url_ = url;
  url_parser_ = http_parser_->parseUrl(url_);
  url_raw_ = url_parser_.path + (url_parser_.query.empty() ? "" : "?") + url_parser_.query;
  this->setHost(url_parser_.host);
  if (url_parser_.protocol == "https") {
    http_ssl = true;
  }
}

std::string VHttpRequest::getRawUrl() const {
  return url_raw_;
}

ParsedURL VHttpRequest::getParsedURL() {
  return url_parser_;
}

std::string VHttpRequest::getHost() const {
  return host_;
}

void VHttpRequest::setHost(const std::string& host) {
  host_ = host;
}

std::map<std::string, std::string> VHttpRequest::getHeaders() const {
  return headers_;
}

void VHttpRequest::addHeader(const std::string& key, const std::string& value) {
  headers_[key] = value;
}

void VHttpRequest::setBody(const VBuf& body) {
  body_ = body;
}

const VBuf& VHttpRequest::getBody() const {
  return body_;
}

void VHttpRequest::setContentType(const std::string& contentType) {
  addHeader("Content-Type", contentType);
  contentType_ = contentType;
}

std::string VHttpRequest::getContentType() const {
  return contentType_;
}

void VHttpRequest::setAccept(const std::string& accept) {
  accept_ = accept;
}

std::string VHttpRequest::getAccept() const {
  return accept_;
}

void VHttpRequest::setReferer(const std::string& referer) {
  addHeader("Referer", referer);
  referer_ = referer;
}

std::string VHttpRequest::getReferer() const {
  return std::string();
}

void VHttpRequest::setKeepAlive(bool keepAlive) {
  if (keepAlive) {
    addHeader("Connection", "keep-alive");
  } else {
    addHeader("Connection", "close");
  }
  keep_alive_ = keepAlive;
}

bool VHttpRequest::getKeepAlive() const {
  return keep_alive_;
}

void VHttpRequest::parseRequest(const VBuf* buf) {
  // Parse request data, you can use methods from VHttpParser to parse HTTP
  // requests
  
  request_data_.clone(*buf);
  if (http_parser_) {
    http_parser_->httpParserExecute(buf->getConstData(), buf->size());
    
  }
}

bool VHttpRequest::isSecure() const {
  // Check if the request is secure, you can implement security checks based on
  // your requirements Here, simply return false
  return false;
}

void VHttpRequest::closeConnection() {
  // Logic to close the TCP connection, you can call methods from VTcpClient to
  // close the connection
  if (tcp_client_) {
    tcp_client_->close();
  }
}

size_t VHttpRequest::getContentLength() const {
  return content_length_;
}

void VHttpRequest::setContentLength(const size_t& length) {
  content_length_ = length;
  this->addHeader("Content-Length", std::to_string(content_length_));
}

bool VHttpRequest::sendRequest(bool isSendBody) {
  if (tcp_client_ == nullptr || (tcp_client_->getStatus() &
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) !=
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED)
    {
    return false;
  }
  
  if (http_parser_ == nullptr) {
    return false;
  }

  this->initSendCallback();
  
  if (isSendBody) {
    if (!this->sendRequestBegin(request_data_, false)) {
      return false;
    }
    if (!this->sendRequestHeader(request_data_, false)) {
      return false;
    }
    if (!this->sendRequestBody(request_data_, true)) {
      return false;
    }
  } else {
    if (!this->sendRequestBegin(request_data_, false)) {
      return false;
    }
    if (!this->sendRequestHeader(request_data_, true)) {
      return false;
    }
  }
  return true;
}

bool VHttpRequest::sendRequestBegin(VBuf& sendBuf, bool isSend) {
  // Construct the HTTP request string
  std::string request = http_parser_->getMethodName(method_) + " " +
                        url_parser_.path +
                        (url_parser_.query.empty() ? "" : "?") +
                        url_parser_.query + " HTTP/1.1\r\n";

   sendBuf.appandData(request.c_str(), request.size());
  if (isSend) {
    tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpRequest::sendRequestHeader(VBuf& sendBuf, bool isSend) {
  std::string request;
     // Add the client identifier
  if (user_agent_.empty())
    this->setUserAgent("VHttpClient/1.0");

  if (host_.empty() && headers_.find("Host") != headers_.end()) {
    host_ = headers_["Host"];
  } else if (host_.empty() && !url_parser_.host.empty()) {
    host_ = url_parser_.host + ":" + std::to_string(url_parser_.port);
    headers_["Host"] = host_;
  } else if (host_.empty()) {
    std::string ip;
    int port;
    tcp_client_->getPeerAddrs(ip, port);
    host_ = ip + ":" + std::to_string(port);
    headers_["Host"] = host_;
  }
  request += "Host: " + host_ + "\r\n";

  // Add content length information
  if (method_ == METHOD_TYPE::POST || method_ == METHOD_TYPE::PUT) {
    if (use_chunked_) {
      if (headers_.find("Content-Length") != headers_.end()) {
        headers_.erase("Content-Length");
        content_length_ = -1;
      }
      if (headers_.find("Transfer-Encoding") == headers_.end()) {
        this->addHeader("Transfer-Encoding", "chunked");
      }
    } else {
      if (headers_.find("Content-Length") != headers_.end()) {
        content_length_ = std::stoul(headers_["Content-Length"]);
      } else if ((body_.size() > content_length_)) {
        content_length_ = body_.size();
      }
      request += "Content-Length: " + std::to_string(content_length_) + "\r\n";
    }
  }

  // Set other request headers
  for (const auto& header : headers_) {
    if (header.first == "Host")
      continue;
    else if (header.first == "Content-Length")
      continue;
    request += header.first + ": " + header.second + "\r\n";
  }

  request += "\r\n";

  sendBuf.appandData(request.c_str(), request.size());
  if (isSend) {
    tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpRequest::sendRequestBody(VBuf& sendBuf, bool isSend) {
  if (use_chunked_) {
    return false;
  }
    // Generate the request data packet
  if (false && use_gzip_) {
    // Gzip compress the body
    VBuf gzip_compressed_data;
    this->zlib_->gzipCompress(&body_, gzip_compressed_data);
    sendBuf.appandData(gzip_compressed_data.getConstData(),
                       gzip_compressed_data.size());
  } else {
    // Original body
    sendBuf.appandData(body_.getConstData(), body_.size());
  }

   if (isSend) {
    tcp_client_->writeData(sendBuf);
  }
  return true;
}

bool VHttpRequest::sendRequestCRLF(VBuf& sendBuf, bool isSend) {

  std::string request = "\r\n";
  sendBuf.appandData(request.c_str(), request.size());
  tcp_client_->writeData(sendBuf);
  return true;
}



bool VHttpRequest::sendRequestChunked(VBuf& sendBuf, bool isEnd) {

  std::string request = std::to_string(sendBuf.size()) + "\r\n";
  sendBuf.appandData(request.c_str(), request.size());
  sendBuf.appandData("\r\n", 2);
  tcp_client_->writeNewData(sendBuf);
  if (isEnd) {
    const static VBuf endBuf("0\r\n\r\n",5);
    tcp_client_->writeNewData(endBuf);
  }
  return true;
}

bool VHttpRequest::isParser() {
  return parser_finish;
}

void VHttpRequest::setUseGzip(bool useGzip) {
  // If gzip is enabled, automatically add the Accept-Encoding header field
  if (useGzip) {
    addHeader("Accept-Encoding", "gzip");
  } else {
    // If gzip is disabled and the Accept-Encoding header field already exists,
    // remove the "gzip" option
    auto it = headers_.find("Accept-Encoding");
    if (it != headers_.end()) {
      std::string& acceptEncoding = it->second;
      std::string::size_type pos = acceptEncoding.find("gzip");
      if (pos != std::string::npos) {
        // Remove the "gzip" option
        acceptEncoding.erase(pos, 4);

        // If the field is empty after removal, remove the entire
        // Accept-Encoding header field
        if (acceptEncoding.empty()) {
          headers_.erase(it);
        }
      }
    }
  }
  use_gzip_ = useGzip;
}

bool VHttpRequest::getUseGzip() const {
  return use_gzip_;
}

std::string VHttpRequest::getHostFromUrl(const std::string& url) const {
  // Parse the URL to extract the host information
  if (http_parser_ != nullptr) {
    return http_parser_->parseUrl(url).host;
  }
  return "";
}
void VHttpRequest::setUserAgent(std::string user_agent) {
  user_agent_ = user_agent;
  this->addHeader("User-Agent", user_agent);
}
void VHttpRequest::setUseChunked(bool useChunked) {
  use_chunked_ = useChunked;
  if (use_chunked_) {
    this->addHeader("Transfer-Encoding", "chunked");
  }
  else {
    if (headers_.find("Transfer-Encoding") != headers_.end()) {
      headers_.erase("Transfer-Encoding");
    }
  }
}
bool VHttpRequest::isValidIpAddress(const std::string& ipAddress) {
  std::regex ipRegex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
  return std::regex_match(ipAddress, ipRegex);
}
bool VHttpRequest::getUseChunked() const {
  return use_chunked_;
}
std::string VHttpRequest::getUserAgent() const {
  return user_agent_;
}
void VHttpRequest::setLocation(std::string location) {
  location_ = location;
  this->addHeader("Location", location_);
}
void VHttpRequest::setMaxBodyCacheLength(const size_t& length) {
  max_body_cache_length_ = length;
}
std::string
    VHttpRequest::getIPFromVGetaddrinfo(const std::string& host) {
  if (this->isValidIpAddress(host))
    {
    return host;
    }
  if (getaddrinfo_ != nullptr) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    VLoop loop;
    std::string strIp;
    getaddrinfo_->getaddrinfo(
        &loop,
        [this, &strIp](VGetaddrinfo* getaddrinfo, int status,
                       struct addrinfo* ip) {
          if (status < 0) {
            error_message = "getaddrinfo callback error: " +
                            std::string(uv_strerror(status));
            return;
          }

          char addr[INET_ADDRSTRLEN];
          struct sockaddr_in* addr_in = (struct sockaddr_in*)ip->ai_addr;
          inet_ntop(AF_INET, &(addr_in->sin_addr), addr, INET_ADDRSTRLEN);
          strIp = std::string(addr);
        },
        host.c_str(), nullptr, &hints);

    loop.run();

    return strIp;
  }
}

std::string VHttpRequest::getLocation() const {
  return location_;
}

void VHttpRequest::setRequestSendFinishCb(
    std::function<void(int)> request_send_finish_cb) {
  http_request_send_finish_cb = request_send_finish_cb;
}

void VHttpRequest::setRequestParserFinishCb(
    std::function<void(int)> request_parser_finish_cb) {
  http_request_parser_finish_cb = request_parser_finish_cb;
}


 bool VHttpRequest::connect(const std::string& addrs, int port) {
  if (addrs.empty() || port <= 0 || port > VNETWORK_MAX_PORT_NUMBER) {
    error_message = "addrs or port is null";
    return false;
  }
  std::string ip;
  if (!this->isValidIpAddress(addrs)) {
    ip = this->getIPFromVGetaddrinfo(addrs);
    if (ip.empty()) {
      error_message = "getIPFromVGetaddrinfo is null";
      return false;
    } else if (!this->isValidIpAddress(ip)) {
      error_message = "getIPFromVGetaddrinfo is not ip,value:" + ip;
      return false;
    }
  } else {
    ip = addrs;
  }

  if (tcp_client_ == nullptr) {
    return false;
  }
  if (tcp_client_->connect(ip.c_str(), port) != 0) {
    return false;
  }

  if (openssl_ != nullptr && url_parser_.protocol == "https") {
      http_ssl = true;
  }

  error_message =
      "connect is error addrs:" + addrs + ", port:" + std::to_string(port);
  return false;
 }