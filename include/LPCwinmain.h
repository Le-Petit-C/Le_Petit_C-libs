
#pragma once

#include "LPCdebug.h"

int LPCinitapiA();
int LPCinitapiW();

#ifndef _LIB
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\LPCwinmain.lib")
		#else
			#pragma comment(lib, "..\\Release\\LPCwinmain.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\LPCwinmain.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\LPCwinmain.lib")
		#endif
	#endif
	int WINAPI WinMain(
		_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPSTR lpCmdLine,
		_In_ int nShowCmd
	) {
#ifndef LPCWINMAINW
		return LPCinitapiA();
#else
		return LPCinitapiW();
#endif
	}
#endif

