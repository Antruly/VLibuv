#include <string>
#include <locale>
#include <codecvt>
#include <cstring> // For std::memset
#include <memory> // For std::unique_ptr
#include "VString.h"

#ifdef _WIN32
const char* GBK_LOCALE_NAME = ".936";  // GBK在Windows下的locale name
#else
const char* GBK_LOCALE_NAME = "zh_CN.GB18030";  // Linux下的locale名是"zh_CN.GB18030"
#endif

VString::VString(const std::string& str) : data(str) {}
VString::VString(const char* str, size_t count) : data(str, count) {}
VString::~VString() {}

VString::operator std::string() const {
  return data;
}

// begin 方法

VString::iterator VString::begin() {
  return iterator(data.begin());
}

// end 方法

VString::iterator VString::end() {
  return iterator(data.end());
}


// 重载常用方法

size_t VString::size() const {
  return data.size();
}

VString& VString::operator=(const VString& other) {
  if (this != &other) {
    data = other.data;
  }
  return *this;
}

VString& VString::operator=(const std::string& str) {
  data = str;
  return *this;
}

VString VString::operator+(const VString& other) const {
  return data + other.data;
}

VString& VString::operator+=(const VString& other) {
  data += other.data;
  return *this;
}

// Comparison operators

bool VString::operator==(const VString& other) const {
  return data == other.data;
}

bool VString::operator!=(const VString& other) const {
  return !(*this == other);
}

// Overload [] operator to access characters by index

char& VString::operator[](size_t index) {
  return data[index];
}

// Substring method


// Overload [] operator for const objects

const char& VString::operator[](size_t index) const {
  return data[index];
}

VString VString::substr(size_t pos, size_t count) const {
  return data.substr(pos, count);
}

// Check if string is empty

bool VString::empty() const {
  return data.empty();
}

size_t VString::find(const VString& str, size_t pos) const {
  return data.find(str.data, pos);
}

bool VString::contains(const VString& str) const {
  return find(str) != std::string::npos;
}

std::vector<VString> VString::split(char delimiter) const {
  std::vector<VString> tokens;
  std::istringstream iss(data);
  VString token;

  while (std::getline(iss, token.data, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

std::vector<VString> VString::split(const VString& delimiter) const {
  std::vector<VString> tokens;
  std::istringstream iss(data);
  VString token;
  std::string tempData = data;  // 创建数据的临时副本
  size_t pos = 0;
  while ((pos = tempData.find(delimiter.data, pos)) != std::string::npos) {
    token = tempData.substr(0, pos);
    tokens.push_back(token);
    tempData.erase(0, pos + delimiter.size());
    pos = 0;  // 为下一次搜索重置位置
  }
  tokens.push_back(tempData);  // 在最后一个分隔符后添加剩余的子字符串
  
  return tokens;
}

// replace 方法

void VString::replace(size_t pos, size_t count, const std::string& str) {
  data.replace(pos, count, str);
}

const char* VString::c_str() {
  return data.c_str();
}

std::string VString::ReplaceAll(const std::string& search,
                                       const std::string& replace) {
  std::string result = data;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
}

// Convert ASCII to UTF-8

std::string VString::ASCIIToUTF8() const {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  std::u32string utf32String = converter.from_bytes(data);
  return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(
      utf32String);
}

// Convert UTF-8 to ASCII

std::string VString::UTF8ToASCII() const {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  std::u32string utf32String = converter.from_bytes(data);
  return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(
      utf32String);
}

// Convert UTF-8 to UTF-16

std::u16string VString::UTF8ToUTF16() const {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  std::string utf8String = reinterpret_cast<const char*>(data.c_str());
  return converter.from_bytes(utf8String);
}

// Convert UTF-8 to UTF-32

std::u32string VString::UTF8ToUTF32() const {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  return converter.from_bytes(data);
}

std::string UnicodeToUTF8(const std::wstring& wstr) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.to_bytes(wstr);
}

std::wstring UTF8ToUnicode(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.from_bytes(str);
}

std::string UnicodeToANSI(const std::wstring& wstr) {
  std::string ret;
  std::mbstate_t state;
  std::memset(&state, 0, sizeof(state));  // Initialize the state
  const wchar_t* src = wstr.data();
  size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
  if (static_cast<size_t>(-1) != len) {
    std::unique_ptr<char[]> buff(new char[len + 1]);
    len = std::wcsrtombs(buff.get(), &src, len, &state);
    if (static_cast<size_t>(-1) != len) {
      ret.assign(buff.get(), len);
    }
  }
  return ret;
}

std::wstring ANSIToUnicode(const std::string& str) {
  std::wstring ret;
  std::mbstate_t state;
  std::memset(&state, 0, sizeof(state));  // Initialize the state
  const char* src = str.data();
  size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
  if (static_cast<size_t>(-1) != len) {
    std::unique_ptr<wchar_t[]> buff(new wchar_t[len + 1]);
    len = std::mbsrtowcs(buff.get(), &src, len, &state);
    if (static_cast<size_t>(-1) != len) {
      ret.assign(buff.get(), len);
    }
  }
  return ret;
}

std::string UTF8ToANSI(const std::string& str) {
  return UnicodeToANSI(UTF8ToUnicode(str));
}

std::string ANSIToUTF8(const std::string& str) {
  return UnicodeToUTF8(ANSIToUnicode(str));
}

std::string GBKToUTF8(const std::string& gbkData) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
  std::wstring wString = ANSIToUnicode(gbkData);  // string => wstring
  return convert.to_bytes(wString);               // wstring => utf-8
}

std::string ReplaceAll(const std::string& str,
                       const std::string& search,
                       const std::string& replace) {
  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
}

VString::iterator::iterator(std::string::iterator iter) : it(iter) {}

char& VString::iterator::operator*() {
  return *it;
}

VString::iterator& VString::iterator::operator++() {
  ++it;
  return *this;
}

bool VString::iterator::operator!=(const iterator& other) const {
  return it != other.it;
}
