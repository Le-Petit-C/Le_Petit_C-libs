
#ifndef LPCdebug_h
#define LPCdebug_h

#pragma execution_character_set("en_US.UTF-8")
//我也不知道为什么这里会报一个warning，但事实就是把这里按他说的改了之后就会输出乱码

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include <functional>

//debug模式下输出到控制台
//非debug模式下输出到debugoutput.txt

namespace LPC {
	//函数返回值类型，do exception occurd的缩写，为true时表示出现了异常，LPC::lasterror是最后一个错误的描述字符串，可printf("%s",LPC::lasterror)查看
	using dexcoc = bool;
	//最后一个错误的描述字符串
	extern const char* lasterror;
	extern BOOL debugOutputAllocated;
	dexcoc allocDebugOutput();
	int dvprintf(const char* format, va_list v);
	int dvprintf_s(const char* format, va_list v);
	int dvwprintf(const wchar_t* format, va_list v);
	int dvwprintf_s(const wchar_t* format, va_list v);
	int dprintf(const char* format, ...);
	int dprintf_s(const char* format, ...);
	int dwprintf(const wchar_t* format, ...);
	int dwprintf_s(const wchar_t* format, ...);
	int dvscanf_s(const char* format, va_list v);
	int dscanf_s(const char* format, ...);
	int sprintfsetexception(const char* format, ...);
	void setexception(const char* str) noexcept;
	namespace priv {
		extern bool shouldExceptionOutput;
	}
	void LPCexceptionOutput(bool b);
}

#ifdef _DEBUG
namespace LPC {
	namespace priv {
		void putLastError();
	}
	template<typename T1, typename T2, typename compare_t>
	T1 debug_assert_error_func(T1 return_value, T2 compare_value, compare_t compare_func, const char* file, int line, const char* function, const char* assert_expression, const char* formatstr, ...);
	template<typename T1, typename T2, typename compare_t>
	T1 debug_assert_warning_func(T1 return_value, T2 compare_value, compare_t compare_func, const char* file, int line, const char* function, const char* assert_expression, const char* formatstr...);
}
template<typename T1, typename T2, typename compare_t>
T1 LPC::debug_assert_error_func(T1 return_value, T2 compare_value, compare_t compare_func, const char* file, int line, const char* function, const char* assert_expression, const char* formatstr, ...) {
	if (!priv::shouldExceptionOutput || compare_func(return_value, compare_value))
		return return_value;
	DWORD error = GetLastError();
	va_list v;
	va_start(v, formatstr);
	sprintfsetexception(formatstr, v);
	va_end(v);
	::LPC::dprintf_s("\n[File:%s|Line:%d|In function:%s]\n%s\nError:%s\n", file, line, function, assert_expression, ::LPC::lasterror);
	priv::putLastError();
	__debugbreak();
	return return_value;
}
template<typename T1, typename T2, typename compare_t>
T1 LPC::debug_assert_warning_func(T1 return_value, T2 compare_value, compare_t compare_func, const char* file, int line, const char* function, const char* assert_expression, const char* formatstr...) {
	if (!priv::shouldExceptionOutput || compare_func(return_value, compare_value))
		return return_value;
	DWORD error = GetLastError();
	va_list v;
	va_start(v, formatstr);
	sprintfsetexception(formatstr, v);
	va_end(v);
	::LPC::dprintf_s("\n[File:%s|Line:%d|In function:%s]\n%s\nWarning:%s\n", file, line, function, assert_expression, ::LPC::lasterror);
	priv::putLastError();
	return return_value;
}

#define debug_assert_error(assert_expression, compare_value, compare_function, formatstr, ...) \
	LPC::debug_assert_error_func(assert_expression, compare_value, compare_function, __FILE__, __LINE__, __FUNCTION__ , #assert_expression, formatstr, __VA_ARGS__)
#define debug_assert_warning(assert_expression, compare_value, compare_function, formatstr, ...) \
	LPC::debug_assert_warning_func(assert_expression, compare_value, compare_function, __FILE__, __LINE__, __FUNCTION__, #assert_expression, formatstr, __VA_ARGS__)
#else
#define debug_assert_error(assert_expression, compare_value, compare_function, formatstr, ...) assert_expression
#define debug_assert_warning(assert_expression, compare_value, compare_function, formatstr, ...) assert_expression
#endif

#define dGetDC(...) debug_assert_error(::GetDC(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "GetDC Failed!")
#define dGetMessage(...) debug_assert_warning(::GetMessage(__VA_ARGS__), -1, ::std::not_equal_to<BOOL>(), "GetMessage Failed!")
#define dReleaseDC(...) debug_assert_warning(::ReleaseDC(__VA_ARGS__), 0, ::std::not_equal_to<int>(), "ReleaseDC Failed!")
#define dCreateCompatibleDC(...) debug_assert_error(::CreateCompatibleDC(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateCompatibleDC Failed!")
#define dDeleteDC(...) debug_assert_warning(::DeleteDC(__VA_ARGS__), 0, ::std::not_equal_to<BOOL>(), "DeleteDC Failed!")
#define dCreateWindow(...) debug_assert_error(::CreateWindow(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateWindow Failed!")
#define dCreateWindowA(...) debug_assert_error(::CreateWindowA(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateWindowA Failed!")
#define dCreateWindowW(...) debug_assert_error(::CreateWindowW(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateWindowW Failed!")
#define dCreateWindowExA(...) debug_assert_error(::CreateWindowExA(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateWindowExA Failed!")
#define dCreateWindowExW(...) debug_assert_error(::CreateWindowExW(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateWindowExW Failed!")
#define dSelectObject(...) debug_assert_error(::SelectObject(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "SelectObject Failed!")
#define dSetDCBrushColor(...) debug_assert_warning(::SetDCBrushColor(__VA_ARGS__), CLR_INVALID, ::std::not_equal_to<COLORREF>(), "SetDCBrushColor Failed!")
#define dEllipse(...) debug_assert_warning(::Ellipse(__VA_ARGS__), 0, ::std::not_equal_to<BOOL>(), "Ellipse Failed!")
#define dDeleteObject(hobj) debug_assert_warning(::DeleteObject(hobj), 0, ::std::not_equal_to<BOOL>(), "Delete object Failed! (HGDIOBJ:%p)", (void*)hobj)
#define dCreateCompatibleBitmap(...) debug_assert_error(::CreateCompatibleBitmap(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "CreateCompatibleBitmap Failed!")
#define dSetTimer(...) debug_assert_warning(::dSetTimer(__VA_ARGS__), 0, ::std::not_equal_to<void*>(), "SetTimer Failed!")
#define dPostThreadMessage(...) debug_assert_error(::PostThreadMessage(__VA_ARGS__), 0, ::std::not_equal_to<BOOL*>(), "PostThreadMessage Failed!")
#define dPostMessage(...) debug_assert_error(::PostMessage(__VA_ARGS__), 0, ::std::not_equal_to<BOOL*>(), "PostMessage Failed!")
#define dmalloca(...) debug_assert_error(_malloca(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "_malloca Failed!")
#define dmalloc(...) debug_assert_error(_malloc(__VA_ARGS__), nullptr, ::std::not_equal_to<void*>(), "_malloc Failed!")

#endif
