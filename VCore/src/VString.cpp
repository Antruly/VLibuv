#include "VString.h"
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <codecvt>
#include <cstring> // For std::memset
#include <iomanip>
#include <locale>
#include <memory> // For std::unique_ptr
#include <string>
#include <sstream>

//#ifdef _WIN32
//const char *GBK_LOCALE_NAME = ".936"; // GBK在Windows下的locale name
//#else
//const char *GBK_LOCALE_NAME =
//    "zh_CN.GB18030"; // Linux下的locale名是"zh_CN.GB18030"
//#endif
#ifdef _WIN32
#define GBK_LOCALE_NAME "CHS"
#else
#define GBK_LOCALE_NAME "zh_CN.GBK"
#endif
static VString StrEncoding("你好VLibuv!");
static VString::Encoding encoding = StrEncoding.detectEncoding();

VString::VString() : vdata() {}
VString::VString(const std::string &str) : vdata(str) {}
VString::VString(const VString &str) : vdata(str.vdata) {}
VString::VString(const char ch) : vdata() {
  char chs[2]{ch, '\0'};
  vdata = chs;
}
VString::VString(const char *str) : vdata(str) {}
VString::VString(const char *str, size_t count) : vdata(str, count) {}
VString::~VString() {}

VString::operator std::string() const { return vdata; }

const std::string &VString::stdString() const { return vdata; }

void VString::push_back(char _Ch) { vdata.push_back(_Ch); }

void VString::pop_back() { vdata.pop_back(); }

char &VString::front() { return vdata.front(); }

const char &VString::front() const { return vdata.front(); }

char &VString::back() { return vdata.back(); }

const char &VString::back() const { return vdata.back(); }

const char *VString::c_str() const { return vdata.c_str(); }

const char *VString::data() const { return vdata.data(); }

size_t VString::length() const { return vdata.length(); }

// begin 方法

VString::iterator VString::begin() { return iterator(vdata.begin()); }

// end 方法

VString::iterator VString::end() { return iterator(vdata.end()); }

// 私有辅助方法
size_t VString::utf8CharLength(unsigned char c) const {
  if (c < 0x80)
    return 1;
  if ((c & 0xE0) == 0xC0)
    return 2;
  if ((c & 0xF0) == 0xE0)
    return 3;
  if ((c & 0xF8) == 0xF0)
    return 4;
  return 0;
}

size_t VString::gbkCharLength(unsigned char c) const {
  return (c >= 0x81 && c <= 0xFE) ? 2 : 1;
}

// 重载常用方法

size_t VString::size() const { return vdata.size(); }

size_t VString::max_size() const { return vdata.max_size(); }

void VString::resize(size_t _Newsize) { vdata.resize(_Newsize); }
void VString::resize(size_t _Newsize, char _Ch) { vdata.resize(_Newsize, _Ch); }

size_t VString::capacity() const { return vdata.capacity(); }


void VString::reserve(size_t _Newcap) {}

// 字符串操作
char &VString::operator[](size_t index) { return vdata[index]; }

const char &VString::operator[](size_t index) const { return vdata[index]; }

VString &VString::operator=(const VString &other) {
  if (this != &other) {
    vdata = other.vdata;
  }
  return *this;
}

VString VString::operator+(const VString &other) const {
  return vdata + other.vdata;
}

VString &VString::operator+=(const VString &other) {
  vdata += other.vdata;
  return *this;
}

bool VString::operator==(const VString &other) const {
  return vdata == other.vdata;
}

bool VString::operator!=(const VString &other) const {
  return !(*this == other);
}

bool VString::operator<(const VString &other) const {
  return vdata < other.vdata;
}

bool VString::operator>(const VString &other) const {
  return vdata > other.vdata;
}

bool VString::operator>=(const VString &other) const {
  return vdata >= other.vdata;
}
bool VString::operator<=(const VString &other) const {
  return vdata <= other.vdata;
}
VString VString::substr(size_t pos, size_t count) const {
  return vdata.substr(pos, count);
}

// Check if string is empty

bool VString::empty() const { return vdata.empty(); }

size_t VString::find(const VString &str, size_t pos) const {
  return vdata.find(str.vdata, pos);
}

