
#ifndef LPCstr_h
#define LPCstr_h

#include <string>
#include <vector>
#include <stdlib.h>

namespace LPC {
	// �� std::string ת���� std::wstring
	std::wstring stringToWString(const std::string& str);

	// �� std::wstring ת���� std::string
	std::string wstringToString(const std::wstring& wstr);
}

#endif
