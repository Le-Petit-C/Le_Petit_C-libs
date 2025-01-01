
#include <LPCwinmain.h>
#include <LPCwnd.h>
#include <conio.h>
#include <LPCgdiplusgraphics.h>
#include <LPCgraphics_autoinit.h>
#include <LPCwndex.h>

using namespace LPC;

int main() {
	PushDefaultWindowSetting setting;
	RECT rect{500, 500, 800, 700};
	setCreateClientRect(rect);
	window wnd;
	setDefaultWindow def(wnd);
	//resizeClient(300, 200);
	FillRectangle(0, 0, 100, 100, 255, 0, 0);
	ref();
	dprintf("%d, %d\n", SCRW, SCRH);
	waitForDestroy();
	return 0;
}