size_t VString::find(const char ch, size_t pos) const {
  return vdata.find(ch, pos);
}

size_t VString::rfind(const VString &str, size_t pos) const {
  return vdata.rfind(str.vdata, pos);
}

size_t VString::rfind(char ch, size_t pos) const {
  return vdata.rfind(ch, pos);
}

void VString::trim() {
  if (vdata.empty())
    return;

  // 查找第一个非空白字符
  size_t start = 0;
  while (start < vdata.size() &&
         std::isspace(static_cast<unsigned char>(vdata[start]))) {
    ++start;
  }

  // 查找最后一个非空白字符
  size_t end = vdata.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(vdata[end - 1]))) {
    --end;
  }

  vdata = vdata.substr(start, end - start);
}

void VString::trimUtf8() {
  if (!isUtf8String())
    return; // 使用已有编码检测方法

  // UTF-8空白字符定义（包含全角空格）
  const std::vector<std::string> UTF8_WHITESPACE = {
      "\x20",        // Space
      "\x09",        // Tab
      "\x0A",        // LF
      "\x0D",        // CR
      "\xC2\xA0",    // No-break space
      "\xE3\x80\x80" // 全角空格(U+3000)
  };

  size_t byteStart = 0;
  size_t totalLen = vdata.size();

  // Trim start
  while (byteStart < totalLen) {
    bool found = false;
    for (const auto &ws : UTF8_WHITESPACE) {
      if (vdata.substr(byteStart, ws.size()) == ws) {
        byteStart += ws.size();
        found = true;
        break;
      }
    }
    if (!found)
      break;
  }

  // Trim end
  size_t byteEnd = totalLen;
  while (byteEnd > byteStart) {
    bool found = false;
    for (const auto &ws : UTF8_WHITESPACE) {
      size_t wsSize = ws.size();
      if (byteEnd >= wsSize && vdata.substr(byteEnd - wsSize, wsSize) == ws) {
        byteEnd -= wsSize;
        found = true;
        break;
      }
    }
    if (!found)
      break;
  }

  vdata = vdata.substr(byteStart, byteEnd - byteStart);
}

VString VString::trimmedUtf8() const {
  VString result(*this);
  result.trimUtf8();
  return result;
}
void VString::trimGbk() {
  if (!isGbkString())
    return; // 使用已有编码检测方法

  // GBK空白字符定义
  const std::vector<std::string> GBK_WHITESPACE = {
      "\x20",     // 空格
      "\xA1\xA1", // 全角空格(GBK编码)
      "\xA3\xA0", // 中文制表符
  };

  size_t byteStart = 0;
  size_t totalLen = vdata.size();

  // Trim start
  while (byteStart < totalLen) {
    bool found = false;
    for (const auto &ws : GBK_WHITESPACE) {
      size_t wsSize = ws.size();
      if (byteStart + wsSize <= totalLen &&
          vdata.substr(byteStart, wsSize) == ws) {
        byteStart += wsSize;
        found = true;
        break;
      }
    }
    if (!found)
      break;
  }

  // Trim end
  size_t byteEnd = totalLen;
  while (byteEnd > byteStart) {
    bool found = false;
    for (const auto &ws : GBK_WHITESPACE) {
      size_t wsSize = ws.size();
      if (byteEnd >= wsSize && vdata.substr(byteEnd - wsSize, wsSize) == ws) {
        byteEnd -= wsSize;
        found = true;
        break;
      }
    }
    if (!found)
      break;
  }

  vdata = vdata.substr(byteStart, byteEnd - byteStart);
}

VString VString::trimmedGbk() const {
  VString result(*this);
  result.trimGbk();
  return result;
}

void VString::trimAuto() {
  switch (detectEncoding()) {
  case Encoding::UTF8:
    trimUtf8();
    break;
  case Encoding::GBK:
    trimGbk();
    break;
  case Encoding::ASCII:   // 明确包含ASCII类型
  case Encoding::UNKNOWN: // 未知编码按基础处理
  default:
    trim();
  }
}

VString VString::trimmedAuto() const {
  VString result(*this);
  result.trimAuto();
  return result;
}

