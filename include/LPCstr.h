
#ifndef LPCstr_h
#define LPCstr_h

#include <string>
#include <vector>
#include <stdlib.h>

namespace LPC {
	// 从 std::string 转换到 std::wstring
	std::wstring stringToWString(const std::string& str);

	// 从 std::wstring 转换到 std::string
	std::string wstringToString(const std::wstring& wstr);
}

#endif
