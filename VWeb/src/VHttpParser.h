#pragma once
extern "C" {
#include "http_parser.h"
}
#include "VCoreDefine.h"
#include "VObject.h"
#include "VWebDefine.h"
#include <chrono>
#include <functional>
#include <iomanip>
#include <sstream>
#include <VString.h>

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#include <algorithm>
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#if __cplusplus >= 201103L
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

class HttpHeaders {
public:
  using Header = std::pair<std::string, std::string>;
  using iterator = std::vector<Header>::iterator;
  using const_iterator = std::vector<Header>::const_iterator;

private:
  std::vector<Header> headers;

  // 索引结构
  struct Index {
    std::unordered_map<std::string, std::vector<size_t>> data;
    bool enabled;
    Index() : enabled(false) {}
  };
  std::shared_ptr<Index> index;

  // 统一转小写
  static std::string toLower(const std::string &str) {
    std::string lower;
    lower.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
      lower.push_back(static_cast<char>(std::tolower(str[i])));
    }
    return lower;
  }

  // 更新索引
  void updateIndex(size_t pos, const std::string &key) {
    if (index && index->enabled) {
      const std::string lkey = toLower(key);
      index->data[lkey].push_back(pos);
    }
  }

  // 重建索引
  void rebuildIndex() {
    if (index) {
      index->data.clear();
      for (size_t i = 0; i < headers.size(); ++i) {
        const std::string lkey = toLower(headers[i].first);
        index->data[lkey].push_back(i);
      }
    }
  }

public:
  // 构造函数
  HttpHeaders(bool enableIndex = false) : index(new Index()) {
    index->enabled = enableIndex;
  }

  //// 拷贝构造函数
  //HttpHeaders(const HttpHeaders &other)
  //    : headers(other.headers), index(new Index(*other.index)) {}
  // 拷贝构造函数（深拷贝）
  HttpHeaders(const HttpHeaders &other)
      : headers(other.headers), index(std::make_shared<Index>(*other.index)) {
  } // 深拷贝index

  // 赋值运算符（copy-and-swap 惯用法）
  HttpHeaders &operator=(HttpHeaders other) { // 注意：参数是值传递
    swap(*this, other);
    return *this;
  }

  // 交换函数（用于copy-and-swap）
  friend void swap(HttpHeaders &first, HttpHeaders &second) NOEXCEPT {
    using std::swap;
    swap(first.headers, second.headers);
    swap(first.index, second.index);
  }
  // 基础接口
  void push_back(const Header &h) {
    headers.push_back(h);
    updateIndex(headers.size() - 1, h.first);
  }

  size_t size() const { return headers.size(); }
  bool empty() const { return headers.empty(); }
  void clear() {
    headers.clear();
    if (index)
      index->data.clear();
  }

  // 查找（兼容C++11）
  iterator find(const std::string &key) {
    const std::string lkey = toLower(key);
    if (index && index->enabled) {
      std::unordered_map<std::string, std::vector<size_t>>::iterator mapIt =
          index->data.find(lkey);
      if (mapIt != index->data.end() && !mapIt->second.empty()) {
        return headers.begin() + mapIt->second.front();
      }
      return headers.end();
    }

    for (iterator it = headers.begin(); it != headers.end(); ++it) {
      if (toLower(it->first) == lkey)
        return it;
    }
    return headers.end();
  }

  // 删除所有匹配项（C++11版本）
  size_t erase(const std::string &key) {
    const std::string lkey = toLower(key);
    size_t oldSize = headers.size();

    if (index && index->enabled) {
      std::unordered_map<std::string, std::vector<size_t>>::iterator mapIt =
          index->data.find(lkey);
      if (mapIt != index->data.end()) {
        std::vector<size_t> &positions = mapIt->second;
        std::sort(positions.begin(), positions.end(), std::greater<size_t>());
        for (std::vector<size_t>::iterator posIt = positions.begin();
             posIt != positions.end(); ++posIt) {
          if (*posIt < headers.size()) {
            headers.erase(headers.begin() + *posIt);
          }
        }
        index->data.erase(mapIt);
      }
    } else {
      headers.erase(std::remove_if(headers.begin(), headers.end(),
                                   [&](const Header &h) {
                                     return toLower(h.first) == lkey;
                                   }),
                    headers.end());
    }
    return oldSize - headers.size();
  }

  // 迭代器删除（C++11兼容）
  iterator erase(const_iterator pos) {
    size_t offset = pos - headers.begin();

    iterator newIt = headers.erase(headers.begin() + offset);

    if (index && index->enabled) {
      for (std::unordered_map<std::string, std::vector<size_t>>::iterator
               mapIt = index->data.begin();
           mapIt != index->data.end(); ++mapIt) {
        std::vector<size_t> &positions = mapIt->second;
        for (std::vector<size_t>::iterator it = positions.begin();
             it != positions.end();) {
          if (*it > offset) {
            --(*it);
            ++it;
          } else if (*it == offset) {
            it = positions.erase(it);
          } else {
            ++it;
          }
        }
      }
    }
    return newIt;
  }

  // 其他必要接口
  const std::string &at(const std::string &key) const {
    for (const_iterator it = headers.begin(); it != headers.end(); ++it) {
      if (toLower(it->first) == toLower(key))
        return it->second;
    }
    throw std::out_of_range("Key not found");
  }

  std::string &operator[](const std::string &key) {
    iterator it = find(key);
    if (it != end())
      return it->second;

    push_back(Header(key, ""));
    return headers.back().second;
  }

  // 迭代器
  iterator begin() { return headers.begin(); }
  iterator end() { return headers.end(); }
  const_iterator begin() const { return headers.begin(); }
  const_iterator end() const { return headers.end(); }

  // 索引控制
  void enableIndex(bool enable = true) {
    index->enabled = enable;
    if (enable)
      rebuildIndex();
  }
};

