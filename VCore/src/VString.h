#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <memory>
#include "VObject.h"
class VString :public VObject {
 public:

  // 构造函数
  explicit VString() {}
  VString(const std::string& str);
  VString(const char* str, size_t count);
  ~VString();
  operator std::string() const;

  // 迭代器类
  class iterator {
   private:
    std::string::iterator it;

   public:
    iterator(std::string::iterator iter);

    char& operator*();
    iterator& operator++();
    bool operator!=(const iterator& other) const;
  };

  // begin 方法
  iterator begin();

  // end 方法
  iterator end();

  // 重载常用方法
  size_t size() const;
  VString& operator=(const VString& other);
  VString& operator=(const std::string& str);

  VString operator+(const VString& other) const;
  VString& operator+=(const VString& other);
  size_t find(const VString& str, size_t pos = 0) const;
  // replace 方法
  void replace(size_t pos, size_t count, const std::string& str);

  std::string UnicodeToUTF8(const std::wstring& wstr);

  std::wstring UTF8ToUnicode(const std::string& str);

  std::string UnicodeToANSI(const std::wstring& wstr);

  std::wstring ANSIToUnicode(const std::string& str);

  std::string UTF8ToANSI(const std::string& str);

  std::string ANSIToUTF8(const std::string& str);

  std::string GBKToUTF8(const std::string& gbkData);

  std::string ReplaceAll(const std::string& search,
                         const std::string& replace);

 protected:
 private:
  std::string data;

};
std::string UnicodeToUTF8(const std::wstring& wstr);

std::wstring UTF8ToUnicode(const std::string& str);

std::string UnicodeToANSI(const std::wstring& wstr);

std::wstring ANSIToUnicode(const std::string& str);

std::string UTF8ToANSI(const std::string& str);

std::string ANSIToUTF8(const std::string& str);

std::string GBKToUTF8(const std::string& gbkData);

std::string ReplaceAll(const std::string& str,
                        const std::string& search,
                       const std::string& replace);
