
#include <iostream>
//#include <OpenGL++.h>
#include <LPCwndex.h>
#include <LPCwinmainA.h>
#include <LPCgraphics.h>

using LPC::SCRW;
using LPC::SCRH;
//LPC::window wnd("Title");
//LPC::setDefaultWindow def(wnd);

bool exited = false;

DWORD threadfunc(void*) {
	int times = 0;
	while (++times <= 10000000)
		SuspendThread(GetCurrentThread());
	exited = true;
	return 0;
}

int main(int argc, char** argv) {
	MessageBoxA(0, "Start", "TEXT", 0);
	HANDLE hthread = CreateThread(nullptr, 0, threadfunc, nullptr, 0, nullptr);
	if (hthread != nullptr) {
		while (!exited)
			ResumeThread(hthread);
		CloseHandle(hthread);
		MessageBoxA(0, "Done", "TEXT", 0);
	}
	else MessageBoxA(0, "Done", "TEXT", 0);
	//wnd.waitForDestroy();
	return 0;
}