// URL解析结果结构体
struct ParsedURL {
  std::string protocol;
  std::string host;
  uint16_t port;
  std::string path;
  std::string query;
};

struct Cookie {
  VString name;
  VString value;
  VString domain;
  VString path;
  VString sameSite;
  std::chrono::system_clock::time_point expires;
  bool secure = false;
  bool httpOnly = false;
  bool isSession = false; // 会话Cookie（无过期时间）

  VString Cookie::toHeaderString() const {
    // 基础 name=value 部分（强制URL编码）
    VString result = VString::UrlEncode(name) + "=" + VString::UrlEncode(value);

    // 域名处理（RFC 6265 5.2.3）
    if (!domain.empty()) {
      // 域名必须以点开头时添加前导点
      if (domain.front() != '.') {
        result += VString::Format("; Domain=.%s",domain.c_str());
      } else {
        result += VString::Format("; Domain=%s", domain.c_str());
      }
    }

    // 路径处理（RFC 6265 5.2.4）
    if (!path.empty() && path != "/") {
      result += VString::Format("; Path=%s", path.c_str());

    }

    // 过期时间处理（RFC 6265 5.2.1）
    if (!isSession) {
      // 转换为GMT时间字符串（线程安全版本）
      auto tt = std::chrono::system_clock::to_time_t(expires);
      std::tm tm;
#ifdef _WIN32
      gmtime_s(&tm, &tt); // Windows线程安全版本
#else
      gmtime_r(&tt, &tm); // POSIX线程安全版本
#endif

      std::stringstream ss;
      ss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
      result += "; Expires=" + ss.str();
    }

    // 安全标志（RFC 6265 5.2.5）
    if (secure) {
      result += "; Secure";
    }

    // HTTPOnly标志（RFC 6265 5.2.6）
    if (httpOnly) {
      result += "; HttpOnly";
    }

    // SameSite处理（RFC 6265bis）
    if (!sameSite.empty()) {
      result += VString::Format("; SameSite=%s", sameSite.c_str());
    }

    // 最大长度限制（RFC 6265 5.3）
    if (result.size() > 4096) {
      throw std::runtime_error(
          "Cookie header exceeds maximum size of 4096 bytes");
    }

    return result;
  }
};
class CookieJar {
public:
  void addCookie(const Cookie &cookie);

  std::vector<Cookie> getCookiesForRequest(const VString &url,
                                           bool isSecure) const;
  std::vector<Cookie> &getCookies();

  void removeExpiredCookies();

private:
  std::vector<Cookie> cookies;

  // 辅助函数：域名匹配
  bool domainMatches(const VString &host, const VString &domain) const;

  // 辅助函数：路径匹配
  bool pathMatches(const VString &requestPath,
                   const VString &cookiePath = VString()) const;

  // 提取URL的host和path（需要具体实现）
  VString extractHost(const VString &url) const;
  VString extractPath(const VString &url) const;
  bool isCookieExpired(const Cookie &cookie) const;
};

class VHttpParser : public VObject {
public:
  VCORE_DEFINE_INHERIT_FUNC(VHttpParser);
  VCORE_DEFINE_COPY_FUNC(VHttpParser);

  /* Initialize http_parser members. */
  void httpParserInit(enum http_parser_type type);

