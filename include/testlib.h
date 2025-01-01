#pragma once


#ifndef _LIB
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\testlib.lib")
		#else
			#pragma comment(lib, "..\\Release\\testlib.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\testlib.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\testlib.lib")
		#endif
	#endif
#endif

void testfunc();
