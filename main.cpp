
//#include "LPCgamewndtest.h"

#include <LPCwinmain.h>
#include <LPCwnd.h>

int main() {
	LPC::window wnd("Debug window");
	wnd.waitForDestroy();
	MessageBoxA(nullptr, "test", "test", 0);
	return 0;
}
