#pragma once
#include "VBuf.h"
#include "VHttpParser.h"
#include "VOpenSsl.h"
#include "VTcpClient.h"
#include "VZlib.h"
#include <map>
#include <string>

class VHttpResponse {
public:
  // Constructors
  VHttpResponse(VTcpClient *vtcp_client);
  // Destructor
  ~VHttpResponse();

  // Initializes callback functions
  void initRecvCallback();
  void initSendCallback();
  void initCallback();

  void initData();

  void setSslPoint(VOpenSsl *ssl);
  VOpenSsl *getSslPoint() const;
  // Get the associated TCP client object
  VTcpClient *getVTcpClient() const;

  // Get the HTTP parser object used for parsing HTTP requests
  VHttpParser *getVHttpParser() const;

  VZlib *getVZlib() const;

  // Get the response status code
  int getStatusCode() const;

  // Set the response status code
  void setStatusCode(int statusCode);

  // Get the response HTTP version
  std::string getHttpVersion() const;

  // Set the response HTTP version
  void setHttpVersion(const std::string &httpVersion);

  // Get the response headers
  HttpHeaders getHeaders() const;

  // Add a response header
  void addHeader(const std::string &key, const std::string &value);

  // Get the response body
  const VBuf &getBody() const;

  VBuf &recvBody();

  VBuf popBody();

  // Set the response body
  void setBody(const VBuf &body);

  void setBodyCompress(const VBuf &body);

  // Set the response content type
  void setContentType(const std::string &contentType);

  // Get the response content type
  std::string getContentType() const;

  void setAcceptRanges(const std::string &acceptRanges);

  std::string getAcceptRanges() const;

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

  void setServer(const std::string &server);

  void setLocation(std::string location);

  std::string getLocation() const;

  std::string getServer();

  size_t getContentLength() const;
  void setContentLength(const size_t &length);

  void addCookie(const Cookie &cookie) { cookies_.addCookie(cookie); }
  std::vector<Cookie> &getCookies() { return cookies_.getCookies(); }

  void setHttpSsl(bool isSsl);
  bool getHttpSsl() const;

  bool writeData(const VBuf &sendBuf);

  // Check if the request is secure
  bool isSecure() const;

  // Parse response data
  void parseResponse(const VBuf *buf);

  void resetParser();

  // Send the HTTP response
  bool sendResponse(bool isSendBody = true);

  bool sendResponseBegin(VBuf &sendBuf, bool isSend = false);
  bool sendResponseHeader(VBuf &sendBuf, bool isSend = false);
  bool sendResponseBody(VBuf &sendBuf, bool isSend = false);
  bool sendResponseCRLF(VBuf &sendBuf, bool isSend = false);

  bool sendResponseChunked(VBuf &sendBuf, bool isEnd = false);
  bool sendResponseChunkedEnd();

  bool isParser();
  bool isHeaderParser();
  

  std::string getStatusMessage(int statusCode);

  void waitRecvHeadersFinish(const uint64_t &maxTimout = 5000);
  size_t waitRecvBody(const uint64_t &maxTimout = 30000);

  void setData(void *data);
  void *getData();

  void setResponseSendFinishCb(
      std::function<void(VHttpResponse *, int)> response_send_finish_cb);
  void setResponseParserFinishCb(
      std::function<void(VHttpResponse *, int)> response_parser_finish_cb);
  void setResponseParserHeadersFinishCb(
      std::function<void(VHttpResponse *, HttpHeaders *)>
          response_parser_headers_finish_cb);
  void setResponseRecvBodyCb(
      std::function<bool(VHttpResponse *, const VBuf *)> response_recv_body_cb);
  void setResponseChunkHeaderCb(
      std::function<void(VHttpResponse *, int)> response_chunk_header_cb);
  void setResponseChunkCompleteCb(std::function<void(VHttpResponse *, int)>
                                      response_chunk_complete_cb);

protected:
  std::function<void(VHttpResponse *, int)> http_response_send_finish_cb;
  std::function<void(VHttpResponse *, int)> http_response_parser_finish_cb;
  std::function<void(VHttpResponse *, HttpHeaders *)>
      http_response_parser_headers_finish_cb;
  std::function<bool(VHttpResponse *, const VBuf *)> http_response_recv_body_cb;
  std::function<void(VHttpResponse *, int)> http_response_chunk_header_cb;
  std::function<void(VHttpResponse *, int)> http_response_chunk_complete_cb;

private:
  VTcpClient *tcp_client_ = nullptr;   // Associated TCP client object
  VHttpParser *http_parser_ = nullptr; // HTTP parser object
  VZlib *zlib_ = nullptr;              // Zlib object for gzip compression

  VOpenSsl *openssl_ = nullptr;
  void *vdata = nullptr;

  std::string error_message;
  std::string status_message;
  std::string http_version_; // HTTP version
  std::string server_;
  std::string content_language_;
  std::string header_cache_;
  std::string contentType_; // Request content type
  std::string accept_ranges_;
  std::string location_;

  HttpHeaders headers_; // Request headers
  VBuf body_;                                  // Request body
  VBuf response_data_;                         // Request data buffer
  VBuf body_compress_; 
  CookieJar cookies_;
  int status_;
  size_t content_length_;
  size_t max_body_cache_length_;
  bool use_gzip_ = false; // Control whether to use gzip compression
  bool use_chunked_ = false;
  bool keep_alive_ = true; // Control whether to keep the connection alive
  bool http_ssl_ = false;
  bool own_tcp_client_ = false; // Response body
  std::atomic<bool> parser_finish_ = false;
  std::atomic<bool> parser_header_finish_ = false;
  std::atomic<bool> async_recv_ = false;
};
