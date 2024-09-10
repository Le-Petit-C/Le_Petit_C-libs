
#include <LPCwndex.h>
#include <LPCwinmain.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace LPC;

window wnd("Test window");

setDefaultWindow def(wnd);

static int vcolor = 127;
static int k = 2;

void paint(HWND hwnd) {
	dprintf("%d\n", vcolor);
	HDC hdcw = GetDC(hwnd);
	HDC hdc = CreateCompatibleDC(hdcw);
	HGDIOBJ hbm = SelectObject(hdc, CreateCompatibleBitmap(hdcw, SCRW, SCRH));
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, RGB(255, 255, 255));
	Rectangle(hdc, 0, 0, SCRW / 2, SCRH);
	SetDCBrushColor(hdc, RGB(vcolor, vcolor, vcolor));
	Rectangle(hdc, SCRW / 2, 0, SCRW, SCRH);
	for (int a = 0; a < SCRH; a += k)
		for (int b = a % (k * 2); b <= SCRW / 2; b += k * 2)
			for (int c = 0; c < k; ++c)
				for (int d = 0; d < k; ++d)
					SetPixel(hdc, b + d, a + c, RGB(0, 0, 0));
	BitBlt(hdcw, 0, 0, SCRW, SCRH, hdc, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hdc, hbm));
	DeleteDC(hdc);
	ReleaseDC(hwnd, hdcw);
}

LRESULT CALLBACK myproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_UP:
			++vcolor;
			paint(hwnd);
			return 0;
		case VK_DOWN:
			--vcolor;
			paint(hwnd);
			return 0;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char** argv) {
	paint(wnd.gethwnd());
	wnd.setProc(myproc);
	waitForDestroy();
	return 0;
}
