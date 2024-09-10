
#include <LPCgraphics.h>
#include <LPCwinmain.h>
#include <conio.h>
//#include <LPCextra.h>

using namespace LPC;

window wnd("Test window");

void msgloop() {
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int main() {
	wndbuffer buf(wnd);
	Gdiplus::Rect rect(0, 0, 200, 200);
	POINT rects[3] = { {0, 0}, {0, 100}, {400, 0} };
	buf.Rectangle(rect, 127, 0, 0, 255);
	buf.Rectangle(100, 100, 200, 200, 255, 255, 0);
	buf.SetColor(0xffff00ff);
	buf.Triangle(rects);
	buf.ref();
	wnd.waitForDestroy();
	return 0;
}
