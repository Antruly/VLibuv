#include <string>
#include <locale>
#include <codecvt>
#include <cstring> // For std::memset
#include <memory> // For std::unique_ptr

#ifdef _WIN32
const char* GBK_LOCALE_NAME = ".936";  // GBK在Windows下的locale name
#else
const char* GBK_LOCALE_NAME = "zh_CN.GB18030";  // Linux下的locale名是"zh_CN.GB18030"
#endif

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

std::wstring ANSIToUnicode(const std::string& str) {
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

std::string UTF8ToANSI(const std::string& str) {
	return UnicodeToANSI(UTF8ToUnicode(str));
}

std::string ANSIToUTF8(const std::string& str) {
	return UnicodeToUTF8(ANSIToUnicode(str));
}

std::string GBKToUTF8(const std::string& gbkData) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	std::wstring wString = ANSIToUnicode(gbkData);  // string => wstring
	return convert.to_bytes(wString);  // wstring => utf-8
}
