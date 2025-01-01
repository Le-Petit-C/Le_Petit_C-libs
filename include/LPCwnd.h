
#ifndef LPCwnd_h
#define LPCwnd_h

#include <windows.h>
#include <string>
#include <atomic>
#include "LPCdebug.h"
#include "LPCextra.h"

#ifndef _LIB

#pragma comment(lib, "gdiplus.lib")

#ifndef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\LPCwnd.lib")
#else
#pragma comment(lib, "..\\Release\\LPCwnd.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "..\\x64\\Debug\\LPCwnd.lib")
#else
#pragma comment(lib, "..\\x64\\Release\\LPCwnd.lib")
#endif
#endif

#endif


namespace LPC {
	void msgBox(const char* text, const char* caption, UINT type = 0);
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0);

	typedef void (*MOUSEPROC)(int type, int x, int y, int wheel);//鼠标消息的窗口回调函数
	typedef void (*KEYPROC)(bool type, int key);//键盘消息的窗口回调函数
	typedef void (*PAINTPROC)(HDC hdc);//绘图消息的窗口回调函数

	inline bool isKeyPressed(int key);//判断按键是否按下，key是虚拟键码。处理键盘消息的时候或许能用到。
	
	//窗口对象。
	//创建时优先使用构造函数的参数，不足部分再取用默认设置中的参数。
	class window;

	//窗口创建时的设置（当然是创建窗口时用啦，你也可以选择把设置保留下来以后用）
	struct WindowCreateSetting;

	//窗口默认设置对象，存储设置的同时作为栈的一层。
	//构造时自动压栈，并将上一默认复制到自身。析构时自动出栈。
	//可以直接修改成员，也就是修改了默认项。
	class PushDefaultWindowSetting;

	//获取当前窗口创建的默认设置
	WindowCreateSetting* getWindowDefaultSetting();

	//窗口类对象。每一个窗口创建时都归属一个窗口类。
	//不要在销毁属于这个窗口类对象的所有窗口前析构这个窗口类对象
	//不要用已销毁的窗口类对象创建窗口
	class windowclass;

	//窗口类创建时的设置（当然是创建窗口时用啦，你也可以选择把设置保留下来以后用）
	struct WindowClassCreateSetting;
	
	//窗口默认设置对象，存储设置的同时作为栈的一层。
	//构造时自动压栈，并将上一默认复制到自身。析构时自动出栈。
	//可以直接修改成员，也就是修改了默认项。
	class PushDefaultClassSetting;

	//获取当前窗口类创建的默认设置
	WindowClassCreateSetting* getWindowClassDefaultSetting();
}

namespace LPC {
	//设置默认的创建参数
	//设置创建的窗口大小位置
	//基于窗口设置
	inline void setCreateWindowRect(const RECT &rect);
	inline void setCreateWindowRect(const RECT *rect);
	//基于窗口客户区设置
	inline void setCreateClientRect(const RECT &rect);
	inline void setCreateClientRect(const RECT *rect);
}

namespace LPC {
	enum specificWindowStyles {
		MaximizeWindow,//最大化窗口
		FullScreenWindow//全屏窗口
	};
#ifndef LPCNOMACRO
	#define DSKW (getDesktopWidth())
	#define DSKH (getDesktopHeight())
#else//如果不用宏定义，这么操作可以达到类似的效果。但是遇到不定参数函数如printf无法正确返回值。
	struct { inline operator int() const { return getDesktopWidth(); } } DSKW;
	struct { inline operator int() const { return getDesktopHeight(); } } DSKH;
#endif
	inline int getDesktopWidth();
	inline int getDesktopHeight();
}

struct LPC::WindowCreateSetting {
	DWORD dwExStyle;
	const windowclass* pwndclass;
	std::string titleA;
	std::wstring titleW;
	DWORD dwStyle;
	int X, Y, nWidth, nHeight;
	HWND Parent;
	HMENU menu;
	HINSTANCE hInstance;
	void* lpParam;
	WindowCreateSetting();
	WindowCreateSetting(const WindowCreateSetting&) = default;
	HWND create() const;
	void setTitle(std::string);
	void setTitle(std::wstring);
	void setWindowRect(const RECT &rect);
	inline void setWindowRect(const RECT *rect);
	void setClientRect(RECT rect);
	inline void setClientRect(const RECT *rect);
};

class LPC::PushDefaultWindowSetting {
public:
	WindowCreateSetting setting;
	PushDefaultWindowSetting();
	inline WindowCreateSetting* operator->() { return &setting; }
private:
	WindowCreateSetting* lastDefault;
};

