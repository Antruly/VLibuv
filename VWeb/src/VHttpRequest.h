#pragma once
#include "VGetaddrinfo.h"
#include "VHttpParser.h"
#include "VOpenSsl.h"
#include "VTcpClient.h"
#include "VThreadPool.h"
#include "VWebDefine.h"
#include "VZlib.h"
#include <regex>

class VHttpRequest : public VObject {
public:
  // Constructors
  VHttpRequest();
  VHttpRequest(VTcpClient *vtcp_client);

  // Destructor
  ~VHttpRequest();

  void initRecvCallback();
  void initSendCallback();

  // Initializes callback functions
  void initCallback();

  void initData();

  void setSslPoint(VOpenSsl *ssl);
  VOpenSsl *getSslPoint() const;

  // Get the associated TCP client object
  VTcpClient *getVTcpClient() const;

  // Get the HTTP parser object used for parsing HTTP requests
  VHttpParser *getVHttpParser() const;

  // Get the response HTTP version
  std::string getHttpVersion() const;

  // Set the response HTTP version
  void setHttpVersion(const std::string &httpVersion);

  // Get the request method
  std::string getMethodName() const;

  // Get the request method
  METHOD_TYPE getMethod() const;

  // Set the request method
  void setMethodName(const std::string &method);

  // Set the request method
  void setMethod(const METHOD_TYPE &method);

  // Get the request URL
  std::string getUrl() const;

  // Set the request URL
  void setUrl(const std::string &url);

  // Get the request URL
  std::string getRawUrl() const;

  // get  the ParsedURL
  ParsedURL getParsedURL();

  // Get the request Host
  std::string getHost() const;

  // Set the request Host
  void setHost(const std::string &host);

  // Get the request headers
  HttpHeaders getHeaders() const;

  // Add a request header
  void addHeader(const std::string &key, const std::string &value);

  // Set the request body
  void setBody(const VBuf &body);
  void setBodyCompress(const VBuf &body);

  // Get the request body
  const VBuf &getBody() const;
  VBuf &recvBody();

  VBuf popBody();

  // Set the request content type
  void setContentType(const std::string &contentType);

  // Get the request content type
  std::string getContentType() const;

  void setAccept(const std::string &accept);

  std::string getAccept() const;

  // Set the request Referer
  void setReferer(const std::string &referer);

  // Get the request Referer
  std::string getReferer() const;

  // Set whether to keep the connection alive (HTTP 1.1 defaults to true)
  void setKeepAlive(bool keepAlive);

  // Get whether to keep the connection alive
  bool getKeepAlive() const;

  // Control whether to use gzip compression
  void setUseGzip(bool useGzip);

  // Get whether to use gzip compression
  bool getUseGzip() const;

  void setUseChunked(bool useChunked);

  bool getUseChunked() const;

  void setUserAgent(std::string user_agent);

  std::string getUserAgent() const;

  void setLocation(std::string location);

  std::string getLocation() const;

  void setHttpSsl(bool isSsl);
  bool getHttpSsl() const;

  // Parse request data
  void parseRequest(const VBuf *buf);

  void resetParser();

  // Check if the request is secure
  bool isSecure() const;

  // Close the TCP connection
  void closeConnection();

  size_t getContentLength() const;
  void setContentLength(const size_t &length);

  void setMaxBodyCacheLength(const size_t &length);

  void addCookie(const Cookie &cookie) { cookies_.addCookie(cookie);
  }
  std::vector<Cookie> &getCookies() { return cookies_.getCookies();
  }
  

  // Send the HTTP request
  bool sendRequest(bool isSendBody = true);

  bool sendRequestBegin(VBuf &sendBuf, bool isSend = false);
  bool sendRequestHeader(VBuf &sendBuf, bool isSend = false);
  bool sendRequestBody(VBuf &sendBuf, bool isSend = false);
  bool sendRequestCRLF(VBuf &sendBuf, bool isSend = false);

  bool sendRequestChunked(VBuf &sendBuf, bool isEnd = false);
  bool sendRequestChunkedEnd();

  bool isParser();
  bool isHeaderParser();

  bool connect(const std::string &addrs, int port);

  void waitRecvHeadersFinish(const uint64_t &maxTimout = 5000);
  size_t waitRecvBody(const uint64_t &maxTimout = 30000);

  void setData(void *data);
  void *getData();

  void setRequestSendFinishCb(
      std::function<void(VHttpRequest *, int)> request_send_finish_cb);
  void setRequestParserFinishCb(
      std::function<void(VHttpRequest *, int)> request_parser_finish_cb);

  void setRequestParserHeadersFinishCb(
      std::function<void(VHttpRequest *, HttpHeaders *)>
          request_parser_headers_finish_cb);
  void setRequestRecvBodyCb(
      std::function<bool(VHttpRequest *, const VBuf *)> request_recv_body_cb);
  void setRequestChunkHeaderCb(
      std::function<void(VHttpRequest *, int)> request_chunk_header_cb);
  void setRequestChunkCompleteCb(
      std::function<void(VHttpRequest *, int)> request_chunk_complete_cb);

protected:
  // Extract the host from the URL
  std::string getHostFromUrl(const std::string &url) const;

  bool isValidIpAddress(const std::string &ipAddress);

  std::string getIPFromVGetaddrinfo(const std::string &host);

  bool writeData(const VBuf &sendBuf);

protected:
  std::function<void(VHttpRequest *, int)> http_request_send_finish_cb;
  std::function<void(VHttpRequest *, int)> http_request_parser_finish_cb;
  std::function<void(VHttpRequest *, HttpHeaders *)>
      http_request_parser_headers_finish_cb;
  std::function<bool(VHttpRequest *, const VBuf *)> http_request_recv_body_cb;
  std::function<void(VHttpRequest *, int)> http_request_chunk_header_cb;
  std::function<void(VHttpRequest *, int)> http_request_chunk_complete_cb;

private:
  VTcpClient *tcp_client_ = nullptr;   // Associated TCP client object
  VHttpParser *http_parser_ = nullptr; // HTTP parser object
  VZlib *zlib_ = nullptr;              // Zlib object for gzip compression
  VGetaddrinfo *getaddrinfo_ = nullptr;

  VOpenSsl *openssl_ = nullptr;
  void *vdata = nullptr;

  std::string error_message;
  std::string http_version_; // HTTP version
  std::string host_;
  std::string url_;     // Request URL
  std::string url_raw_; // Request URL
  std::string referer_; // Request Referer
  std::string user_agent_;
  std::string accept_language_;
  std::string header_cache_;
  std::string contentType_; // Request content type
  std::string accept_;
  std::string location_;

  ParsedURL url_parser_; // Parsed URL components

  HttpHeaders headers_;                        // Request headers
  VBuf body_;                                  // Request body
  VBuf body_compress_;                         // Request body_compress_
  VBuf request_data_;                          // Request data buffer
  CookieJar cookies_;
  METHOD_TYPE method_; // Request method
  size_t content_length_;
  size_t max_body_cache_length_;
  bool use_gzip_ = false; // Control whether to use gzip compression
  bool use_chunked_ = false;
  bool keep_alive_ = true; // Control whether to keep the connection alive
  bool http_ssl_ = false;
  bool own_tcp_client_ = false;
  std::atomic<bool> parser_finish_ = false;
  std::atomic<bool> parser_header_finish_ = false;
  std::atomic<bool> async_recv_ = false;
};
