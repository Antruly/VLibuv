﻿#pragma once
#include "VHttpParser.h"
#include "VMemory.h"
#include <functional>
#include <mutex>
VHttpParser::VHttpParser()
    : VObject(this), message_begin_cb(), url_cb(), status_cb(),
      header_field_cb(), header_value_cb(), headers_complete_cb(), body_cb(),
      message_complete_cb(), chunk_header_cb(), chunk_complete_cb() {
  m_parser = (http_parser *)VMemory::malloc(sizeof(http_parser));
  memset(m_parser, 0, sizeof(http_parser));
  m_settings =
      (http_parser_settings *)VMemory::malloc(sizeof(http_parser_settings));
  memset(m_settings, 0, sizeof(http_parser_settings));
  this->httpParserSettingsInit();
  this->setHttpParserData();
}
VHttpParser::VHttpParser(VHttpParser *t_p) : VObject(t_p) {}
VHttpParser::~VHttpParser() { this->freeHttpParser(); }

int VHttpParser::setData(void *pdata) {
  vdata = pdata;
  return 0;
}

void *VHttpParser::getData() { return vdata; }

const char *VHttpParser::httpErrnoName(http_errno err) {
  return http_errno_name(err);
}

const char *VHttpParser::httpStatusStr(http_status s) {
  return http_status_str(s);
}

const char *VHttpParser::httpMethodStr(http_method m) {
  return http_method_str(m);
}

void VHttpParser::setHttpParserData() { m_parser->data = this; }

int VHttpParser::callback_message_begin(http_parser *m_parser) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->message_begin_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->message_begin_cb(reinterpret_cast<VHttpParser *>(m_parser->data));
}

int VHttpParser::callback_url(http_parser *m_parser, const char *pdata,
                              size_t size) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->url_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->url_cb(reinterpret_cast<VHttpParser *>(m_parser->data), pdata, size);
}

int VHttpParser::callback_status(http_parser *m_parser, const char *pdata,
                                 size_t size) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->status_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->status_cb(reinterpret_cast<VHttpParser *>(m_parser->data), pdata,
                    size);
}

int VHttpParser::callback_header_field(http_parser *m_parser, const char *pdata,
                                       size_t size) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->header_field_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->header_field_cb(reinterpret_cast<VHttpParser *>(m_parser->data),
                          pdata, size);
}

int VHttpParser::callback_header_value(http_parser *m_parser, const char *pdata,
                                       size_t size) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->header_value_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->header_value_cb(reinterpret_cast<VHttpParser *>(m_parser->data),
                          pdata, size);
}

int VHttpParser::callback_headers_complete(http_parser *m_parser) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->headers_complete_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->headers_complete_cb(reinterpret_cast<VHttpParser *>(m_parser->data));
}

int VHttpParser::callback_body(http_parser *m_parser, const char *pdata,
                               size_t size) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->body_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->body_cb(reinterpret_cast<VHttpParser *>(m_parser->data), pdata, size);
}

int VHttpParser::callback_message_complete(http_parser *m_parser) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->message_complete_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->message_complete_cb(reinterpret_cast<VHttpParser *>(m_parser->data));
}

int VHttpParser::callback_chunk_header(http_parser *m_parser) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->chunk_header_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->chunk_header_cb(reinterpret_cast<VHttpParser *>(m_parser->data));
}

int VHttpParser::callback_chunk_complete(http_parser *m_parser) {
  if (reinterpret_cast<VHttpParser *>(m_parser->data)->chunk_complete_cb)
    return reinterpret_cast<VHttpParser *>(m_parser->data)
        ->chunk_complete_cb(reinterpret_cast<VHttpParser *>(m_parser->data));
}

