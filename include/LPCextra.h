
/*
	这个文件包含一些单独的功能
*/

#ifndef LPCextra_h
#define LPCextra_h

#include <string>
#include <stdlib.h>
#include <windows.h>
//#include "LPCMessages.h"

#ifndef _LIB
#include <windows.h>
#ifndef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\LPCextra.lib")
#else
#pragma comment(lib, "..\\Release\\LPCextra.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\LPCextra.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\LPCextra.lib")
#endif
#endif
#endif

namespace LPC {
	//复制字符串，自动转换编码
	inline errno_t copystring_s(char* dst, size_t sizeInBytes, const char* res) {
		dst[0] = 0;
		return ::strcpy_s(dst, sizeInBytes, res);
	}
	inline errno_t copystring_s(wchar_t* dst, size_t sizeInWords, const wchar_t* res) {
		dst[0] = 0;
		return ::wcscat_s(dst, sizeInWords, res);
	}
	inline errno_t copystring_s(char* dst, size_t sizeInBytes, const wchar_t* res) {
		size_t store = 0;
		dst[0] = 0;
		return ::wcstombs_s(&store, dst, sizeInBytes, res, _TRUNCATE);
	}
	inline errno_t copystring_s(wchar_t* dst, size_t sizeInWords, const char* res) {
		size_t store = 0;
		dst[0] = 0;
		return ::mbstowcs_s(&store, dst, sizeInWords, res, _TRUNCATE);
	}

	// 从 std::string 转换到 std::wstring
	std::wstring stringToWString(const std::string &str);

	// 从 std::wstring 转换到 std::string
	std::string wstringToString(const std::wstring &wstr);

	/*
	*	从用户处输入一个文件路径，让用户选择一个文件路径
	*	Filter:文件过滤器，格式为："说明\0正则表达式\0说明\0正则表达式\0"，可继续拓展
	*	title:选择路径的窗口的标题
	*	Flags:标识部分特性的标识符，详细内容见 commdlg.h Line 277
	*		导航1: OFN_READONLY
	*		导航2: https://learn.microsoft.com/zh-cn/windows/win32/api/commdlg/ns-commdlg-openfilenamew
	*/
	std::string inputFilePath(const char *Filter, const char *title, DWORD Flags);
}

#endif

