
#include "LPCdebug.h"

namespace LPC {
	const char* lasterror;
	BOOL debugOutputAllocated;
	namespace priv {
		bool shouldExceptionOutput = true;
		struct debug_InitClass {
			debug_InitClass() {
				setlocale(LC_ALL, "en_US.UTF-8");
			}
		} debugInitInstance;
	}
}

LPC::dexcoc LPC::allocDebugOutput() {
	if (debugOutputAllocated == 0) {
#if defined _DEBUG
		if (GetConsoleWindow() != nullptr) {
			debugOutputAllocated = true;
			return false;
		}
		debugOutputAllocated = AllocConsole();
		if (debugOutputAllocated == 0) {
			__debugbreak();//Failed to allocate output console!
		}
		FILE* fp;
		if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0) {
			debugOutputAllocated = 0;
			__debugbreak();//Failed to open console output!
		}
		if (freopen_s(&fp, "CONIN$", "r", stdin) != 0) {
			__debugbreak();//Failed to open console input!
		}
#else
		FILE* fp;
		debugOutputAllocated = freopen_s(&fp, "debugoutput.txt", "w", stdout) == 0 ? 1 : 0;
#endif
	}
	return false;
}
int LPC::dvprintf(const char* format, va_list v) {
	if (allocDebugOutput()) return -1;
	return vprintf(format, v);
}
int LPC::dvprintf_s(const char* format, va_list v) {
	if (allocDebugOutput()) return -1;
	return vprintf_s(format, v);
}
int LPC::dvwprintf(const wchar_t* format, va_list v) {
	if (allocDebugOutput()) return -1;
	return vwprintf(format, v);
}
int LPC::dvwprintf_s(const wchar_t* format, va_list v) {
	if (allocDebugOutput()) return -1;
	return vwprintf_s(format, v);
}
int LPC::dprintf(const char* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = dvprintf(format, v);
	va_end(v);
	return ret;
}
int LPC::dprintf_s(const char* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = dvprintf_s(format, v);
	va_end(v);
	return ret;
}
int LPC::dwprintf(const wchar_t* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = dvwprintf(format, v);
	va_end(v);
	return ret;
}
int LPC::dwprintf_s(const wchar_t* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = dvwprintf_s(format, v);
	va_end(v);
	return ret;
}
int LPC::dvscanf_s(const char* format, va_list v) {
	if (allocDebugOutput()) return -1;
	return vscanf_s(format, v);
}
int LPC::dscanf_s(const char* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = dvscanf_s(format, v);
	va_end(v);
	return ret;
}
void LPC::LPCexceptionOutput(bool b) {
	priv::shouldExceptionOutput = b;
}

#ifdef _DEBUG

void LPC::priv::putLastError() {
	DWORD error = GetLastError();
	// 使用FormatMessage函数获取错误描述字符串
	char* errorDescription = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		error,
		GetUserDefaultUILanguage(),
		(char*)&errorDescription,
		0,
		nullptr
	);
	dprintf("code:%d\n", error);
	if (errorDescription) {
		dprintf("description:%hs", errorDescription);
		LocalFree(errorDescription);
	}
	else dprintf("Failed to get description!\n");
}

#endif

static char errorbuf[256];
int LPC::sprintfsetexception(const char* format, ...) {
	va_list v;
	va_start(v, format);
	int ret = vsprintf_s(errorbuf, sizeof(errorbuf), format, v);
	lasterror = errorbuf;
	va_end(v);
	if (priv::shouldExceptionOutput)
		dprintf("%s", lasterror);
	return ret;
}
void LPC::setexception(const char* str) noexcept {
	lasterror = str;
	if (priv::shouldExceptionOutput)
		dprintf("%s", lasterror);
}