struct LPC::WindowClassCreateSetting {
	UINT        style;
	WNDPROC     lpfnWndProc;
	int         cbClsExtra;
	int         cbWndExtra;
	HINSTANCE   hInstance;
	HICON       hIcon;
	HCURSOR     hCursor;
	HBRUSH      hbrBackground;
	std::string menuNameA;
	std::wstring menuNameW;
	//你可以直接给这些参数赋值，当然也可以调用下面的函数
	//给窗口加上或去除CS_OWNDC属性
	void ownDC(bool b);
	//将本窗口的相关参数设置为默认状态，方便之后创建窗口
	void setDefault();
	//修改菜单名
	void setMenuName(std::string);
	void setMenuName(std::wstring);
	//构造函数
	WindowClassCreateSetting();
	WindowClassCreateSetting(const WindowClassCreateSetting&) = default;
};

class LPC::PushDefaultClassSetting {
public:
	WindowClassCreateSetting setting;
	PushDefaultClassSetting();
	inline WindowClassCreateSetting* operator->() { return &setting; }
private:
	WindowClassCreateSetting* lastDefault;
};

class LPC::windowclass {
	static std::atomic_uintptr_t classid;
	intptr_t thisid = 0;
	HINSTANCE hInstance;
	std::string classNameA;
	std::wstring classNameW;
	ATOM classAtom;
	void reg();
	void unreg();
public:
	windowclass(std::wstring classNameW);
	windowclass(std::string classNameA);
	//只是返回存储变量名的字符串地址，不是新开辟一块内存空间，无需手动释放内存
	const char* getClassNameA() const;
	const wchar_t* getClassNameW() const;
	operator const char* ();
	operator const wchar_t* ();
	windowclass();
	~windowclass();
};

class LPC::window {
public:
	using flag_t = uint8_t;
	/*窗口大小改变消息的窗口回调函数，参数是客户区大小。
	type是大小改变的类型，详细内容导航:
	SIZE_RESTORED
	(Ctrl + 鼠标左键单击索引词可导航至内容)。*/
	typedef void (*SIZEPROC)(int w, int h, int type);
protected:
	//窗口句柄
	HWND hwnd = nullptr;
	//窗口客户区矩形区域缓存
	RECT clientRect{};

	//用户自定义内容
	LONG_PTR User = 0;
	MOUSEPROC mouseProc = nullptr;
	KEYPROC keyProc = nullptr;
	PAINTPROC paintProc = nullptr;
	SIZEPROC sizeProc = nullptr;
	WNDPROC defaultProc = DefWindowProc;//剩余消息的窗口回调函数，如果之前的回调函数未设置，或者消息没有匹配的回调函数，会调用这个回调函数
	dexcoc testIfExist() const;
	void refRect();
	static LRESULT CALLBACK LPCwindowDefWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
public:
	window();
	window(const char *title);
	window(const char *title, int clientwidth, int clientHeight);
	window(const char *title, int x, int y, int clientwidth, int clientHeight);
	window(const char *title, RECT clientRect);
	window(const wchar_t *title);
	window(const wchar_t *title, int clientwidth, int clientHeight);
	window(const wchar_t *title, int x, int y, int clientwidth, int clientHeight);
	window(const wchar_t *title, RECT clientRect);
	window(specificWindowStyles style);
	window(const window&) = delete;
	dexcoc create(const WindowCreateSetting* setting = getWindowDefaultSetting());
	constexpr HWND gethwnd() const {
		return hwnd;
	}
	constexpr operator HWND() const {
		return hwnd;
	}
	//成功返回非零值:有窗口时返回SetWindowTextW的返回值，无窗口时返回1
	template<typename char_t>
	BOOL setTitle(const char_t* title) {
		if (hwnd != nullptr)
			return SetWindowTextA(hwnd, title);
		return 1;
	}
	static void resizeWindow(HWND hwnd, int width, int height);//设置窗口大小
	static void resizeClient(HWND hwnd, int width, int height);//设置窗口客户区大小
	inline void resizeWindow(int width, int height) const;//设置窗口大小
	inline void resizeClient(int width, int height) const;//设置窗口客户区大小
	void swap(window& wnd);
	window(window&& wnd) noexcept;
	dexcoc destroy();
	~window();
	void show() const;
	void hide() const;
	void fullScreen() const;//全屏
	void maxShow() const;//最大化，如果窗口样式合适的话会直接全屏
	void restore() const;//恢复默认窗口大小和样式
	bool isFullScreen() const;//是否全屏
	void msgBox(const char* text, const char* caption, UINT type = 0) const;
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0) const;
	inline constexpr bool windowCreated() const {
		return hwnd != nullptr;
	}
	//Windows中默认的User被占用了，请用这个设置额外的User Data
	//返回值为原来的User Data
	inline LONG_PTR setUser(LONG_PTR newuser);
	//Windows中默认的User被占用了，请用这个获取额外的User Data
	inline LONG_PTR getUser() const;
	//设置鼠标消息的回调函数，返回值为原来的回调函数
	inline MOUSEPROC setMouseProc(MOUSEPROC proc);
	//设置键盘消息的回调函数，返回值为原来的回调函数
	inline KEYPROC setKeyProc(KEYPROC proc);
	//设置绘图消息的回调函数，返回值为原来的回调函数
	inline PAINTPROC setPaintProc(PAINTPROC proc);
	//设置窗口大小改变消息的回调函数，返回值为原来的回调函数
	inline SIZEPROC setSizeProc(SIZEPROC proc);
	//Windows中默认的窗口消息回调函数被占用了，请用这个设置额外的窗口消息回调函数
	//设置默认的窗口消息回调函数，返回值为原来的回调函数
	inline WNDPROC setDefaultProc(WNDPROC proc);
	//等待窗口被销毁。如果调用线程是窗口所属线程，会进入只获取该窗口的消息的消息循环。如果不是该窗口所属线程，会直接退出。
	dexcoc waitForDestroy();
	//获取窗口客户区矩形区域
	inline const RECT &getClientRect() const;
	//获取窗口客户区宽度
	inline int getClientWidth() const;
	//获取窗口客户区高度
	inline int getClientHeight() const;
};