VHttpParser::VHttpParser(const VHttpParser &obj) : VObject(nullptr) {
  if (obj.m_parser != nullptr) {
    m_parser = (http_parser *)VMemory::malloc(sizeof(http_parser));
    memcpy(obj.m_parser, this->m_parser, sizeof(http_parser));
    this->setHttpParserData();
    vdata = obj.vdata;
  } else {
    m_parser = nullptr;
  }

  if (obj.m_settings != nullptr) {
    m_settings =
        (http_parser_settings *)VMemory::malloc(sizeof(http_parser_settings));
    memcpy(obj.m_settings, this->m_settings, sizeof(http_parser_settings));
    this->httpParserSettingsInit();
  } else {
    m_settings = nullptr;
  }
}

VHttpParser &VHttpParser::operator=(const VHttpParser &obj) {
  this->freeHttpParser();

  if (obj.m_parser != nullptr) {
    m_parser = (http_parser *)VMemory::malloc(sizeof(http_parser));
    memcpy(obj.m_parser, this->m_parser, sizeof(http_parser));
    this->setHttpParserData();
    vdata = obj.vdata;
  } else {
    m_parser = nullptr;
  }

  if (obj.m_settings != nullptr) {
    m_settings =
        (http_parser_settings *)VMemory::malloc(sizeof(http_parser_settings));
    memcpy(obj.m_settings, this->m_settings, sizeof(http_parser_settings));
    this->httpParserSettingsInit();
  } else {
    m_settings = nullptr;
  }
  return *this;
}

int VHttpParser::httpBodyIsFinal() { return http_body_is_final(m_parser); }

void VHttpParser::setMessageBeginCallback(
    std::function<int(VHttpParser *)> callback) {
  message_begin_cb = callback;
}

void VHttpParser::setUrlCallback(
    std::function<int(VHttpParser *, const char *, size_t)> callback) {
  url_cb = callback;
}

void VHttpParser::setStatusCallback(
    std::function<int(VHttpParser *, const char *, size_t)> callback) {
  status_cb = callback;
}

void VHttpParser::setHeaderFieldCallback(
    std::function<int(VHttpParser *, const char *, size_t)> callback) {
  header_field_cb = callback;
}

void VHttpParser::setHeaderValueCallback(
    std::function<int(VHttpParser *, const char *, size_t)> callback) {
  header_value_cb = callback;
}

void VHttpParser::setHeadersCompleteCallback(
    std::function<int(VHttpParser *)> callback) {
  headers_complete_cb = callback;
}

void VHttpParser::setBodyCallback(
    std::function<int(VHttpParser *, const char *, size_t)> callback) {
  body_cb = callback;
}

void VHttpParser::setMessageCompleteCallback(
    std::function<int(VHttpParser *)> callback) {
  message_complete_cb = callback;
}

void VHttpParser::setChunkHeaderCallback(
    std::function<int(VHttpParser *)> callback) {
  chunk_header_cb = callback;
}

void VHttpParser::setChunkCompleteCallback(
    std::function<int(VHttpParser *)> callback) {
  chunk_complete_cb = callback;
}

// 解析URL并返回解析结果

ParsedURL VHttpParser::parseUrl(const std::string &url) {
  ParsedURL result;

  // 创建 http_parser_url 结构体实例
  http_parser_url parsed_url;

  // 使用 http_parser_parse_url 函数解析 URL
  int parse_result =
      http_parser_parse_url(url.c_str(), url.length(), 0, &parsed_url);
  if (parse_result != 0) {
    Log->logError("Failed to parse URL:%s\n", url.c_str());
    return result; // 返回一个空的解析结果
  }

  // 解析协议
  if (parsed_url.field_set & (1 << UF_SCHEMA)) {
    result.protocol = url.substr(parsed_url.field_data[UF_SCHEMA].off,
                                 parsed_url.field_data[UF_SCHEMA].len);
  }

  // 解析主机名
  if (parsed_url.field_set & (1 << UF_HOST)) {
    result.host = url.substr(parsed_url.field_data[UF_HOST].off,
                             parsed_url.field_data[UF_HOST].len);
  }

  // 解析端口号
  if (parsed_url.field_set & (1 << UF_PORT)) {
    result.port = parsed_url.port;
  } else {
    // 如果没有指定端口号，默认使用80端口（HTTP）或443端口（HTTPS）
    if (result.protocol == "https") {
      result.port = 443;
    } else {
      result.port = 80;
    }
  }

  // 解析路径
  if (parsed_url.field_set & (1 << UF_PATH)) {
    result.path = url.substr(parsed_url.field_data[UF_PATH].off,
                             parsed_url.field_data[UF_PATH].len);
  } else {
    result.path = "/";
  }

  // 解析查询字符串
  if (parsed_url.field_set & (1 << UF_QUERY)) {
    result.query = url.substr(parsed_url.field_data[UF_QUERY].off,
                              parsed_url.field_data[UF_QUERY].len);
  }

  return result;
}

