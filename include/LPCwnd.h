
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
	
	//���ڶ���
	//����ʱ����ʹ�ù��캯���Ĳ��������㲿����ȡ��Ĭ�������еĲ�����
	class window;

	//���ڴ���ʱ�����ã���Ȼ�Ǵ�������ʱ��������Ҳ����ѡ������ñ��������Ժ��ã�
	struct WindowCreateSetting;

	//����Ĭ�����ö��󣬴洢���õ�ͬʱ��Ϊջ��һ�㡣
	//����ʱ�Զ�ѹջ��������һĬ�ϸ��Ƶ���������ʱ�Զ���ջ��
	//����ֱ���޸ĳ�Ա��Ҳ�����޸���Ĭ���
	class PushDefaultWindowSetting;

	//��ȡ��ǰ���ڴ�����Ĭ������
	WindowCreateSetting* getWindowDefaultSetting();

	//���������ÿһ�����ڴ���ʱ������һ�������ࡣ
	//��Ҫ��������������������������д���ǰ����������������
	//��Ҫ�������ٵĴ�������󴴽�����
	class windowclass;

	//�����ഴ��ʱ�����ã���Ȼ�Ǵ�������ʱ��������Ҳ����ѡ������ñ��������Ժ��ã�
	struct WindowClassCreateSetting;
	
	//����Ĭ�����ö��󣬴洢���õ�ͬʱ��Ϊջ��һ�㡣
	//����ʱ�Զ�ѹջ��������һĬ�ϸ��Ƶ���������ʱ�Զ���ջ��
	//����ֱ���޸ĳ�Ա��Ҳ�����޸���Ĭ���
	class PushDefaultClassSetting;

	//��ȡ��ǰ�����ഴ����Ĭ������
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
	//�����ֱ�Ӹ���Щ������ֵ����ȻҲ���Ե�������ĺ���
	//�����ڼ��ϻ�ȥ��CS_OWNDC����
	void ownDC(bool b);
	//�������ڵ���ز�������ΪĬ��״̬������֮�󴴽�����
	void setDefault();
	//�޸Ĳ˵���
	void setMenuName(std::string);
	void setMenuName(std::wstring);
	//���캯��
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
	//ֻ�Ƿ��ش洢���������ַ�����ַ�������¿���һ���ڴ�ռ䣬�����ֶ��ͷ��ڴ�
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
	//���ھ��
	HWND hwnd = nullptr;
	//���ڿͻ����������򻺴�
	RECT clientRect{};

	//�û��Զ�������
	LONG_PTR User = 0;
	//���ڻص�����
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
	//�ɹ����ط���ֵ:�д���ʱ����SetWindowTextW�ķ���ֵ���޴���ʱ����1
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
	//Windows��Ĭ�ϵ�User��ռ���ˣ�����������ö����User Data
	//����ֵΪԭ����User Data
	constexpr LONG_PTR setUser(LONG_PTR newuser) {
		LONG_PTR lastUser = User;
		User = newuser;
		return lastUser;
	}
	//Windows��Ĭ�ϵ�User��ռ���ˣ����������ȡ�����User Data
	constexpr LONG_PTR getUser(LONG_PTR newuser) const {
		return User;
	}
	//�����û����Զ�����Ϣ�ص�������û������ʱ��DefWindowProc��
	constexpr WNDPROC setProc(WNDPROC proc) {
		WNDPROC lastproc = UserProc;
		UserProc = proc;
		return lastproc;
	}
	//�ȴ����ڱ����١���������߳��Ǵ��������̣߳������ֻ��ȡ�ô��ڵ���Ϣ����Ϣѭ����������Ǹô��������̣߳���ֱ���˳���
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
