#pragma once
extern "C" {
#include "http_parser.h"
}
#include <functional>
#include "VCoreDefine.h"
#include "VWebDefine.h"
#include "VObject.h"
#include <string>



// URL解析结果结构体
struct ParsedURL {
  std::string protocol;
  std::string host;
  uint16_t port;
  std::string path;
  std::string query;
};


class VHttpParser : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VHttpParser);
  VCORE_DEFINE_COPY_FUNC(VHttpParser);

  /* Initialize http_parser members. */
  void httpParserInit(enum http_parser_type type);

  /* Executes the parser. Returns number of parsed bytes. Sets
   * `parser->http_errno` on error. */
  size_t httpParserExecute(const char* data, size_t len);

  /* If httpShouldKeepAlive() in the on_headers_complete or
   * on_message_complete callback returns 0, then this should be
   * the last message on the connection.
   * If you are the server, respond with the "Connection: close" header.
   * If you are the client, close the connection.
   */
  int httpShouldKeepAlive();

  /* Pause or un-pause the parser; a nonzero value pauses. */
  void httpParserPause(int paused);

  /* Checks if this is the final chunk of the body. */
  int httpBodyIsFinal();

     // 设置 message_begin_cb 回调函数
  void setMessageBeginCallback(std::function<int(VHttpParser*)> callback);

  // 设置 url_cb 回调函数
  void setUrlCallback(
      std::function<int(VHttpParser*, const char*, size_t)> callback);

  // 设置 status_cb 回调函数
  void setStatusCallback(
      std::function<int(VHttpParser*, const char*, size_t)> callback);

  // 设置 header_field_cb 回调函数
  void setHeaderFieldCallback(
      std::function<int(VHttpParser*, const char*, size_t)> callback);

  // 设置 header_value_cb 回调函数
  void setHeaderValueCallback(
      std::function<int(VHttpParser*, const char*, size_t)> callback);

  // 设置 headers_complete_cb 回调函数
  void setHeadersCompleteCallback(std::function<int(VHttpParser*)> callback);

  // 设置 body_cb 回调函数
  void setBodyCallback(
      std::function<int(VHttpParser*, const char*, size_t)> callback);

  // 设置 message_complete_cb 回调函数
  void setMessageCompleteCallback(std::function<int(VHttpParser*)> callback);

  // 设置 chunk_header_cb 回调函数
  void setChunkHeaderCallback(std::function<int(VHttpParser*)> callback);

  // 设置 chunk_complete_cb 回调函数
  void setChunkCompleteCallback(std::function<int(VHttpParser*)> callback);

  // 解析URL并返回解析结果
  ParsedURL parseUrl(const std::string& url);

  std::string getMethodName(const METHOD_TYPE& method_type);
  METHOD_TYPE getMethodType(const std::string& method_name);

  http_parser* getHttpParser() const;

  int setData(void* pdata);
  void* getData();

 public:
  /* Returns a string version of the HTTP method. */
  static const char* httpMethodStr(enum http_method m);

  /* Returns a string version of the HTTP status code. */
  static const char* httpStatusStr(enum http_status s);

  /* Return a string name of the given error. */
  static const char* httpErrnoName(enum http_errno err);

  /* Return a string description of the given error. */
  static const char* httpErrnoDescription(enum http_errno err);

  /* Initialize all http_parser_url members to 0. */
  static void httpParserUrlInit(struct http_parser_url* u);

  /* Parse a URL; return nonzero on failure. */
  static int httpParserParseUrl(const char* buf,
                                size_t buflen,
                                int isConnect,
                                struct http_parser_url* u);

  /* Change the maximum header size provided at compile time. */
  static void httpParserSetMaxHeaderSize(uint32_t size);

  /* Returns the library version. Bits 16-23 contain the major version number,
   * bits 8-15 the minor version number and bits 0-7 the patch level.
   * Usage example:
   *
   *   unsigned long version = httpParserVersion();
   *   unsigned major = (version >> 16) & 255;
   *   unsigned minor = (version >> 8) & 255;
   *   unsigned patch = version & 255;
   *   Log->logDebug("http_parser v%u.%u.%u\n", major, minor, patch);
   */
  static unsigned long httpParserVersion(void);

 protected:
  void setHttpParser(void* hd);
  void setHttpParserData();

  static int callback_message_begin(http_parser* m_parser);
  static int callback_url(http_parser* m_parser,
                           const char* pdata,
                           size_t size);
  static int callback_status(http_parser* m_parser,
                              const char* pdata,
                              size_t size);
  static int callback_header_field(http_parser* m_parser,
                                    const char* pdata,
                                    size_t size);
  static int callback_header_value(http_parser* m_parser,
                                    const char* pdata,
                                    size_t size);
  static int callback_headers_complete(http_parser* m_parser);
  static int callback_body(http_parser* m_parser,
                            const char* pdata,
                            size_t size);
  static int callback_message_complete(http_parser* m_parser);
  static int callback_chunk_header(http_parser* m_parser);
  static int callback_chunk_complete(http_parser* m_parser);

 


 private:
  void freeHttpParser();
  /* Initialize http_parser_settings members to 0. */
  void httpParserSettingsInit();
  http_parser_settings* getHttpCallBack();

 protected:
  std::function<int(VHttpParser*)> message_begin_cb;
  std::function<int(VHttpParser*, const char*, size_t)> url_cb;
  std::function<int(VHttpParser*, const char*, size_t)> status_cb;
  std::function<int(VHttpParser*, const char*, size_t)> header_field_cb;
  std::function<int(VHttpParser*, const char*, size_t)> header_value_cb;
  std::function<int(VHttpParser*)> headers_complete_cb;
  std::function<int(VHttpParser*, const char*, size_t)> body_cb;
  std::function<int(VHttpParser*)> message_complete_cb;
  std::function<int(VHttpParser*)> chunk_header_cb;
  std::function<int(VHttpParser*)> chunk_complete_cb;

 private:
  http_parser* m_parser = nullptr;
  http_parser_settings* m_settings = nullptr;
  void* vdata = nullptr;
};