namespace LPC {
	struct wndDC {
		inline wndDC(HWND hWnd) { hwnd = hWnd; hdc = GetDC(hwnd); }
		inline wndDC(const window &wnd) { hwnd = wnd.gethwnd(); hdc = GetDC(hwnd); };
		inline wndDC(const window *wnd) { hwnd = wnd->gethwnd(); hdc = GetDC(hwnd); };
		inline ~wndDC() { ReleaseDC(hwnd, hdc); }
		inline operator HDC() { return hdc; }
	private:
		HWND hwnd;
		HDC hdc;
	};
}
namespace LPC{
	inline bool isKeyPressed(int key) {
		return GetAsyncKeyState(key) & 0x8000;
	}
	inline void setCreateWindowRect(const RECT &rect) {
		getWindowDefaultSetting()->setWindowRect(rect);
	}
	inline void setCreateWindowRect(const RECT *rect) {
		getWindowDefaultSetting()->setWindowRect(rect);
	}
	inline void setCreateClientRect(const RECT &rect){
		getWindowDefaultSetting()->setClientRect(rect);
	}
	inline void setCreateClientRect(const RECT *rect) {
		getWindowDefaultSetting()->setClientRect(rect);
	}
	inline int getDesktopWidth() {
		return GetSystemMetrics(SM_CXSCREEN);
	}
	inline int getDesktopHeight() {
		return GetSystemMetrics(SM_CYSCREEN);
	}
	inline void WindowCreateSetting::setWindowRect(const RECT *rect) {
		setWindowRect(*rect);
	}
	inline void WindowCreateSetting::setClientRect(const RECT *rect) {
		setClientRect(*rect);
	}
	inline void window::resizeWindow(int width, int height) const {
		resizeWindow(hwnd, width, height);
	}
	inline void window::resizeClient(int width, int height) const {
		resizeClient(hwnd, width, height);
	}
	inline LONG_PTR window::setUser(LONG_PTR newuser) {
		LONG_PTR lastUser = User;
		User = newuser;
		return lastUser;
	}
	inline LONG_PTR window::getUser() const {
		return User;
	}
	inline MOUSEPROC window::setMouseProc(MOUSEPROC proc) {
		MOUSEPROC lastproc = mouseProc;
		mouseProc = proc;
		return lastproc;
	}
	inline KEYPROC window::setKeyProc(KEYPROC proc) {
		KEYPROC lastproc = keyProc;
		keyProc = proc;
		return lastproc;
	}
	inline PAINTPROC window::setPaintProc(PAINTPROC proc) {
		PAINTPROC lastproc = paintProc;
		paintProc = proc;
		return lastproc;
	}
	inline window::SIZEPROC window::setSizeProc(SIZEPROC proc) {
		SIZEPROC lastproc = sizeProc;
		sizeProc = proc;
		return lastproc;
	}
	inline WNDPROC window::setDefaultProc(WNDPROC proc) {
		WNDPROC lastproc = defaultProc;
		defaultProc = proc;
		return lastproc;
	}
	inline const RECT &window::getClientRect() const {
		return clientRect;
	}
	inline int window::getClientWidth() const {
		return clientRect.right - clientRect.left;
	}
	inline int window::getClientHeight() const {
		return clientRect.bottom - clientRect.top;
	}
}

#endif
