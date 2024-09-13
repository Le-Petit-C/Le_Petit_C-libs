
#include <LPCgraphics_autoinit.h>
#include <LPCwndex.h>
#include <LPCwinmain.h>

using namespace LPC;

LRESULT CALLBACK myproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_MOUSEMOVE:
		Circle(LOWORD(lp), HIWORD(lp), 16, 63, 63, 255);
		ref();
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int main(int argc, char** argv) {
	window wnd("Test window");
	setDefault def(wnd);
	Ellipse(SCRW / 3, SCRH / 3, SCRW * 2 / 3, SCRH * 2 / 3, 255, 63, 63);
	Circle(SCRW / 2, SCRH / 2, 100, 63, 63, 255);
	Rectangle(0, 0, SCRW / 2, SCRH / 2, 255, 0, 0);
	ref();
	dprintf("≤‚ ‘\n");
	wnd.setProc(myproc);
	dprintf("%llu", (unsigned long long) - 1);
	waitForDestroy();
	return 0;
}
