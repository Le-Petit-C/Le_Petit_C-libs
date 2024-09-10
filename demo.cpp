
#include <LPCwndex.h>
#include <LPCwinmain.h>

using namespace LPC;

int main(int argc, char** argv) {
	window wnd("Test window");
	setDefault def(wnd);
	Ellipse(SCRW / 3, SCRH / 3, SCRW * 2 / 3, SCRH * 2 / 3, 255, 63, 63);
	Circle(SCRW / 2, SCRH / 2, 100, 63, 63, 255);
	ref();
	dprintf("≤‚ ‘");
	waitForDestroy();
	return 0;
}
