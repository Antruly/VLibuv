#include "VString.h"
#include <assert.h>
#include <cctype>
#include <codecvt>
#include <cstring> // For std::memset
#include <iomanip>
#include <locale>
#include <memory> // For std::unique_ptr
#include <string>

#ifdef _WIN32
const char *GBK_LOCALE_NAME = ".936"; // GBK在Windows下的locale name
#else
const char *GBK_LOCALE_NAME =
    "zh_CN.GB18030"; // Linux下的locale名是"zh_CN.GB18030"
#endif

VString::VString(const std::string &str) : data(str) {}
VString::VString(const char *str, size_t count) : data(str, count) {}
VString::~VString() {}

VString::operator std::string() const { return data; }

// begin 方法

VString::iterator VString::begin() { return iterator(data.begin()); }

// end 方法

VString::iterator VString::end() { return iterator(data.end()); }

// 重载常用方法

size_t VString::size() const { return data.size(); }

VString &VString::operator=(const VString &other) {
  if (this != &other) {
    data = other.data;
  }
  return *this;
}

VString &VString::operator=(const std::string &str) {
  data = str;
  return *this;
}

VString VString::operator+(const VString &other) const {
  return data + other.data;
}

VString &VString::operator+=(const VString &other) {
  data += other.data;
  return *this;
}

// Comparison operators

bool VString::operator==(const VString &other) const {
  return data == other.data;
}

bool VString::operator!=(const VString &other) const {
  return !(*this == other);
}

// Overload [] operator to access characters by index

char &VString::operator[](size_t index) { return data[index]; }

// Substring method

// Overload [] operator for const objects

const char &VString::operator[](size_t index) const { return data[index]; }

VString VString::substr(size_t pos, size_t count) const {
  return data.substr(pos, count);
}

// Check if string is empty

bool VString::empty() const { return data.empty(); }

size_t VString::find(const VString &str, size_t pos) const {
  return data.find(str.data, pos);
}

bool VString::contains(const VString &str) const {
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

std::vector<VString> VString::split(const VString &delimiter) const {
  std::vector<VString> tokens;
  std::istringstream iss(data);
  VString token;
  std::string tempData = data; // 创建数据的临时副本
  size_t pos = 0;
  while ((pos = tempData.find(delimiter.data, pos)) != std::string::npos) {
    token = tempData.substr(0, pos);
    tokens.push_back(token);
    tempData.erase(0, pos + delimiter.size());
    pos = 0; // 为下一次搜索重置位置
  }
  tokens.push_back(tempData); // 在最后一个分隔符后添加剩余的子字符串

  return tokens;
}

// replace 方法

void VString::replace(size_t pos, size_t count, const std::string &str) {
  data.replace(pos, count, str);
}

const char *VString::c_str() { return data.c_str(); }

std::string VString::replaceAll(const std::string &search,
                                const std::string &replace) {
  std::string result = data;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
}

unsigned char ToHex(unsigned char x) { return x > 9 ? x + 55 : x + 48; }

unsigned char FromHex(unsigned char x) {
  unsigned char y;
  if (x >= 'A' && x <= 'Z')
    y = x - 'A' + 10;
  else if (x >= 'a' && x <= 'z')
    y = x - 'a' + 10;
  else if (x >= '0' && x <= '9')
    y = x - '0';
  else
    assert(0);
  return y;
}

std::string VString::urlEncode(const std::string &str) {
  std::string strTemp = "";
  size_t length = str.length();
  for (size_t i = 0; i < length; i++) {
    if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') ||
        (str[i] == '.') || (str[i] == '~'))
      strTemp += str[i];
    else if (str[i] == ' ')
      strTemp += "+";
    else {
      strTemp += '%';
      strTemp += ToHex((unsigned char)str[i] >> 4);
      strTemp += ToHex((unsigned char)str[i] % 16);
    }
  }
  return strTemp;
}

std::string VString::urlDecode(const std::string &str) {
  std::string strTemp = "";
  size_t length = str.length();
  for (size_t i = 0; i < length; i++) {
    if (str[i] == '+')
      strTemp += ' ';
    else if (str[i] == '%') {
      assert(i + 2 < length);
      unsigned char high = FromHex((unsigned char)str[++i]);
      unsigned char low = FromHex((unsigned char)str[++i]);
      strTemp += high * 16 + low;
    } else
      strTemp += str[i];
  }
  return strTemp;
}

    // Convert UTF-8 to ASCII
std::string VString::utf8ToAscll() const {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  std::u32string utf32String = converter.from_bytes(data);
  return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(
      utf32String);
}

// Convert UTF-8 to UTF-16
std::u16string VString::utf8ToUtf16() const {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  std::string utf8String = reinterpret_cast<const char *>(data.c_str());
  return converter.from_bytes(utf8String);
}

// Convert UTF-8 to UTF-32
std::u32string VString::utf8ToUtf32() const {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  return converter.from_bytes(data);
}

