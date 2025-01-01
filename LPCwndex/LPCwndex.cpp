
#include "pch.h"

namespace LPC {
	gdiplusBitmap *defaultbuffer;
	window *defaultwindow;
	resizeProcMode resizeMode = ALIGN_MIDDLE;
	static void LPCwndexPaintProc(HDC hdc);
	static void LPCwndexSizeProc(int width, int height, int type);
}

static void LPC::LPCwndexPaintProc(HDC hdc) {
	defaultbuffer->toHDC(hdc);
}

static void LPC::LPCwndexSizeProc(int width, int height, int type) {
	bool shouldStretch = ((resizeMode & ALIGN_LEFT) && (resizeMode & ALIGN_RIGHT)) || ((resizeMode & ALIGN_TOP) && (resizeMode & ALIGN_BOTTOM));
	gdiplusBitmap *oldbuffer = defaultbuffer;
	if (shouldStretch) {
		int x, y, w, h;
		defaultbuffer = new gdiplusBitmap(width, height, PixelFormat24bppRGB);
		if (resizeMode & ALIGN_LEFT)
			x = 0;
		else if (resizeMode & ALIGN_RIGHT)
			x = width - oldbuffer->getWidth();
		else
			x = (width - oldbuffer->getWidth()) / 2;
		if (resizeMode & ALIGN_RIGHT)
			w = width - x;
		else w = oldbuffer->getWidth();
		if (resizeMode & ALIGN_TOP)
			y = 0;
		else if (resizeMode & ALIGN_BOTTOM)
			y = height - oldbuffer->getHeight();
		else
			y = (height - oldbuffer->getHeight()) / 2;
		if (resizeMode & ALIGN_BOTTOM)
			h = height - y;
		else h = oldbuffer->getHeight();
		defaultbuffer->grp->DrawImage(oldbuffer->bmp.get(), x, y, w, h);
	}
	else {
		int x, y;
		if (!(resizeMode & ALIGN_LEFT) && !(resizeMode & ALIGN_RIGHT)) {
			x = (width - oldbuffer->getWidth()) / 2;
		}
		else if (resizeMode & ALIGN_LEFT) {
			x = 0;
		}
		else {
			x = width - oldbuffer->getWidth();
		}
		if (!(resizeMode & ALIGN_TOP) && !(resizeMode & ALIGN_BOTTOM)) {
			y = (height - oldbuffer->getHeight()) / 2;
		}
		else if (resizeMode & ALIGN_TOP) {
			y = 0;
		}
		else {
			y = height - oldbuffer->getHeight();
		}
		defaultbuffer = new gdiplusBitmap(*oldbuffer, -x, -y, width, height);
	}
	delete oldbuffer;
}

LPC::setDefaultWindow::setDefaultWindow(window *wnd, bool resetPaintProc, bool resetSizeProc) :
	lastbuffer(defaultbuffer),
	lastwindow(defaultwindow),
	shoulddeletebuffer(true),
	shoulddeletewindow(false) {
	defaultwindow = nullptr;
	defaultbuffer = nullptr;
	set(wnd, resetPaintProc, resetSizeProc);
}

LPC::setDefaultWindow::setDefaultWindow(window &wnd, bool resetPaintProc, bool resetSizeProc) :
	lastbuffer(defaultbuffer),
	lastwindow(defaultwindow),
	shoulddeletebuffer(true),
	shoulddeletewindow(false) {
	defaultwindow = nullptr;
	defaultbuffer = nullptr;
	set(&wnd, resetPaintProc, resetSizeProc);
}

LPC::setDefaultWindow::setDefaultWindow(window &&wnd, bool resetPaintProc, bool resetSizeProc) :
	lastbuffer(defaultbuffer),
	lastwindow(defaultwindow),
	shoulddeletebuffer(true),
	shoulddeletewindow(true) {
	defaultwindow = nullptr;
	defaultbuffer = nullptr;
	set(new window(std::move(wnd)), resetPaintProc, resetSizeProc);
}

LPC::setDefaultWindow::~setDefaultWindow() {
	if (shoulddeletebuffer) delete defaultbuffer;
	if (shoulddeletewindow) delete defaultwindow;
	defaultbuffer = lastbuffer;
	defaultwindow = lastwindow;
}

void LPC::setDefaultWindow::reset(window *wnd, bool resetPaintProc, bool resetSizeProc) {
	set(wnd, resetPaintProc, resetSizeProc);
	shoulddeletebuffer = true;
	shoulddeletewindow = false;
}

void LPC::setDefaultWindow::reset(window &wnd, bool resetPaintProc, bool resetSizeProc) {
	set(&wnd, resetPaintProc, resetSizeProc);
	shoulddeletebuffer = true;
	shoulddeletewindow = false;
}

void LPC::setDefaultWindow::reset(window &&wnd, bool resetPaintProc, bool resetSizeProc) {
	set(new window(std::move(wnd)), resetPaintProc, resetSizeProc);
	shoulddeletebuffer = true;
	shoulddeletewindow = true;
}

void LPC::setDefaultWindow::set(window *wnd, bool resetPaintProc, bool resetSizeProc) {
	if (shoulddeletebuffer && defaultbuffer) delete defaultbuffer;
	if (shoulddeletewindow && defaultwindow) delete defaultwindow;
	if (wnd) {
		defaultbuffer = new gdiplusBitmap(wnd->getClientWidth(), wnd->getClientHeight(), PixelFormat24bppRGB);
		if (resetPaintProc)
			wnd->setPaintProc(LPCwndexPaintProc);
		if (resetSizeProc)
			wnd->setSizeProc(LPCwndexSizeProc);
	}
	else
		defaultbuffer = nullptr;
	defaultwindow = wnd;
}

LPC::defaultWindow::defaultWindow() :
	window(),
	setDefaultWindow(*(window*)this) {}

LPC::defaultWindow::defaultWindow(const char *title) :
	window(title),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const char *title, int width, int height) :
	window(title, width, height),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const char *title, int width, int height, int x, int y) :
	window(title, width, height, x, y),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const char *title, RECT clientRect) :
	window(title, clientRect),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const wchar_t *title) :
	window(title),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const wchar_t *title, int width, int height) :
	window(title, width, height),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const wchar_t *title, int width, int height, int x, int y) :
	window(title, width, height, x, y),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(const wchar_t *title, RECT clientRect) :
	window(title, clientRect),
	setDefaultWindow(*(window *)this) {}

LPC::defaultWindow::defaultWindow(specificWindowStyles style) :
	window(style),
	setDefaultWindow(*(window *)this) {}

void LPC::refsize() {
	HWND hwnd = defaultwindow->gethwnd();
	RECT rect;
	GetClientRect(hwnd, &rect);
	defaultbuffer->resize(rect.right - rect.left, rect.bottom - rect.top);
}

void LPC::resizeWindow(int width, int height) {
	defaultwindow->resizeWindow(width, height);
	refsize();
}

void LPC::resizeClient(int width, int height) {
	defaultwindow->resizeClient(width, height);
	refsize();
}

void LPC::ref() {
	defaultbuffer->toWindow(defaultwindow);
}