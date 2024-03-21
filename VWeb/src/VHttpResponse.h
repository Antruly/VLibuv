#pragma once
#include <map>
#include <string>
#include "VHttpParser.h"
#include "VBuf.h"
#include "VZlib.h"
#include "VTcpClient.h"

class VHttpResponse {
 public:
  // Constructors
  VHttpResponse(VTcpClient* vtcp_client);
  // Destructor
  ~VHttpResponse();

    // Initializes callback functions
  void initRecvCallback();
  void initSendCallback();
  void initCallback();

  void initData();

  // Get the associated TCP client object
  VTcpClient* getVTcpClient() const;

  // Get the HTTP parser object used for parsing HTTP requests
  VHttpParser* getVHttpParser() const;

  // Get the response status code
  int getStatusCode() const;

  // Set the response status code
  void setStatusCode(int statusCode);

  // Get the response HTTP version
  std::string getHttpVersion() const;

  // Set the response HTTP version
  void setHttpVersion(const std::string& httpVersion);

  // Get the response headers
  std::map<std::string, std::string> getHeaders() const;

  // Add a response header
  void addHeader(const std::string& key, const std::string& value);

  // Get the response body
  const VBuf& getBody() const;

  VBuf& recvBody();

  // Set the response body
  void setBody(const VBuf& body);

  // Set the response content type
  void setContentType(const std::string& contentType);

  // Get the response content type
  std::string getContentType() const;

  void setAcceptRanges(const std::string& acceptRanges);

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

  void setServer(const std::string& server);

  std::string getServer();

    size_t getContentLength() const;
  void setContentLength(const size_t& length);

    // Check if the request is secure
  bool isSecure() const;

  // Parse response data
  void parseResponse(const VBuf* buf);

  // Send the HTTP response
  bool sendResponse();

  bool isParser();

  std::string getStatusMessage(int statusCode);

  void waitRecvFinish(const uint64_t& maxTimout = 30000);

   void setResponseSendFinishCb(
      std::function<void(int)> response_send_finish_cb);
  void setResponseParserFinishCb(
      std::function<void(int)> response_parser_finish_cb);

 protected:
  std::function<void(int)> http_response_send_finish_cb;
  std::function<void(int)> http_response_parser_finish_cb;
 private:
  VTcpClient* tcp_client_ = nullptr;    // Associated TCP client object
  VHttpParser* http_parser_ = nullptr;  // HTTP parser object
  VZlib* zlib_ = nullptr;               // Zlib object for gzip compression
  VAsync* async_recv = nullptr;

  std::string error_message;
  std::string status_message;
  std::string http_version_;  // HTTP version
  std::string server_;
  std::string content_language_;
  std::string header_cache_;
  std::string contentType_;  // Request content type
  std::string accept_ranges_;
 



  std::map<std::string, std::string> headers_;  // Request headers
  VBuf body_;                                   // Request body
  VBuf response_data_;                           // Request data buffer

  int status_;
  size_t content_length_;
  size_t max_body_cache_length_;
  bool use_gzip_ = false;   // Control whether to use gzip compression
  bool use_chunked_ = false;
  bool keep_alive_ = true;  // Control whether to keep the connection alive
  bool parser_finish = false;
  bool http_ssl = false;
  bool own_tcp_client = false;  // Response body
};
