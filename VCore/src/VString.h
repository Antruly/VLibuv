#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <memory>
#include "VObject.h"
#include <vector>
#include <sstream>
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
  // Comparison operators
  bool operator==(const VString& other) const;

  bool operator!=(const VString& other) const;
  // Overload [] operator to access characters by index
  char& operator[](size_t index);

  // Overload [] operator for const objects
  const char& operator[](size_t index) const;
  // Substring method
  VString substr(size_t pos, size_t count) const;

  // Check if string is empty
  bool empty() const;

  size_t find(const VString& str, size_t pos = 0) const;
  bool contains(const VString& str) const;

  std::vector<VString> split(char delimiter) const;
  std::vector<VString> split(const VString& delimiter) const;

  // replace 方法
  void replace(size_t pos, size_t count, const std::string& str);

  const char* c_str();

  std::string ReplaceAll(const std::string& search, const std::string& replace);

// Convert ASCII to UTF-8
  std::string ASCIIToUTF8() const;

  // Convert UTF-8 to ASCII
  std::string UTF8ToASCII() const;

  // Convert UTF-8 to UTF-16
  std::u16string UTF8ToUTF16() const;

  // Convert UTF-8 to UTF-32
  std::u32string UTF8ToUTF32() const;
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
