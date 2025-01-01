
#include "pch.h"
#include <vector>
#include <windows.h>

// 从 std::string 转换到 std::wstring
std::wstring LPC::stringToWString(const std::string &str) {
	size_t len;
	mbstowcs_s(&len, nullptr, 0, str.c_str(), 0);
	std::vector<wchar_t> wstr(len + 1, L'\0');
	mbstowcs_s(&len, wstr.data(), len + 1, str.c_str(), len);
	return std::wstring(wstr.data());
}

// 从 std::wstring 转换到 std::string
std::string LPC::wstringToString(const std::wstring &wstr) {
	size_t len;
	wcstombs_s(&len, nullptr, 0, wstr.c_str(), 0);
	std::vector<char> str(len + 1, '\0');
	wcstombs_s(&len, str.data(), len + 1, wstr.c_str(), len);
	return std::string(str.data());
}

std::string LPC::inputFilePath(const char* Filter, const char* title, DWORD flags) {
	OPENFILENAMEA ofn;       // 用于存储文件对话框的配置和结果
	char szFile[MAX_PATH + 1]{};       // 存储选中文件的路径

	// 初始化OPENFILENAME结构体
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);			// 结构体大小
	//ofn.hwndOwner = NULL;					// 父窗口句柄，可以为NULL
	ofn.lpstrFile = szFile;					// 存放文件路径的缓冲区
	ofn.nMaxFile = sizeof(szFile);			// 缓冲区的大小
	ofn.lpstrFilter = Filter;				// 文件过滤器
	ofn.nFilterIndex = 1;					// 默认过滤器索引
	//ofn.lpstrFileTitle = NULL;			// 文件名，如果你只需要文件路径，设为NULL
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = NULL;			// 初始目录
	ofn.lpstrTitle = title;		// 对话框标题
	ofn.Flags = flags; // 文件必须存在
	// 打开文件选择对话框
	if (GetOpenFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;
	else return "";
}