
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

namespace LPC {
	static WindowCreateSetting& windowDefault() {
		static WindowCreateSetting windowDefault;
		return windowDefault;
	}
	static WindowCreateSetting*& windowDefaultSetting() {
		static WindowCreateSetting* windowDefaultSetting = &windowDefault();
		return windowDefaultSetting;
	}
	static WindowClassCreateSetting& windowClassDefault() {
		static WindowClassCreateSetting windowClassDefault;
		return windowClassDefault;
	}
	static WindowClassCreateSetting*& windowClassDefaultSetting() {
		static WindowClassCreateSetting* windowClassDefaultSetting = &windowClassDefault();
		return windowClassDefaultSetting;
	}
	static windowclass& Defaultclass() {
		static windowclass Defaultclass;
		return Defaultclass;
	}
}


LPC::WindowCreateSetting* LPC::getWindowDefaultSetting() {
	return windowDefaultSetting();
}
LPC::WindowClassCreateSetting* LPC::getWindowClassDefaultSetting() {
	return windowClassDefaultSetting();
}
void LPC::msgBox(const char* text, const char* caption, UINT type) {
	MessageBoxA(0, text, caption, type);
}
void LPC::msgBox(const wchar_t* text, const wchar_t* caption, UINT type) {
	MessageBoxW(0, text, caption, type);
}

LPC::WindowCreateSetting::WindowCreateSetting() :
	dwExStyle(0),
	pwndclass(&Defaultclass()),
	titleA(defaultWindowTitle),
	titleW(defaultWindowTitleW),
	dwStyle(WS_OVERLAPPEDWINDOW),
	X(CW_USEDEFAULT),
	Y(CW_USEDEFAULT),
	nWidth(CW_USEDEFAULT),
	nHeight(CW_USEDEFAULT),
	Parent(nullptr),
	menu(nullptr),
	hInstance(GetModuleHandle(NULL)),
	lpParam(nullptr) {}

HWND LPC::WindowCreateSetting::create() const{
	return CreateWindowExW(
		dwExStyle,
		pwndclass->getClassNameW(),
		titleW.c_str(),
		dwStyle,
		X, Y, nWidth, nHeight,
		Parent,
		menu,
		hInstance,
		lpParam);
}

void LPC::WindowCreateSetting::setTitle(std::string title) {
	titleW = stringToWString(title);
	titleA.swap(title);
}

void LPC::WindowCreateSetting::setTitle(std::wstring title) {
	titleA = wstringToString(title);
	titleW.swap(title);
}

LPC::PushDefaultWindowSetting::PushDefaultWindowSetting() : lastDefault(windowDefaultSetting()), setting(*windowDefaultSetting()) {
	windowDefaultSetting() = &setting;
}

void LPC::WindowClassCreateSetting::ownDC(bool b) {
	if (b) style |= CS_OWNDC;
	else style &= ~CS_OWNDC;
}

void LPC::WindowClassCreateSetting::setDefault() {
	style = CS_HREDRAW | CS_VREDRAW;
	lpfnWndProc = DefWindowProc;
	cbClsExtra = 0;
	cbWndExtra = 0;
	hInstance = GetModuleHandle(NULL);
	hIcon = nullptr;
	hCursor = LoadCursor(nullptr, IDC_ARROW);
	hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	menuNameA = "LPCWND menu";
	menuNameW = L"LPCWND menu";
}

void LPC::WindowClassCreateSetting::setMenuName(std::string menuName) {
	menuNameW = stringToWString(menuName);
	menuNameA.swap(menuName);
}

void LPC::WindowClassCreateSetting::setMenuName(std::wstring menuName) {
	menuNameA = wstringToString(menuName);
	menuNameW.swap(menuName);
}

LPC::PushDefaultClassSetting::PushDefaultClassSetting() : lastDefault(windowClassDefaultSetting()), setting(*windowClassDefaultSetting()){
	windowClassDefaultSetting() = &setting;
}

LPC::WindowClassCreateSetting::WindowClassCreateSetting() {
	setDefault();
}

