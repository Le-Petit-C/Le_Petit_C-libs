
#ifndef LPCwnd_h
#define LPCwnd_h

#include <unordered_map>

#include <iostream>
#include <windows.h>
#include <stdint.h>
#include <string>
#include <malloc.h>
#include <tuple>
#include <thread>
#include <functional>
#include <type_traits>
#include <deque>
#include <atomic>
#include <vector>
#include "LPCmessages.h"
#include "LPCdebug.h"
#include "LPCextra.h"

#pragma comment(lib, "gdiplus.lib")
#include "LPCcommentlib.h"

namespace LPC {
	void msgBox(const char* text, const char* caption, UINT type = 0);
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0);
	
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
	//dexcoc createdErrorDetect() const;
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

namespace LPC {
	/*struct wndCreateContainerExW {
		wndCreateContainerExW();
	};
	HWND CreateWindowExW(const wndCreateContainerExW& createData);*/
}

class LPC::window {
public:
	using flag_t = uint8_t;
private:
	//窗口句柄
	HWND hwnd = nullptr;
	//窗口客户区矩形区域缓存
	RECT clientRect{};

	//用户自定义内容
	LONG_PTR User = 0;
	//窗口回调函数
	WNDPROC UserProc = DefWindowProc;
	//wndCreateContainerExW createContainer;
	flag_t flag = 0;
	dexcoc testIfExist() const;
	void refRect();
	static LRESULT CALLBACK LPCwindowDefWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
public:
	window();
	window(const char* title);
	window(const window&) = delete;
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
	void swap(window& wnd);
	window(window&& wnd) noexcept;
	dexcoc create(const WindowCreateSetting* setting = getWindowDefaultSetting());
	dexcoc destroy();
	~window();
	void show() const;
	void hide() const;
	void msgBox(const char* text, const char* caption, UINT type = 0) const;
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0) const;
	constexpr bool windowCreated() const {
		return hwnd != nullptr;
	}
	//Windows中默认的User被占用了，请用这个设置额外的User Data
	//返回值为原来的User Data
	constexpr LONG_PTR setUser(LONG_PTR newuser) {
		LONG_PTR lastUser = User;
		User = newuser;
		return lastUser;
	}
	//Windows中默认的User被占用了，请用这个获取额外的User Data
	constexpr LONG_PTR getUser(LONG_PTR newuser) const {
		return User;
	}
	//设置用户的自定义消息回调函数。没有设置时是DefWindowProc。
	constexpr WNDPROC setProc(WNDPROC proc) {
		WNDPROC lastproc = UserProc;
		UserProc = proc;
		return lastproc;
	}
	//等待窗口被销毁。如果调用线程是窗口所属线程，会进入只获取该窗口的消息的消息循环。如果不是该窗口所属线程，会直接退出。
	dexcoc waitForDestroy();
	constexpr int getClientWidth() const {
		return clientRect.right - clientRect.left;
	}
	constexpr int getClientHeight() const {
		return clientRect.bottom - clientRect.top;
	}
};

namespace LPC {
	struct wndDC {
		inline wndDC(HWND hWnd) { hwnd = hWnd; hdc = GetDC(hwnd); }
		inline wndDC(const window& wnd) { hwnd = wnd.gethwnd(); hdc = GetDC(hwnd); };
		inline wndDC(const window* wnd) { hwnd = wnd->gethwnd(); hdc = GetDC(hwnd); };
		inline ~wndDC() { ReleaseDC(hwnd, hdc); }
		inline operator HDC() { return hdc; }
	private:
		HWND hwnd;
		HDC hdc;
	};
}

#endif