Cookie VHttpParser::parseSetCookieHeader(const VString &header,
                                         const VString &requestDomain) {
  Cookie cookie;
  std::istringstream iss(header);
  std::string part;

  // 解析name=value部分
  std::getline(iss, part, ';');
  size_t pos = part.find('=');
  cookie.name = part.substr(0, pos);
  cookie.value = part.substr(pos + 1);

  // 解析属性
  while (std::getline(iss, part, ';')) {
    // trim(part); // 需要实现trim函数
    pos = part.find('=');
    VString key = (pos != VString::npos) ? part.substr(0, pos) : part;
    VString value = (pos != VString::npos) ? part.substr(pos + 1) : "";

    if (key == "Domain") {
      cookie.domain = value;
    } else if (key == "Path") {
      cookie.path = value;
    } else if (key == "Expires") {
      std::tm tm = {};
      std::istringstream timeIss(value);
      timeIss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
      cookie.expires = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else if (key == "Max-Age") {
      auto maxAge = std::chrono::seconds(std::stol(value));
      cookie.expires = std::chrono::system_clock::now() + maxAge;
    } else if (key == "Secure") {
      cookie.secure = true;
    } else if (key == "HttpOnly") {
      cookie.httpOnly = true;
    }
  }

  if (cookie.domain.empty())
    cookie.domain = requestDomain;
  if (cookie.path.empty())
    cookie.path = "/";
  return cookie;
}

std::map<VString, VString> VHttpParser::parseUrlEncoded(const VString& body)
{
    std::map<VString, VString> params;

    // 空字符串直接返回空结果
    if (body.empty()) {
        return params;
    }

    // 分割键值对
    std::vector<VString> pairs = body.split('&');

    for (const VString& pair : pairs) {
        // 分割键和值（只分割第一个'='）
        size_t pos = pair.find('=');
        if (pos == VString::npos) {
            // 没有等号，作为没有值的键处理
            VString key = pair;
            params[key.urlDecode()] = VString();
        }
        else {
            // 分割键和值
            VString key = pair.substr(0, pos);
            VString value = pair.substr(pos + 1);

            // URL 解码后存入 map
            params[key.urlDecode()] = value.urlDecode();
        }
    }

    return params;
}

std::string VHttpParser::getMethodName(const METHOD_TYPE &method_type) {

  switch (method_type) {
  case OPTIONS:
    return "OPTIONS";
  case GET:
    return "GET";
  case HEAD:
    return "HEAD";
  case POST:
    return "POST";
  case PUT:
    return "PUT";
  case DELETE_METHOD:
    return "DELETE_METHOD";
  case TRACE:
    return "TRACE";
  case CONNECT:
    return "CONNECT";
  default:
    return "";
  }
}

METHOD_TYPE VHttpParser::getMethodType(const std::string &method_name) {

  if (method_name == "OPTIONS") {
    return METHOD_TYPE::OPTIONS;
  } else if (method_name == "GET") {
    return METHOD_TYPE::GET;
  } else if (method_name == "HEAD") {
    return METHOD_TYPE::HEAD;
  } else if (method_name == "POST") {
    return METHOD_TYPE::POST;
  } else if (method_name == "PUT") {
    return METHOD_TYPE::PUT;
  } else if (method_name == "DELETE_METHOD") {
    return METHOD_TYPE::DELETE_METHOD;
  } else if (method_name == "TRACE") {
    return METHOD_TYPE::TRACE;
  } else if (method_name == "CONNECT") {
    return METHOD_TYPE::CONNECT;
  }
  return METHOD_TYPE();
}

http_parser *VHttpParser::getHttpParser() const { return m_parser; }

unsigned long VHttpParser::httpParserVersion(void) {
  return http_parser_version();
}

ParsedURL VHttpParser::ParseUrl(const std::string &url) {
  ParsedURL result;

  // 创建 http_parser_url 结构体实例
  http_parser_url parsed_url;

  // 使用 http_parser_parse_url 函数解析 URL
  int parse_result =
      http_parser_parse_url(url.c_str(), url.length(), 0, &parsed_url);
  if (parse_result != 0) {
    Log->logError("Failed to parse URL:%s\n", url.c_str());
    return result; // 返回一个空的解析结果
  }

  // 解析协议
  if (parsed_url.field_set & (1 << UF_SCHEMA)) {
    result.protocol = url.substr(parsed_url.field_data[UF_SCHEMA].off,
                                 parsed_url.field_data[UF_SCHEMA].len);
  }

  // 解析主机名
  if (parsed_url.field_set & (1 << UF_HOST)) {
    result.host = url.substr(parsed_url.field_data[UF_HOST].off,
                             parsed_url.field_data[UF_HOST].len);
  }

  // 解析端口号
  if (parsed_url.field_set & (1 << UF_PORT)) {
    result.port = parsed_url.port;
  } else {
    // 如果没有指定端口号，默认使用80端口（HTTP）或443端口（HTTPS）
    if (result.protocol == "https") {
      result.port = 443;
    } else {
      result.port = 80;
    }
  }

  // 解析路径
  if (parsed_url.field_set & (1 << UF_PATH)) {
    result.path = url.substr(parsed_url.field_data[UF_PATH].off,
                             parsed_url.field_data[UF_PATH].len);
  } else {
    result.path = "/";
  }

  // 解析查询字符串
  if (parsed_url.field_set & (1 << UF_QUERY)) {
    result.query = url.substr(parsed_url.field_data[UF_QUERY].off,
                              parsed_url.field_data[UF_QUERY].len);
  }

  return result;
}

void VHttpParser::setHttpParser(void *hd) {
  this->freeHttpParser();
  m_parser = (http_parser *)hd;
  this->setHttpParserData();
  return;
}

void VHttpParser::freeHttpParser() {
  if (m_parser != nullptr) {
    VMemory::free(m_parser);
    m_parser = nullptr;
  }
  if (m_settings != nullptr) {
    VMemory::free(m_settings);
    m_settings = nullptr;
  }
}

void VHttpParser::httpParserSettingsInit() {
  http_parser_settings_init(m_settings);
  m_settings->on_message_begin = &VHttpParser::callback_message_begin;
  m_settings->on_url = &VHttpParser::callback_url;
  m_settings->on_status = &VHttpParser::callback_status;
  m_settings->on_header_field = &VHttpParser::callback_header_field;
  m_settings->on_header_value = &VHttpParser::callback_header_value;
  m_settings->on_headers_complete = &VHttpParser::callback_headers_complete;
  m_settings->on_body = &VHttpParser::callback_body;
  m_settings->on_message_complete = &VHttpParser::callback_message_complete;
  /* When on_chunk_header is called, the current chunk length is stored
   * in parser->content_length.
   */
  m_settings->on_chunk_header = &VHttpParser::callback_chunk_header;
  m_settings->on_chunk_complete = &VHttpParser::callback_chunk_complete;
}

http_parser_settings *VHttpParser::getHttpCallBack() { return m_settings; }

void VHttpParser::httpParserInit(http_parser_type type) {
  http_parser_init(m_parser, type);
}

size_t VHttpParser::httpParserExecute(const char *data, size_t len) {
  return http_parser_execute(m_parser, m_settings, data, len);
}

int VHttpParser::httpShouldKeepAlive() {
  return http_should_keep_alive(m_parser);
}

void VHttpParser::httpParserPause(int paused) {
  http_parser_pause(m_parser, paused);
}

const char *VHttpParser::httpErrnoDescription(http_errno err) {
  return http_errno_description(err);
}

void VHttpParser::httpParserUrlInit(http_parser_url *u) {
  http_parser_url_init(u);
}

int VHttpParser::httpParserParseUrl(const char *buf, size_t buflen,
                                    int isConnect, http_parser_url *u) {
  return http_parser_parse_url(buf, buflen, isConnect, u);
}

void VHttpParser::httpParserSetMaxHeaderSize(uint32_t size) {
  http_parser_set_max_header_size(size);
}

// 辅助函数：路径匹配

void CookieJar::addCookie(const Cookie &cookie) {
  // 移除旧的同名、同域名、同路径的Cookie
  cookies.erase(std::remove_if(cookies.begin(), cookies.end(),
                               [&](const Cookie &c) {
                                 return c.name == cookie.name &&
                                        c.domain == cookie.domain &&
                                        c.path == cookie.path;
                               }),
                cookies.end());
  // 添加新Cookie
  cookies.push_back(cookie);
}

std::vector<Cookie> CookieJar::getCookiesForRequest(const VString &url,
                                                    bool isSecure) const {
  VString host = this->extractHost(url);
  VString path = this->extractPath(url);
  std::vector<Cookie> result;

  for (const auto &cookie : cookies) {
    if (this->isCookieExpired(cookie))
      continue;
    if (!this->domainMatches(host, cookie.domain))
      continue;
    if (!this->pathMatches(path, cookie.path))
      continue;
    if (cookie.secure && !isSecure)
      continue;
    result.push_back(cookie);
  }

  return result;
}

std::vector<Cookie> &CookieJar::getCookies() { return cookies; }

void CookieJar::removeExpiredCookies() {
  auto now = std::chrono::system_clock::now();
  cookies.erase(
      std::remove_if(cookies.begin(), cookies.end(),
                     [now](const Cookie &c) { return c.expires < now; }),
      cookies.end());
}

// 辅助函数：域名匹配

bool CookieJar::domainMatches(const VString &host,
                              const VString &domain) const {
  VString normDomain = domain.startsWith('.') ? domain.substr(1) : domain;
  return host == normDomain ||
         (host.endsWith(normDomain) &&
          host[host.size() - normDomain.size() - 1] == '.');
}

bool CookieJar::pathMatches(const VString &requestPath,
                            const VString &cookiePath) const {
  return requestPath.startsWith(cookiePath) &&
         (cookiePath.empty() || cookiePath.back() == '/' ||
          requestPath.size() <= cookiePath.size() ||
          requestPath[cookiePath.size()] == '/');
}

// 提取URL的host和path（需要具体实现）

VString CookieJar::extractHost(const VString &url) const {
  return VHttpParser::ParseUrl(url).host;
}

VString CookieJar::extractPath(const VString &url) const {
  return VHttpParser::ParseUrl(url).path;
}

bool CookieJar::isCookieExpired(const Cookie &cookie) const {
  if (cookie.isSession) {
    return false; // 会话Cookie由浏览器生命周期管理
  }
  static std::chrono::system_clock::time_point lastCheck;
  static std::mutex timeMutex;
  static std::chrono::seconds tolerance = std::chrono::seconds(1);
  {
    std::lock_guard<std::mutex> lock(timeMutex);
    if (std::chrono::system_clock::now() + tolerance - lastCheck >
        std::chrono::seconds(1)) {
      lastCheck = std::chrono::system_clock::now();
    }
  }

  return cookie.expires < lastCheck;
}