/*LPC::dexcoc LPC::windowclass::createdErrorDetect() const {
	if (classAtom != 0) {
		setexception("You cannot set a window class when it has already been created!");
		return true;
	}
	return false;
}*/

void LPC::windowclass::reg() {
	//if (createdErrorDetect()) return true;
	WNDCLASSW wc{};
	wc.style = windowClassDefaultSetting()->style;
	wc.lpfnWndProc = windowClassDefaultSetting()->lpfnWndProc;
	wc.cbClsExtra = windowClassDefaultSetting()->cbClsExtra;
	wc.cbWndExtra = windowClassDefaultSetting()->cbWndExtra;
	wc.hInstance = hInstance;
	wc.hIcon = windowClassDefaultSetting()->hIcon;
	wc.hCursor = windowClassDefaultSetting()->hCursor;
	wc.hbrBackground = windowClassDefaultSetting()->hbrBackground;
	wc.lpszMenuName = windowClassDefaultSetting()->menuNameW.c_str();
	wc.lpszClassName = classNameW.c_str();
	classAtom = RegisterClassW(&wc);
}

void LPC::windowclass::unreg() {
	UnregisterClassW(classNameW.c_str(), hInstance);
	classAtom = 0;
}

LPC::windowclass::windowclass(std::wstring className) {
	classNameA = wstringToString(className);
	classNameW = className;
	hInstance = windowClassDefaultSetting()->hInstance;
	reg();
}

LPC::windowclass::windowclass(std::string className) {
	classNameA = className;
	classNameW = stringToWString(className);
	hInstance = windowClassDefaultSetting()->hInstance;
	reg();
}

const char* LPC::windowclass::getClassNameA() const {
	return classNameA.c_str();
}

const wchar_t* LPC::windowclass::getClassNameW() const {
	return classNameW.c_str();
}

LPC::windowclass::operator const char* () {
	return classNameA.c_str();
}
LPC::windowclass::operator const wchar_t* () {
	return classNameW.c_str();
}

LPC::windowclass::windowclass() {
	classAtom = 0;
	thisid = classid++;
	constexpr size_t bufsizeInWords = 65536;
	std::vector<wchar_t> buf;
	buf.resize(bufsizeInWords);
	GetModuleFileNameW(nullptr, &buf.front(), bufsizeInWords);
	classNameW = std::wstring(&buf.front());
	swprintf_s(&buf.front(), bufsizeInWords, L"_%zx_LPCWND", thisid);
	classNameW += &buf.front();
	classNameA = wstringToString(classNameW);
	reg();
}

LPC::windowclass::~windowclass() {
	if (classAtom != 0) unreg();
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

LPC::window::window() {
	create();
	show();
}

LPC::window::window(const char* title) {
	PushDefaultWindowSetting setting;
	setting->setTitle(title);
	create();
	show();
}

void LPC::window::swap(window& wnd) {
	std::swap(hwnd, wnd.hwnd);
	std::swap(clientRect, wnd.clientRect);
	std::swap(User, wnd.User);
	//std::swap(createContainer, wnd.createContainer);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(wnd.hwnd, GWLP_USERDATA, (LONG_PTR)&wnd);
}

LPC::window::window(window&& wnd) noexcept {
	swap(wnd);
}

LPC::dexcoc LPC::window::create(const WindowCreateSetting* setting) {
	hwnd = setting->create();
	if (hwnd == nullptr) {
		char buf[256];
		sprintf_s(buf, sizeof(buf), "CreateWindowExW Failed! Error code: %x", GetLastError());
		setexception(buf);
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

void LPC::window::show() const{
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void LPC::window::hide() const{
	ShowWindow(hwnd, SW_HIDE);
}

void LPC::window::msgBox(const char* text, const char* caption, UINT type) const{
	MessageBoxA(hwnd, text, caption, type);
}

void LPC::window::msgBox(const wchar_t* text, const wchar_t* caption, UINT type) const {
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

std::atomic_uintptr_t LPC::windowclass::classid = 0;
