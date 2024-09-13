
#pragma once

#include "LPCdebug.h"

#ifndef _LIB
	#include <windows.h>
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\Le_Petit_C-winmain.lib")
		#else
			#pragma comment(lib, "..\\Release\\Le_Petit_C-winmain.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\Le_Petit_C-winmain.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\Le_Petit_C-winmain.lib")
		#endif
	#endif
#endif
