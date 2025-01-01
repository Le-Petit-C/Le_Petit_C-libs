
#include "pch.h"
#include <windowsx.h>

namespace LPC {
	namespace priv {
		const MSG *pmsg;
	}
}

LPC::windowclass* gameclass;
LPC::defaultWindow* gamewindow;

static LPC::event_t gamewndevent;
static UINT currentmessage;
static const MSG* currentpmsg;
static uint64_t extradata[1];

static void SetWheelBywParam(WPARAM wParam) {
	extradata[0] = ((short)HIWORD(wParam)) / WHEEL_DELTA;
}
int LPC::GetWheel() { return (int)extradata[0]; }
int LPC::GetWheelPixel() { return (int)extradata[0] * WHEEL_DELTA; }

static int event_dispatch(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZE:
		return gamewndevent(WD_SIZE, LOWORD(lParam), HIWORD(lParam));
	case WM_KEYDOWN:
		return gamewndevent(KY_DOWN, wParam, lParam);
	case WM_KEYUP:
		return gamewndevent(KY_UP, wParam, lParam);
	case WM_CHAR:
		return gamewndevent(KY_CHAR, wParam, lParam);
	case WM_MOUSEMOVE:
		message = MS_MOVE;
		goto mousecallev1;
	case WM_LBUTTONDOWN:
		message = MS_LDOWN;
		goto mousecallev1;
	case WM_LBUTTONUP:
		message = MS_LUP;
		goto mousecallev1;
	case WM_RBUTTONDOWN:
		message = MS_RDOWN;
		goto mousecallev1;
	case WM_RBUTTONUP:
		message = MS_RUP;
		goto mousecallev1;
	case WM_MBUTTONDOWN:
		message = MS_MDOWN;
		goto mousecallev1;
	case WM_MBUTTONUP:
		message = MS_MUP;
		goto mousecallev1;
	mousecallev1: 
		return gamewndevent(message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	case WM_MOUSEWHEEL:
		message = MS_WHEEL;
		SetWheelBywParam(wParam);
	/*mousecallev2:*/ {
			POINT pt{};
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			ScreenToClient(hwnd, &pt);
			return gamewndevent(message, pt.x, pt.y);
		}
	default:
		return 0;
	}
}

static LRESULT CALLBACK gamewndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	currentmessage = message;
	if (gamewndevent(WND_MESSAGE, wParam, lParam) != 0) return 0;
	if (event_dispatch(hwnd, message, wParam, lParam) != 0) return 0;
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

static int donothing_event(UINT, intptr_t, intptr_t) { return 0; }

void LPC::gamewndstartup(event_t event, const char* title) {
	PushDefaultClassSetting setting;
	if (gamewindow != nullptr) return;
	gamewndevent = donothing_event;
	setting->ownDC(true);
	gameclass = new windowclass;
	gamewindow = new defaultWindow;
	gamewindow->setDefaultProc(gamewndproc);
	if(title) gamewindow->setTitle(title);
	gamewndevent = event;
}

void LPC::gamewndshutdown() {
	delete gamewindow;
	delete gameclass;
}

LPC::event_t LPC::setevent(event_t event) {
	std::swap(event, gamewndevent);
	return event;
}

UINT LPC::GetMSGCode() {
	return currentmessage;
}
