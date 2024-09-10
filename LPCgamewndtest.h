
#include <LPCgamewnd.h>

using namespace LPC;

int main() {
	Clear(0);
	Circle(100, 100, 100, 255, 0, 0);
	Rectangle(200, 200, 200, 200, 0xffffff);
	Circle(400, 400, 100, 0, 255, 255);
	ref();
	waitForDestroy();
}