std::string VString::gbkToUtf8() const {
  std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(
      new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
  std::wstring tmp_wstr = convert.from_bytes(data);
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cv2;
  return cv2.to_bytes(tmp_wstr);
}
std::string VString::utf8ToGbk() const {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  std::wstring tmp_wstr = conv.from_bytes(data);
  std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(
      new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
  return convert.to_bytes(tmp_wstr);
}
std::wstring VString::utf8ToUnicode() const {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.from_bytes(data);
}
std::wstring VString::gbkToUnicode() const {
  std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>>
      convert(new std::codecvt_byname<wchar_t, char, std::mbstate_t>(
          GBK_LOCALE_NAME));
  return convert.from_bytes(data);
}

VString::Encoding VString::detectEncoding() {
	if (isUtf8String()) {
		return VString::Encoding::UTF8;
	}
	else if (isGbkString()) {
		return VString::Encoding::GBK;
	}
	else if (isAscllString()) {
		return VString::Encoding::ASCII;
	}
	else {
		return VString::Encoding::UNKNOWN;
	}
}

bool VString::isUtf8String() {
  int i = 0;
  while (i < data.size()) {
    unsigned char ch = data[i];
    if (ch >= 0 && ch <= 127) { // 0x00 - 0x7F
      i++;
      continue;
    }
    if ((ch & 0xE0) == 0xC0) { // 0xC0 - 0xDF
      if (i + 1 >= data.size() || (data[i + 1] & 0xC0) != 0x80) {
        return false;
      }
      i += 2;
    } else if ((ch & 0xF0) == 0xE0) { // 0xE0 - 0xEF
      if (i + 2 >= data.size() || (data[i + 1] & 0xC0) != 0x80 ||
          (data[i + 2] & 0xC0) != 0x80) {
        return false;
      }
      i += 3;
    } else {
      return false;
    }
  }
  return true;
}

bool VString::isGbkString() {
  for (unsigned char c : data) {
    if (c >= 0x81 && c <= 0xFE) {
      return true;
    }
  }
  return false;
}

bool VString::isAscllString() {
  for (unsigned char c : data) {
    if (c >= 0x80) {
      return false;
    }
  }
  return true;
}

std::string UnicodeToUTF8(const std::wstring &wstr) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.to_bytes(wstr);
}

std::wstring UTF8ToUnicode(const std::string &str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.from_bytes(str);
}

// 将 Unicode 编码的字符串转换为 ANSI 编码的字符串
std::string UnicodeToANSI(const std::wstring &wstr) {
  // 使用系统默认的 ANSI 代码页进行转换
  std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
  return converter.to_bytes(wstr);
}

// 将 ANSI 编码的字符串转换为 Unicode 编码的字符串
std::wstring ANSIToUnicode(const std::string &str) {
  // 使用系统默认的 ANSI 代码页进行转换
  std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
  return converter.from_bytes(str);
}

// 将 UTF-8 编码的字符串转换为 ANSI 编码的字符串
std::string UTF8ToANSI(const std::string &str) {
  return UnicodeToANSI(UTF8ToUnicode(str));
}

// 将 ANSI 编码的字符串转换为 UTF-8 编码的字符串
std::string ANSIToUTF8(const std::string &str) {
  return UnicodeToUTF8(ANSIToUnicode(str));
}

std::string GBKToUTF8(const std::string &str) {
  std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(
      new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
  std::wstring tmp_wstr = convert.from_bytes(str);
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cv2;
  return cv2.to_bytes(tmp_wstr);
}

std::string UTF8ToGBK(const std::string &str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  std::wstring tmp_wstr = conv.from_bytes(str);
  std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(
      new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
  return convert.to_bytes(tmp_wstr);
}

std::string replaceAll(const std::string &str, const std::string &search,
                       const std::string &replace) {
  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
}

VString::Encoding DetectEncoding(const std::string &str) {
	if (IsUtf8String(str)) {
		return VString::Encoding::UTF8;
	}
	else if (IsGbkString(str)) {
		return VString::Encoding::GBK;
	}
	else if (IsAscllString(str)) {
		return VString::Encoding::ASCII;
	}
	else {
		return VString::Encoding::UNKNOWN;
	}
}

bool IsUtf8String(const std::string &str) {
  int i = 0;
  while (i < str.size()) {
    unsigned char ch = str[i];
    if (ch >= 0 && ch <= 127) { // 0x00 - 0x7F
      i++;
      continue;
    }
    if ((ch & 0xE0) == 0xC0) { // 0xC0 - 0xDF
      if (i + 1 >= str.size() || (str[i + 1] & 0xC0) != 0x80) {
        return false;
      }
      i += 2;
    } else if ((ch & 0xF0) == 0xE0) { // 0xE0 - 0xEF
      if (i + 2 >= str.size() || (str[i + 1] & 0xC0) != 0x80 ||
          (str[i + 2] & 0xC0) != 0x80) {
        return false;
      }
      i += 3;
    } else {
      return false;
    }
  }
  return true;
}

bool IsGbkString(const std::string &str) {
  for (unsigned char c : str) {
    if (c >= 0x81 && c <= 0xFE) {
      return true;
    } else if (c >= 0x80) {
      continue;
    }
  }
  return false;
}

bool IsAscllString(const std::string &str) {
  for (unsigned char c : str) {
    if (c >= 0x80) {
      return false;
    }
  }
  return true;
}

VString::iterator::iterator(std::string::iterator iter) : it(iter) {}

char &VString::iterator::operator*() { return *it; }

VString::iterator &VString::iterator::operator++() {
  ++it;
  return *this;
}

bool VString::iterator::operator!=(const iterator &other) const {
  return it != other.it;
}
