#pragma once
#include "VObject.h"
#include <codecvt>
#include <fstream>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class VString : public VObject {
public:
  enum class Encoding { ASCII, UTF8, GBK, UNKNOWN };
  // 构造函数
  explicit VString() {}
  VString(const std::string &str);
  VString(const char *str, size_t count);
  ~VString();
  operator std::string() const;

  // 迭代器类
  class iterator {
  private:
    std::string::iterator it;

  public:
    iterator(std::string::iterator iter);

    char &operator*();
    iterator &operator++();
    bool operator!=(const iterator &other) const;
  };

  // begin 方法
  iterator begin();

  // end 方法
  iterator end();

  // 重载常用方法
  size_t size() const;
  VString &operator=(const VString &other);
  VString &operator=(const std::string &str);

  VString operator+(const VString &other) const;
  VString &operator+=(const VString &other);
  // Comparison operators
  bool operator==(const VString &other) const;

  bool operator!=(const VString &other) const;
  // Overload [] operator to access characters by index
  char &operator[](size_t index);

  // Overload [] operator for const objects
  const char &operator[](size_t index) const;
  // Substring method
  VString substr(size_t pos, size_t count) const;

  // Check if string is empty
  bool empty() const;

  size_t find(const VString &str, size_t pos = 0) const;
  bool contains(const VString &str) const;

  std::vector<VString> split(char delimiter) const;
  std::vector<VString> split(const VString &delimiter) const;

  // replace 方法
  void replace(size_t pos, size_t count, const std::string &str);

  const char *c_str();

  std::string replaceAll(const std::string &search, const std::string &replace);

  std::string urlEncode(const std::string &value);

  std::string urlDecode(const std::string &value);

  std::string utf8ToAscll() const;

  std::u16string utf8ToUtf16() const;

  std::u32string utf8ToUtf32() const;

  std::string gbkToUtf8() const;
  std::string utf8ToGbk() const;
  std::wstring utf8ToUnicode() const;
  std::wstring gbkToUnicode() const;

  Encoding detectEncoding();

  bool isUtf8String();

  bool isGbkString();

  bool isAscllString();

protected:
private:
  std::string data;
};
std::string UnicodeToUTF8(const std::wstring &wstr);

std::wstring UTF8ToUnicode(const std::string &str);

std::string UnicodeToANSI(const std::wstring &wstr);

std::wstring ANSIToUnicode(const std::string &str);

std::string UTF8ToANSI(const std::string &str);

std::string ANSIToUTF8(const std::string &str);

std::string GBKToUTF8(const std::string &str);

std::string UTF8ToGBK(const std::string &str);

std::string replaceAll(const std::string &str, const std::string &search,
                       const std::string &replace);
VString::Encoding DetectEncoding(const std::string &str);

bool IsUtf8String(const std::string &str);

bool IsGbkString(const std::string &str);

bool IsAscllString(const std::string &str);