  /* Executes the parser. Returns number of parsed bytes. Sets
   * `parser->http_errno` on error. */
  size_t httpParserExecute(const char *data, size_t len);

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
  void setMessageBeginCallback(std::function<int(VHttpParser *)> callback);

  // 设置 url_cb 回调函数
  void setUrlCallback(
      std::function<int(VHttpParser *, const char *, size_t)> callback);

  // 设置 status_cb 回调函数
  void setStatusCallback(
      std::function<int(VHttpParser *, const char *, size_t)> callback);

  // 设置 header_field_cb 回调函数
  void setHeaderFieldCallback(
      std::function<int(VHttpParser *, const char *, size_t)> callback);

  // 设置 header_value_cb 回调函数
  void setHeaderValueCallback(
      std::function<int(VHttpParser *, const char *, size_t)> callback);

  // 设置 headers_complete_cb 回调函数
  void setHeadersCompleteCallback(std::function<int(VHttpParser *)> callback);

  // 设置 body_cb 回调函数
  void setBodyCallback(
      std::function<int(VHttpParser *, const char *, size_t)> callback);

  // 设置 message_complete_cb 回调函数
  void setMessageCompleteCallback(std::function<int(VHttpParser *)> callback);

  // 设置 chunk_header_cb 回调函数
  void setChunkHeaderCallback(std::function<int(VHttpParser *)> callback);

  // 设置 chunk_complete_cb 回调函数
  void setChunkCompleteCallback(std::function<int(VHttpParser *)> callback);

  // 解析URL并返回解析结果
  ParsedURL parseUrl(const std::string &url);
  Cookie parseSetCookieHeader(const VString &header,
                              const VString &requestDomain = VString());

  std::string getMethodName(const METHOD_TYPE &method_type);
  METHOD_TYPE getMethodType(const std::string &method_name);

  http_parser *getHttpParser() const;

  int setData(void *pdata);
  void *getData();

public:
  /* Returns a string version of the HTTP method. */
  static const char *httpMethodStr(enum http_method m);

  /* Returns a string version of the HTTP status code. */
  static const char *httpStatusStr(enum http_status s);

  /* Return a string name of the given error. */
  static const char *httpErrnoName(enum http_errno err);

  /* Return a string description of the given error. */
  static const char *httpErrnoDescription(enum http_errno err);

  /* Initialize all http_parser_url members to 0. */
  static void httpParserUrlInit(struct http_parser_url *u);

  /* Parse a URL; return nonzero on failure. */
  static int httpParserParseUrl(const char *buf, size_t buflen, int isConnect,
                                struct http_parser_url *u);

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

  static ParsedURL ParseUrl(const std::string &url);

protected:
  void setHttpParser(void *hd);
  void setHttpParserData();

  static int callback_message_begin(http_parser *m_parser);
  static int callback_url(http_parser *m_parser, const char *pdata,
                          size_t size);
  static int callback_status(http_parser *m_parser, const char *pdata,
                             size_t size);
  static int callback_header_field(http_parser *m_parser, const char *pdata,
                                   size_t size);
  static int callback_header_value(http_parser *m_parser, const char *pdata,
                                   size_t size);
  static int callback_headers_complete(http_parser *m_parser);
  static int callback_body(http_parser *m_parser, const char *pdata,
                           size_t size);
  static int callback_message_complete(http_parser *m_parser);
  static int callback_chunk_header(http_parser *m_parser);
  static int callback_chunk_complete(http_parser *m_parser);

private:
  void freeHttpParser();
  /* Initialize http_parser_settings members to 0. */
  void httpParserSettingsInit();
  http_parser_settings *getHttpCallBack();

protected:
  std::function<int(VHttpParser *)> message_begin_cb;
  std::function<int(VHttpParser *, const char *, size_t)> url_cb;
  std::function<int(VHttpParser *, const char *, size_t)> status_cb;
  std::function<int(VHttpParser *, const char *, size_t)> header_field_cb;
  std::function<int(VHttpParser *, const char *, size_t)> header_value_cb;
  std::function<int(VHttpParser *)> headers_complete_cb;
  std::function<int(VHttpParser *, const char *, size_t)> body_cb;
  std::function<int(VHttpParser *)> message_complete_cb;
  std::function<int(VHttpParser *)> chunk_header_cb;
  std::function<int(VHttpParser *)> chunk_complete_cb;

private:
  http_parser *m_parser = nullptr;
  http_parser_settings *m_settings = nullptr;
  void *vdata = nullptr;
};
