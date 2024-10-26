
#include "pch.h"

// 从 std::string 转换到 std::wstring
std::wstring LPC::stringToWString(const std::string& str) {
	size_t len;
	mbstowcs_s(&len, nullptr, 0, str.c_str(), 0);
	std::vector<wchar_t> wstr(len + 1, L'\0');
	mbstowcs_s(&len, wstr.data(), len + 1, str.c_str(), len + 1);
	return std::wstring(wstr.data());
}

// 从 std::wstring 转换到 std::string
std::string LPC::wstringToString(const std::wstring& wstr) {
	size_t len;
	wcstombs_s(&len, nullptr, 0, wstr.c_str(), 0);
	std::vector<char> str(len + 1, '\0');
	wcstombs_s(&len, str.data(), len + 1, wstr.c_str(), len + 1);
	return std::string(str.data());
}