// 支持自定义trim字符集的版本

VString &VString::trimChars(const VString &chars) {
  size_t start = 0;
  while (start < length() && chars.contains(vdata[start])) {
    ++start;
  }

  size_t end = length();
  while (end > start && chars.contains(vdata[end - 1])) {
    --end;
  }

  return *this = substr(start, end - start);
}


// 返回新对象的trim版本

VString VString::trimmed() const {
  VString result(*this);
  result.trim();
  return result;
}

bool VString::contains(const VString &str) const {
  return find(str) != std::string::npos;
}

bool VString::startsWith(const VString &prefix) const {
  if (prefix.size() > vdata.size())
    return false;
  return std::equal(prefix.vdata.begin(), prefix.vdata.end(), vdata.begin());
}

bool VString::endsWith(const VString &suffix) const {
  if (suffix.size() > vdata.size())
    return false;
  return std::equal(suffix.vdata.rbegin(), suffix.vdata.rend(), vdata.rbegin());
}

std::vector<VString> VString::split(char delimiter) const {
  std::vector<VString> tokens;
  std::string token;
  std::istringstream iss(vdata);

  while (std::getline(iss, token, delimiter)) {
    tokens.emplace_back(token); // 使用VString的隐式构造函数
  }
  return tokens;
}

std::vector<VString> VString::split(const VString &delimiter) const {
  std::vector<VString> tokens;
  const std::string &delim = delimiter; // 假设有str()方法
  size_t start = 0;
  size_t end = vdata.find(delim);

  while (end != std::string::npos) {
    tokens.emplace_back(vdata.substr(start, end - start));
    start = end + delim.length();
    end = vdata.find(delim, start);
  }
  tokens.emplace_back(vdata.substr(start));
  return tokens;
}

// replace 方法

void VString::replace(size_t pos, size_t count, const VString &str) {
  vdata.replace(pos, count, str);
}

VString VString::replaceAll(const VString &search, const VString &replace) {
  std::string result = vdata;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
}

VString& VString::urlEncode() {
  std::string strTemp = "";
  size_t length = vdata.length();
  for (size_t i = 0; i < length; i++) {
    if (isalnum((unsigned char)vdata[i]) || (vdata[i] == '-') ||
        (vdata[i] == '_') || (vdata[i] == '.') || (vdata[i] == '~'))
      strTemp += vdata[i];
    else if (vdata[i] == ' ')
      strTemp += "+";
    else {
      strTemp += '%';
      strTemp += ToHex((unsigned char)vdata[i] >> 4);
      strTemp += ToHex((unsigned char)vdata[i] % 16);
    }
  }
  vdata = strTemp;
  return *this;
}

VString& VString::urlDecode() {
  std::string strTemp = "";
  size_t length = vdata.length();
  for (size_t i = 0; i < length; i++) {
    if (vdata[i] == '+')
      strTemp += ' ';
    else if (vdata[i] == '%') {
      assert(i + 2 < length);
      unsigned char high = FromHex((unsigned char)vdata[++i]);
      unsigned char low = FromHex((unsigned char)vdata[++i]);
      strTemp += high * 16 + low;
    } else
      strTemp += vdata[i];
  }
  vdata = strTemp;
  return *this;
}

