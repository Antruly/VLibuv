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

size_t VString::find(const VString& str, size_t pos) const {
  return data.find(str.data, pos);
}

// replace 方法

void VString::replace(size_t pos, size_t count, const std::string& str) {
  data.replace(pos, count, str);
}

std::string VString::UnicodeToUTF8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	return convert.to_bytes(wstr);
}

std::wstring VString::UTF8ToUnicode(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	return convert.from_bytes(str);
}

std::string VString::UnicodeToANSI(const std::wstring& wstr) {
	std::string ret;
	std::mbstate_t state;
	std::memset(&state, 0, sizeof(state)); // Initialize the state
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

std::wstring VString::ANSIToUnicode(const std::string& str) {
	std::wstring ret;
	std::mbstate_t state;
	std::memset(&state, 0, sizeof(state)); // Initialize the state
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

std::string VString::UTF8ToANSI(const std::string& str) {
	return UnicodeToANSI(UTF8ToUnicode(str));
}

std::string VString::ANSIToUTF8(const std::string& str) {
	return UnicodeToUTF8(ANSIToUnicode(str));
}

std::string VString::GBKToUTF8(const std::string& gbkData) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	std::wstring wString = ANSIToUnicode(gbkData);  // string => wstring
	return convert.to_bytes(wString);  // wstring => utf-8
}

std::string VString::ReplaceAll(
                       const std::string& search,
                       const std::string& replace) {
  std::string result = data;
  size_t pos = 0;
  while ((pos = result.find(search, pos)) != std::string::npos) {
    result.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return result;
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
