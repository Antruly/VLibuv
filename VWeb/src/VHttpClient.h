#pragma once
#include "VHttpRequest.h"
#include "VHttpResponse.h"
#include "VOpenSsl.h"
#include "VTcpClient.h"
#include "VWebDefine.h"

struct VHttpPart {
  std::string key;
  VBuf value;
  std::string content_type;     // application/octet-stream
  std::string file_name;
};

class VHttpMultiPart {
public:
  VHttpMultiPart();
  ~VHttpMultiPart();

  void init();

  void append(const VHttpPart &dataPart);
  void appendFinally(const VHttpPart &dataPart);

  const VBuf &getConstData()const;

  const VString getMuitiPartString() const;

  VString generateBoundary();

  // 辅助函数：解析 Content-Disposition 参数
  static void parseContentDisposition(const VString &header_value,
                                      VString &name, VString &filename);

  // 新增方法：从 Content-Type 中解析 boundary
  static VString parseBoundaryFromContentType(const VString &contentType);

  // 解析单个Part的头部和内容
  void processPart(const VString &partData);

 // 主解析方法（新增 contentType 参数）
  bool parseFromBodyData(const VBuf &bodyData,
                                         const VString &contentType);

  // 设置Boundary
  void setBoundary(const VString &boundary);

  std::vector<VHttpPart> &getHttpParts();

private:
  VString multi_boundary = "";
  VBuf data;
  std::vector<VHttpPart> http_parts;
};

class VHttpClient : public VObject {
 public:
  VHttpClient();
  VHttpClient(VTcpClient* tcp_client, bool is_own_tcp_client = false);
  virtual ~VHttpClient();

  void setId(int64_t id);
  int64_t getId();

  VHttpRequest* getVHttpRequest()const;
  VHttpResponse* getVHttpResponse() const;
  VTcpClient* getVTcpClient() const;
  VOpenSsl* getVOpenSsl() const;

  virtual void initCallback(bool isSsl = false);

  void initSsl(const SSL_METHOD* method = SSLv23_client_method());

  std::string getUrlFileName();

    // 等待请求解析完成,返回请求数据长度，通过
  size_t waitRecvRequest(const uint64_t &maxTimeout = 5000);
  size_t waitRecvRequestBody(const uint64_t &maxTimeout = 30000);

  // 发送响应（还需要调用sendBody）
  bool sendResponse(int code = 200, const VBuf &body = VBuf());
  // 发送body需要调用）
  bool sendResponseBody(const VBuf &body, bool isEnd = false);


  // 同步请求
  bool sendRequest(const METHOD_TYPE& method,
                   const std::string& url,
                   const HttpHeaders& headers =
                       HttpHeaders(),
                   const VBuf& body = VBuf(),
                   const uint64_t& maxTimeout = 30000);

  // 发送请求（请求方法和报头,post和put 方法还需要调用sendBody）
  bool sendRequest(const std::string& url, const VBuf& body = VBuf());
  // 发送body（仅post和put方法需要调用）
  bool sendRequestBody(const VBuf &body, bool isEnd = false);
  // 开始读取数据
  void readStart();
  // 等待响应,返回响应数据长度，通过
  size_t waitRecvResponse(const uint64_t& maxTimeout = 5000);
  size_t waitRecvResponseBody(const uint64_t &maxTimeout = 30000);
  // 取出已解析的body, 0 表示当前所有
  VBuf getRecvResponseBody(size_t count = 0);
  // 清除所有缓冲区响应缓冲区数据
  void clearRecvResponseBody();

  // 异步请求
  void sendRequestAsync(const METHOD_TYPE& method,
                        const std::string& url,
                        const HttpHeaders& headers =
                            HttpHeaders(),
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
      std::function<void(VHttpRequest*, HttpHeaders*)>
          request_parser_headers_finish_cb);
  void setRequestRecvBodyCb(
      std::function<bool(VHttpRequest*, const VBuf*)> request_recv_body_cb);

  void setResponseParserHeadersFinishCb(
      std::function<void(VHttpResponse*, HttpHeaders*)>
          response_parser_headers_finish_cb);
  void setResponseRecvBodyCb(
      std::function<bool(VHttpResponse*, const VBuf*)> response_recv_body_cb);

  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);

  void close();

 public:
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

  int64_t client_id = 0;
  VBuf cache_body_;

  bool own_tcp_client_ = false;
};
