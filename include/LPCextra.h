
#ifndef LPCextra_h
#define LPCextra_h

#include <string.h>
#include <stdlib.h>
#include "LPCMessages.h"

namespace LPC {
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
}

#endif

