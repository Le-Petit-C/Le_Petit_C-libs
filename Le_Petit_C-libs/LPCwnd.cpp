
#include "pch.h"

namespace {
	enum {
		LPCWND_DONE
	};
	UINT LPCWND_DONE_ID;
	struct LPCwndinitst {
		LPCwndinitst() {
			LPCWND_DONE_ID = LPC::registerEvent("LPCWND_DONE", LPCWND_DONE, nullptr);
		}
	};
}


void LPC::msgBox(const char* text, const char* caption, UINT type) {
	MessageBoxA(0, text, caption, type);
}
void LPC::msgBox(const wchar_t* text, const wchar_t* caption, UINT type) {
	MessageBoxW(0, text, caption, type);
}

namespace LPC {
	HINSTANCE statichInstance;
	namespace {
		class statichInstanceIniter{
			statichInstanceIniter() {
				statichInstance = GetModuleHandle(NULL);
			}
		};
	}
}

LPC::dexcoc LPC::windowclass::createdErrorDetect() const {
	if (classAtom != 0) {
		setexception("You cannot set a window class when it has already been created!");
		return true;
	}
	return false;
}

LPC::dexcoc LPC::windowclass::reg() {
	if (createdErrorDetect()) return true;
	classAtom = RegisterClassW(&wc);
	return false;
}

LPC::dexcoc LPC::windowclass::unreg() {
	if (classAtom == 0) {
		setexception("There are no window classes here that need to be unregistered.");
		return true;
	}
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	classAtom = 0;
	return false;
}

LPC::dexcoc LPC::windowclass::ownDC(bool b) {
	if (createdErrorDetect()) return true;
	if (b) wc.style |= CS_OWNDC;
	else wc.style &= ~CS_OWNDC;
	return false;
}

const wchar_t* LPC::windowclass::getClassName() const {
	return className;
}

LPC::dexcoc LPC::windowclass::setDefault() {
	if (createdErrorDetect()) return true;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = statichInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszClassName = className;
	wc.lpszMenuName = nullptr;
	return false;
}

LPC::windowclass::operator const wchar_t* () {
	return wc.lpszClassName;
}

const WNDCLASSW& LPC::windowclass::getwc() const {
	return wc;
}

LPC::windowclass::windowclass(bool instant_create) {
	classAtom = 0;
	thisid = classid++;
	setDefault();
	wchar_t buf[bufsizeInWords];
	GetModuleFileNameW(nullptr, buf, bufsizeInWords);
	swprintf_s(className, bufsizeInWords, L"LPCWND_%tu_%ls", thisid, buf);
	if (instant_create) reg();
}

LPC::windowclass::~windowclass() {
	if (classAtom != 0) unreg();
}

namespace LPC {
	windowclass Defaultclass;
}

LPC::wndCreateContainerExW::wndCreateContainerExW() {
	wcscpy_s(title, sizeof(title) / sizeof(title[0]), defaultWindowTitle);
	X = Y = nWidth = nHeight = CW_USEDEFAULT;
}

HWND LPC::CreateWindowExW(const wndCreateContainerExW& createData) {
	return ::CreateWindowExW(
		createData.dwExStyle,
		createData.pwndclass->getwc().lpszClassName,
		createData.title,
		createData.dwStyle,
		createData.X,
		createData.Y,
		createData.nWidth,
		createData.nHeight,
		createData.Parent,
		createData.menu,
		createData.hInstance,
		createData.lpParam
	);
}

static inline void windowDoesNotExists() {
	LPC::setexception("Window does not exists!");
}

LPC::dexcoc LPC::window::testIfExist() const {
	if (hwnd == nullptr) {
		windowDoesNotExists();
		return true;
	}
	return false;
}

void LPC::window::refRect() {
	GetClientRect(hwnd, &clientRect);
}

static constexpr LPC::window::flag_t waiting_for_destroy_flag = 0x01;

LRESULT CALLBACK LPC::window::LPCwindowDefWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	window* wnd = (window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg) {
	case WM_SIZE:
		wnd->refRect();
		break;
	}
	LRESULT ret = wnd->UserProc(hwnd, msg, wp, lp);
	switch (msg) {
	case WM_DESTROY:
		if ((wnd->flag & waiting_for_destroy_flag) != 0)
			wnd->flag &= ~waiting_for_destroy_flag;
		wnd->hwnd = nullptr;
		break;
	}
	return ret;
}

LPC::window::window() {}

LPC::window::window(const char* title) {
	setTitle(title);
	create();
	show();
}

LPC::dexcoc LPC::window::setClass(const windowclass& wc) noexcept {
	if (hwnd != nullptr) {
		setexception("We can't set a class while a window has been created!");
		return true;
	}
	createContainer.pwndclass = &wc;
	return false;
}

void LPC::window::swap(window& wnd) {
	std::swap(hwnd, wnd.hwnd);
	std::swap(wndc, wnd.wndc);
	std::swap(clientRect, wnd.clientRect);
	std::swap(User, wnd.User);
	std::swap(createContainer, wnd.createContainer);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(wnd.hwnd, GWLP_USERDATA, (LONG_PTR)&wnd);
}

LPC::window::window(window&& wnd) noexcept {
	swap(wnd);
}

LPC::dexcoc LPC::window::create() {
	hwnd = CreateWindowExW(createContainer);
	if (hwnd == nullptr) {
		setexception("CreateWindowExW Failed!");
		return 1;
	}
	SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)LPCwindowDefWindowProc);
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	refRect();
	return 0;
}

LPC::dexcoc LPC::window::destroy() {
	if (hwnd == 0) {
		setexception("Why would you try to destroy a window that do not exists?");
		return true;
	}
	DestroyWindow(hwnd);
	hwnd = nullptr;
	return false;
}

LPC::window::~window() {
	if(hwnd != 0)
		destroy();
}

void LPC::window::show() {
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void LPC::window::hide() {
	ShowWindow(hwnd, SW_HIDE);
}

void LPC::window::msgBox(const char* text, const char* caption, UINT type) {
	MessageBoxA(hwnd, text, caption, type);
}

void LPC::window::msgBox(const wchar_t* text, const wchar_t* caption, UINT type) {
	MessageBoxW(hwnd, text, caption, type);
}



LPC::dexcoc LPC::window::waitForDestroy() {
	if (windowCreated()) {
		MSG msg;
		BOOL x = PeekMessage(&msg, hwnd, 0, 0, PM_NOREMOVE);
		if (x == -1) {
			setexception("Probably this window is not for this thread.");
			return true;
		}
		flag |= waiting_for_destroy_flag;
		while (true) {
			x = dGetMessage(&msg, hwnd, 0, 0);
			if (x == -1 || x == 0)
				break;
			TranslateMessage(&msg);
			LPCDispatchMessage(&msg);
			if ((flag & waiting_for_destroy_flag) == 0)
				break;
		}
		if (x == 0 || x == -1) {
			setexception("GetMessage returned unexpected value.");
			return true;
		}
		else return false;
	}
	else return false;
}

/*
UINT_PTR LPC::window::SetTimer(UINT uElapse, TIMERPROC proc, UINT_PTR id = 0) {
	return ::SetTimer(hwnd, id == 0 ? ++timerid : id, uElapse, proc);
}*/

std::atomic_uintptr_t LPC::windowclass::classid = 0;
