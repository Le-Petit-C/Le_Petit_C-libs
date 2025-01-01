
/*
	����ļ�����һЩ�����Ĺ���
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
	//�����ַ������Զ�ת������
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

	// �� std::string ת���� std::wstring
	std::wstring stringToWString(const std::string &str);

	// �� std::wstring ת���� std::string
	std::string wstringToString(const std::wstring &wstr);

	/*
	*	���û�������һ���ļ�·�������û�ѡ��һ���ļ�·��
	*	Filter:�ļ�����������ʽΪ��"˵��\0������ʽ\0˵��\0������ʽ\0"���ɼ�����չ
	*	title:ѡ��·���Ĵ��ڵı���
	*	Flags:��ʶ�������Եı�ʶ������ϸ���ݼ� commdlg.h Line 277
	*		����1: OFN_READONLY
	*		����2: https://learn.microsoft.com/zh-cn/windows/win32/api/commdlg/ns-commdlg-openfilenamew
	*/
	std::string inputFilePath(const char *Filter, const char *title, DWORD Flags);
}

#endif

