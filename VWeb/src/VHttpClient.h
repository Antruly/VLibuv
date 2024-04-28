#pragma once
#include "VHttpRequest.h"
#include "VHttpResponse.h"
#include "VOpenSsl.h"
#include "VTcpClient.h"
#include "VWebDefine.h"

struct VHttpPart {
  std::string key;
  std::string value;
  std::string content_type;
};

class VHttpMultiPart {
public:
  VHttpMultiPart();
  ~VHttpMultiPart();

  void init();

  void append(const VHttpPart &dataPart);
  void appendFinally(const VHttpPart &dataPart);

  const VBuf &getConstData()const;

  const std::string getMuitiPartString() const;

  std::string generateBoundary();

private:
  std::string multi_boundary = "";
  VBuf data;
};

class VHttpClient : public VObject {
 public:
  VHttpClient();
  VHttpClient(VTcpClient* tcp_client);
  virtual ~VHttpClient();
  VHttpRequest* getVHttpRequest()const;
  VHttpResponse* getVHttpResponse() const;
  VTcpClient* getVTcpClient() const;
  VOpenSsl* getVOpenSsl() const;

  virtual void initCallback(bool isSsl = false);

  void initSsl(const SSL_METHOD* method = SSLv23_client_method());

  std::string getUrlFileName();

  // 同步请求
  bool sendRequest(const METHOD_TYPE& method,
                   const std::string& url,
                   const std::map<std::string, std::string>& headers =
                       std::map<std::string, std::string>(),
                   const VBuf& body = VBuf(),
                   const uint64_t& maxTimeout = 30000);

  // 发送请求（请求方法和报头,post和put 方法还需要调用sendBody）
  bool sendRequest(const std::string& url, const VBuf& body = VBuf());
  // 发送body（仅post和put方法需要调用）
  bool sendRequestBody(const VBuf& body);
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

  void setConnectiondCb(std::function<void(VHttpClient*, int)> connectiond_cb);
  void setCloseCb(std::function<void(VHttpClient*, int)> close_cb);
  void setRequestSendFinishCb(
      std::function<void(VHttpRequest*, int)> request_send_finish_cb);
  void setRequestParserFinishCb(
      std::function<void(VHttpRequest*, int)> request_parser_finish_cb);
  void setResponseSendFinishCb(
      std::function<void(VHttpResponse*, int)> response_send_finish_cb);
  void setResponseParserFinishCb(
      std::function<void(VHttpResponse*, int)> response_parser_finish_cb);

  void setRequestParserHeadersFinishCb(
      std::function<void(VHttpRequest*, std::map<std::string, std::string>*)>
          request_parser_headers_finish_cb);
  void setRequestRecvBodyCb(
      std::function<bool(VHttpRequest*, const VBuf*)> request_recv_body_cb);

  void setResponseParserHeadersFinishCb(
      std::function<void(VHttpResponse*, std::map<std::string, std::string>*)>
          response_parser_headers_finish_cb);
  void setResponseRecvBodyCb(
      std::function<bool(VHttpResponse*, const VBuf*)> response_recv_body_cb);

  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);

 protected:
  virtual bool connect(const std::string& url);

 protected:
  std::function<void(VHttpClient*, int)> http_client_connectiond_cb;
  std::function<void(VHttpClient*, int)> http_client_close_cb;
 protected:
  std::string error_message;
 private:
  VTcpClient* tcp_client_ = nullptr;
  VHttpRequest* request_ = nullptr;
  VHttpResponse* response_ = nullptr;
  VOpenSsl* openssl_ = nullptr;

  VBuf cache_body_;

  bool own_tcp_client_ = false;
};