VString VString::UrlEncode(const VString &str) {
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

VString VString::UrlDecode(const VString &str) {
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


// 编码检测方法
bool VString::isUtf8String() const {
  size_t i = 0;
  while (i < vdata.size()) {
    unsigned char ch = vdata[i];
    if (ch <= 0x7F) {
      ++i;
    } else if ((ch & 0xE0) == 0xC0) {
      if (i + 1 >= vdata.size() || (vdata[i + 1] & 0xC0) != 0x80 || ch < 0xC2)
        return false;
      i += 2;
    } else if ((ch & 0xF0) == 0xE0) {
      if (i + 2 >= vdata.size() || (vdata[i + 1] & 0xC0) != 0x80 ||
          (vdata[i + 2] & 0xC0) != 0x80)
        return false;
      unsigned char c1 = vdata[i + 1];
      if ((ch == 0xE0 && c1 < 0xA0) || (ch == 0xED && c1 > 0x9F))
        return false;
      i += 3;
    } else if ((ch & 0xF8) == 0xF0) {
      if (i + 3 >= vdata.size() || (vdata[i + 1] & 0xC0) != 0x80 ||
          (vdata[i + 2] & 0xC0) != 0x80 || (vdata[i + 3] & 0xC0) != 0x80 ||
          ch > 0xF4)
        return false;
      unsigned char c1 = vdata[i + 1];
      if ((ch == 0xF0 && c1 < 0x90) || (ch == 0xF4 && c1 > 0x8F))
        return false;
      i += 4;
    } else {
      return false;
    }
  }
  return true;
}

bool VString::isGbkString() const {
  size_t i = 0;
  while (i < vdata.size()) {
    unsigned char c = vdata[i];
    if (c >= 0x81 && c <= 0xFE) {
      if (i + 1 >= vdata.size())
        return false;
      unsigned char c2 = vdata[i + 1];
      if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0x80 && c2 <= 0xFE)) {
        i += 2;
      } else {
        return false;
      }
    } else if (c <= 0x7F) {
      ++i;
    } else {
      return false;
    }
  }
  return true;
}

bool VString::isAsciiString() const {
  for (unsigned char c : vdata) {
    if (c > 0x7F) {
      return false;
    }
  }
  return true;
}

// 编码转换方法
VString VString::utf8ToAscii() const {
  try {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string utf32 = converter.from_bytes(vdata);

    std::string result;
    result.reserve(vdata.size());
    for (auto c : utf32) {
      result += (c <= 0x7F) ? static_cast<char>(c) : '?';
    }
    return result;
  } catch (const std::exception &) {
    return VString();
  }
}

std::u16string VString::utf8ToUtf16() const {
  try {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.from_bytes(vdata);
  } catch (const std::exception &) {
    return {};
  }
}

std::u32string VString::utf8ToUtf32() const {
  try {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(vdata);
  } catch (const std::exception &) {
    return {};
  }
}

VString VString::gbkToUtf8() const {
  try {
    if (isUtf8String())
      return vdata;

    static std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>>
        gbk_converter(
            new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));

    std::wstring wstr = gbk_converter.from_bytes(vdata);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_converter;
    return utf8_converter.to_bytes(wstr);
  } catch (const std::exception &) {
    return VString();
  }
}

VString VString::utf8ToGbk() const {
  try {
    if (!isUtf8String())
      return vdata;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_converter;
    std::wstring wstr = utf8_converter.from_bytes(vdata);

    static std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>>
        gbk_converter(
            new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));

    return gbk_converter.to_bytes(wstr);
  } catch (const std::exception &) {
    return VString();
  }
}


std::wstring VString::utf8ToUnicode() const {
  try {
// 使用UTF-8到UTF-16的转换器（Windows平台wchar_t为2字节）
#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>>
        converter; // Linux/Mac使用4字节wchar_t
#endif
    return converter.from_bytes(vdata);
  } catch (const std::exception &e) {
    return L""; // 返回空字符串或抛出异常
  }
}

std::wstring VString::gbkToUnicode() const {
  try {
    // 使用GBK到宽字符的转换器
    static std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>>
        converter(
            new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));

    return converter.from_bytes(vdata);
  } catch (const std::exception &e) {
    return L"";
  }
}

VString::Encoding VString::detectEncoding() const {
  if (isUtf8String())
    return VString::Encoding::UTF8;
  if (isGbkString())
    return VString::Encoding::GBK;
  if (isAsciiString())
    return VString::Encoding::ASCII;
  return VString::Encoding::UNKNOWN;
}

