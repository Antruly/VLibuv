#pragma once
#include "VHttpRequest.h"
#include "VHttpResponse.h"
#include "VTcpClient.h"

class VHttpClient : public VObject{
 public:
  VHttpClient();
  VHttpClient(VTcpClient* tcp_client);
  ~VHttpClient();
  VHttpRequest* getVHttpRequest();
  VHttpResponse* getVHttpResponse();
  VTcpClient* getVTcpClient();

  void initCallback();

  std::string getUrlFileName();

  // 同步请求
 bool sendRequest(const METHOD_TYPE& method, const std::string& url,
                      const std::map<std::string, std::string>& headers =
                          std::map<std::string, std::string>(),
                      const VBuf& body = VBuf(),
                      const uint64_t& maxTimeout = 30000);

  // 发送请求（请求方法和报头,post和put 方法还需要调用sendBody）
  bool sendRequest(const std::string& url);
  // 发送body（仅post和put方法需要调用）
  bool sendBody(const VBuf& body);
  // 开始读取数据
  void readStart();
  // 等待响应,返回响应数据长度，通过
  size_t waitRecvResponse(const uint64_t& maxTimeout = 30000);
  // 取出已解析的body, 0 表示当前所有
  VBuf getRecvResponseBody(size_t count = 0);
  // 清除所有缓冲区响应缓冲区数据
  void clearRecvResponseBody();

  // 异步请求
 void sendRequestAsync(const METHOD_TYPE& method,
                       const std::string& url,
                       const std::map<std::string, std::string>& headers =
                           std::map<std::string, std::string>(),
                       const VBuf& body = VBuf(),
                       const uint64_t& maxTimeout = 30000);
 
  void setConnectiondCb(std::function<void(int)> connectiond_cb);
 void setRequestSendFinishCb(
     std::function<void(int)> request_send_finish_cb);
  void setRequestParserFinishCb(
      std::function<void(int)> request_parser_finish_cb);
 void setResponseSendFinishCb(
     std::function<void(int)> response_send_finish_cb);
  void setResponseParserFinishCb(
      std::function<void(int)> response_parser_finish_cb);

  protected:
  bool connect(const std::string& url);

  protected:
  std::function<void(int)> http_client_connectiond_cb;

  std::function<void(int)> http_client_request_send_finish_cb;

  std::function<void(int)> http_client_request_parser_finish_cb;

  std::function<void(int)> http_client_response_send_finish_cb;

  std::function<void(int)> http_client_response_parser_finish_cb;


 private:
  VTcpClient* tcp_client_;
  VHttpRequest* request_;
  VHttpResponse* response_;

  std::string error_message;

  VBuf cache_body_;

  bool own_http_client_ = false;
};
