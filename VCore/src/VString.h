#pragma once
#include "VObject.h"
#include <codecvt>
#include <cstdarg>
#include <locale>
#include <regex>
#include <string>
#include <vector>
#include <cctype>

#define WHITESPACE_CHARS "　 \t\n\r\f\v"

class VString : public VObject {
public:
  enum class Encoding { ASCII, UTF8, GBK, UNKNOWN };
  static const size_t npos = static_cast<size_t>(-1);
  // 定义空白字符集合

  explicit VString();
  VString(const std::string &str);
  VString(const VString &str);
  VString(const char ch);
  VString(const char *str);
  VString(const char *str, size_t count);
  ~VString();

  operator std::string() const;

  const std::string &stdString() const;

  // std::string 方法实现
  void push_back(char _Ch);
  void pop_back();
  char &front();
  const char &front() const;
  char &back();
  const char &back() const;
  const char *c_str() const;
  const char *data() const;
  size_t length() const;
  size_t size() const;
  size_t max_size() const;
  void resize(size_t _Newsize);
  void resize(size_t _Newsize, char _Ch);
  size_t capacity() const;
  void reserve(size_t _Newcap = 0);
  bool empty() const;

  // 基础字符串操作
  char &operator[](size_t index);
  const char &operator[](size_t index) const;
  VString &operator=(const VString &other);
  VString operator+(const VString &other) const;
  VString &operator+=(const VString &other);
  bool operator==(const VString &other) const;
  bool operator!=(const VString &other) const;
  bool operator<(const VString &other) const;
  bool operator>(const VString &other) const;
  bool operator>=(const VString &other) const;
  bool operator<=(const VString &other) const;



  // 输出运算符 <<
  friend std::ostream &operator<<(std::ostream &os, const VString &str);
  friend const std::ostream &operator<<(const std::ostream &os,
                                       const VString &str);
  // 输入运算符 >>
  friend std::istream &operator>>(std::istream &is, VString &str);

  // 字符串功能方法
  VString substr(size_t pos, size_t count = npos) const;
  size_t find(const VString &str, size_t pos = 0) const;
  size_t find(char ch, size_t pos = 0) const;
  size_t rfind(const VString &str, size_t pos = 0) const;
  size_t rfind(char ch, size_t pos = 0) const;
  void trim();
  void trimUtf8();
  VString trimmedUtf8() const;
  void trimGbk();
  VString trimmedGbk() const;
  // 新增自动trim方法
  void trimAuto();
  VString trimmedAuto() const;
  // 返回新对象的trim版本
  VString trimmed() const;
  // 支持自定义trim字符集的版本
  VString &trimChars(const VString &chars = WHITESPACE_CHARS);
  bool contains(const VString &str) const;
  bool startsWith(const VString &prefix) const;
  bool endsWith(const VString &suffix) const;
  std::vector<VString> split(char delimiter) const;
  std::vector<VString> split(const VString &delimiter) const;
  void replace(size_t pos, size_t count, const VString &str);
  VString replaceAll(const VString &search, const VString &replace);
  VString& urlEncode();
  VString& urlDecode();

  // 编码转换
  VString utf8ToAscii() const;
  std::u16string utf8ToUtf16() const;
  std::u32string utf8ToUtf32() const;
  VString gbkToUtf8() const;
  VString utf8ToGbk() const;
  std::wstring utf8ToUnicode() const;
  std::wstring gbkToUnicode() const;

  // 编码检测
  Encoding detectEncoding() const;
  bool isUtf8String() const;
  bool isGbkString() const;
  bool isAsciiString() const;

  // 格式转换
  VString toString(Encoding encoding) const;
  VString toUtf8String() const;
  VString toGbkString() const;
  VString toAsciiString() const;
  VString toSystemString() const;
  VString toUpper() const;
  VString toLower() const;
  int toInt() const;
  unsigned int toUInt() const;
  double toDouble() const;
  int64_t toInt64() const;
  uint64_t toUInt64() const;

  // 工具方法
  static VString Format(const char *fmt, ...);
  static VString Format(const VString &fmt, ...);
  static VString UrlEncode(const VString &str);
  static VString UrlDecode(const VString &str);

  // 迭代器支持
  class iterator {
  public:
    iterator(std::string::iterator iter);
    char &operator*();
    iterator &operator++();
    bool operator!=(const iterator &other) const;

  private:
    std::string::iterator it;
  };
  iterator begin();
  iterator end();

private:
  std::string vdata;
  size_t utf8CharLength(unsigned char c) const;
  size_t gbkCharLength(unsigned char c) const;
};

// 系统级编码转换函数
VString::Encoding SystemEncoding();
std::string UnicodeToUTF8(const std::wstring &wstr);
std::wstring UTF8ToUnicode(const std::string &str);
std::string UTF8ToANSI(const std::string &str);
std::string ANSIToUTF8(const std::string &str);
unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);
size_t CountVarArgs(const char *fmt, va_list args);
