
#ifndef LPCwinmainA_h
#define LPCwinmainA_h 1

#include <windows.h>

int LPCinitapiA();

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	return LPCinitapiA();
}

#endif