VString VString::toString(VString::Encoding encoding) const {
  switch (encoding) {
  case VString::Encoding::ASCII: {
    switch (this->detectEncoding()) {
    case VString::Encoding::ASCII:
      return vdata;
    case VString::Encoding::UTF8:
      return this->utf8ToAscii();
    case VString::Encoding::GBK:
      return vdata;
    case VString::Encoding::UNKNOWN:
      return vdata;
    default:
      return vdata;
    }
  } break;
  case VString::Encoding::UTF8: {
    switch (this->detectEncoding()) {
    case VString::Encoding::ASCII:
      return ANSIToUTF8(vdata);
    case VString::Encoding::UTF8:
      return vdata;
    case VString::Encoding::GBK:
      return this->gbkToUtf8();
    case VString::Encoding::UNKNOWN:
      return vdata;
    default:
      return vdata;
    }
  } break;
  case VString::Encoding::GBK: {
    switch (this->detectEncoding()) {
    case VString::Encoding::ASCII:
      return vdata;
    case VString::Encoding::UTF8:
      return this->utf8ToGbk();
    case VString::Encoding::GBK:
      return vdata;
    case VString::Encoding::UNKNOWN:
      return vdata;
    default:
      return vdata;
    }
  } break;
  case VString::Encoding::UNKNOWN:
    return vdata;
  default:
    return vdata;
  }
}

VString VString::toUtf8String() const {
  return this->toString(VString::Encoding::UTF8);
}

VString VString::toGbkString() const {
  return this->toString(VString::Encoding::GBK);
}

VString VString::toAsciiString() const {
  return this->toString(VString::Encoding::ASCII);
}

VString VString::toSystemString() const {
  return this->toString(SystemEncoding());
}

VString VString::toUpper() const {
  std::string strRet = vdata;
  transform(strRet.begin(), strRet.end(), strRet.begin(),
            (int (*)(int))toupper);
  return strRet;
}

VString VString::toLower() const {
  std::string strRet = vdata;
  transform(strRet.begin(), strRet.end(), strRet.begin(),
            (int (*)(int))tolower);
  return strRet;
}

VString VString::Format(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_list args_copy;
  va_copy(args_copy, args);

  // 计算所需缓冲区大小（不包括终止符）
  int size = vsnprintf(nullptr, 0, fmt, args_copy);
  va_end(args_copy);

  if (size <= 0) {
    va_end(args);
    return VString();
  }

  // 分配缓冲区（size + 1 用于容纳终止符）
  std::string buffer;
  buffer.resize(size + 1);

  // 填充实际内容
  int written = vsnprintf(&buffer[0], size + 1, fmt, args);
  va_end(args);

  if (written != size) { // 确保写入长度一致
    return VString();
  }

  buffer.resize(size); // 移除终止符
  return buffer;
}

VString VString::Format(const VString &sfmt, ...) {
  const char *fmt = sfmt.c_str();
  va_list args;

  // 计算所需缓冲区大小
  va_start(args, sfmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  if (size < 0) {
    return VString(); // 返回空字符串表示失败
  }

  // 分配缓冲区并填充数据
  std::vector<char> buffer(size + 1);
  va_start(args, sfmt);
  vsnprintf(buffer.data(), buffer.size(), fmt, args);
  va_end(args);

  return VString(buffer.data());
}

// 输出运算符 <<
std::ostream &operator<<(std::ostream &os, const VString &str) {
  os << str.vdata;
  return os;
}

const std::ostream &operator<<(const std::ostream &os, const VString &str) {
  os << str.vdata;
  return os;
}

// 输入运算符 >>
std::istream &operator>>(std::istream &is, VString &str) {
  is >> str.vdata;
  return is;
}

VString::Encoding SystemEncoding() { return encoding; }

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

unsigned char ToHex(unsigned char x) { return x > 9 ? x + 55 : x + 48; }

unsigned char FromHex(unsigned char x) {
  if (x >= 'A' && x <= 'Z')
    return x - 'A' + 10;
  if (x >= 'a' && x <= 'z')
    return x - 'a' + 10;
  if (x >= '0' && x <= '9')
    return x - '0';
  throw std::invalid_argument("Invalid hex character");
}
size_t CountVarArgs(const char *fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  int count = vsnprintf(nullptr, 0, fmt, args_copy);
  va_end(args_copy);
  return (count >= 0) ? static_cast<size_t>(count) : 0;
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

VString::iterator::iterator(std::string::iterator iter) : it(iter) {}

char &VString::iterator::operator*() { return *it; }

VString::iterator &VString::iterator::operator++() {
  ++it;
  return *this;
}

bool VString::iterator::operator!=(const iterator &other) const {
  return it != other.it;
}


