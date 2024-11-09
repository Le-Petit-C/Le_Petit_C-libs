
#include "pch.h"
#include <unordered_map>


LPC::timer::timer(HWND hwnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc) {
	this->hwnd = hwnd;
	id = SetTimer(hwnd, nIDEvent, uElapse, lpTimerFunc);
}

LPC::timer::~timer() {
	KillTimer(hwnd, id);
}
