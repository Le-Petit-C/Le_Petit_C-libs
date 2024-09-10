
#include <iostream>
//#include <OpenGL++.h>
#include <LPCwndex.h>
#include <LPCwinmainA.h>
#include <LPCgraphics.h>

bool exited = false;

DWORD threadfunc(void*) {
	int times = 0;
	MSG msg;
	while (++times <= 1000000)
		GetMessage(&msg, 0, 0, 0);
	exited = true;
	return 0;
}

int main(int argc, char** argv) {
	DWORD threadid;
	HANDLE hthread;
	MessageBoxA(0, "Start", "TEXT", 0);
	hthread = CreateThread(nullptr, 0, threadfunc, nullptr, 0, &threadid);
	if (hthread != nullptr) {
		CloseHandle(hthread);
		while (!exited)
			PostThreadMessage(threadid, WM_USER, 0, 0);
		MessageBoxA(0, "Done", "TEXT", 0);
	}
	else MessageBoxA(0, "Done", "TEXT", 0);
	return 0;
}