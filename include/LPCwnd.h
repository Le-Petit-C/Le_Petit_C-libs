
#ifndef LPCwnd_h
#define LPCwnd_h

#include <unordered_map>

#include <windows.h>
#include <stdint.h>
#include <string.h>
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
	extern HINSTANCE statichInstance;
	class windowclass;
}

class LPC::windowclass {
	static std::atomic_uintptr_t classid;
	intptr_t thisid;
	static const constexpr size_t bufsizeInWords = 256;
	wchar_t menuName[bufsizeInWords];
	wchar_t className[bufsizeInWords];
	WNDCLASSW wc;
	ATOM classAtom;
	dexcoc createdErrorDetect() const;
public:
	dexcoc reg();
	dexcoc unreg();
	template<typename char_t>
	dexcoc setMenuName(char_t* szMenuName) {
		if (createdErrorDetect()) return true;
		if (szMenuName == nullptr)
			wc.lpszMenuName = nullptr;
		else {
			copystring_s(menuName, bufsizeInWords, szMenuName);
			wc.lpszMenuName = menuName;
		}
		return false;
	}
	template<typename char_t>
	dexcoc setClassName(char_t* szClassName) {
		if (createdErrorDetect()) return true;
		copystring_s(className, bufsizeInWords, szClassName);
		return false;
	}
	dexcoc ownDC(bool b);
	//只是返回存储变量名的字符串地址，不是新开辟一块内存空间，无需手动释放内存
	const wchar_t* getClassName() const;
	dexcoc setDefault();
	operator const wchar_t* ();
	const WNDCLASSW& getwc() const;
	windowclass(bool instant_create = true);
	~windowclass();
};

namespace LPC {
	constexpr const wchar_t* defaultWindowTitle = L"Supported by Le_Petit_C";
	extern windowclass Defaultclass;
	struct wndCreateContainerExW {
		DWORD dwExStyle = 0;
		const windowclass* pwndclass = &Defaultclass;
		wchar_t title[256];
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		int X, Y, nWidth, nHeight;
		HWND Parent = nullptr;
		HMENU menu = nullptr;
		HINSTANCE hInstance = statichInstance;
		void* lpParam = nullptr;
		wndCreateContainerExW();
	};
	HWND CreateWindowExW(const wndCreateContainerExW& createData);
	class window;
}

class LPC::window {
public:
	using flag_t = uint8_t;
private:
	HWND hwnd = nullptr;
	windowclass* wndc = &Defaultclass;
	RECT clientRect{};
	UINT_PTR timerid = 0;
	LONG_PTR User = 0;
	WNDPROC UserProc = DefWindowProc;
	wndCreateContainerExW createContainer;
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
	dexcoc setClass(const windowclass& wc) noexcept;
	//成功返回非零值:有窗口时返回SetWindowTextW的返回值，无窗口时返回1
	template<typename char_t>
	BOOL setTitle(const char_t* title) {
		copystring_s(createContainer.title, sizeof(createContainer.title) / sizeof(wchar_t), title);
		if (hwnd != nullptr)
			return SetWindowTextW(hwnd, createContainer.title);
		return 1;
	}
	void swap(window& wnd);
	window(window&& wnd) noexcept;
	dexcoc create();
	dexcoc destroy();
	~window();
	void show();
	void hide();
	void msgBox(const char* text, const char* caption, UINT type = 0);
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0);
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


namespace LPC {
	namespace priv {
		static thread_local struct timerdata_t {
			bool destruced = false;
			typedef std::unordered_map<UINT_PTR, void*> data_t;
			data_t data;
			data_t* operator->() { return &data; }
			data_t& operator*() { return data; }
			~timerdata_t() { destruced = true; }
		}timerdata;
	}
	template<typename ...procParams>
	class timer {
	public:
		typedef void (*proc_t)(procParams...);
		timer(UINT _uElapse, proc_t _proc, bool start = true, procParams... _params) : params(_params...) {
			proc = _proc;
			uElapse = _uElapse;
			timerid = 0;
			if (start) resume();
		}
		~timer() {
			pause();
		}
		//继续定时器
		void resume(){
			if (running) return;
			timerid = SetTimer(nullptr, 0, uElapse, wndtimerproc);
			running = true;
			(*priv::timerdata)[timerid] = this;
		}
		//暂停定时器
		void pause(){
			if (!running) return;
			KillTimer(hwnd, timerid);
			if (!priv::timerdata.destruced)
				priv::timerdata->erase(timerid);
			timerid = 0;
			running = false;
		}
		//如果定时器是暂停着，让它继续，反之让它暂停
		void change() {
			if (running) pause();
			else resume();
		}
	private:
		static void CALLBACK wndtimerproc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
			timer<procParams...>* me = (decltype(me))(*priv::timerdata)[idEvent];
			return std::apply(me->proc, me->params);
		}
		proc_t proc;
		HWND hwnd = nullptr;
		UINT_PTR timerid;
		std::tuple<procParams...> params;
		UINT uElapse;
		bool running = false;
	};
}

#endif
