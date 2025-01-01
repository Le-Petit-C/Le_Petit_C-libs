
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

	typedef void (*MOUSEPROC)(int type, int x, int y, int wheel);//�����Ϣ�Ĵ��ڻص�����
	typedef void (*KEYPROC)(bool type, int key);//������Ϣ�Ĵ��ڻص�����
	typedef void (*PAINTPROC)(HDC hdc);//��ͼ��Ϣ�Ĵ��ڻص�����

	inline bool isKeyPressed(int key);//�жϰ����Ƿ��£�key��������롣���������Ϣ��ʱ��������õ���
	
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

namespace LPC {
	//����Ĭ�ϵĴ�������
	//���ô����Ĵ��ڴ�Сλ��
	//���ڴ�������
	inline void setCreateWindowRect(const RECT &rect);
	inline void setCreateWindowRect(const RECT *rect);
	//���ڴ��ڿͻ�������
	inline void setCreateClientRect(const RECT &rect);
	inline void setCreateClientRect(const RECT *rect);
}

namespace LPC {
	enum specificWindowStyles {
		MaximizeWindow,//��󻯴���
		FullScreenWindow//ȫ������
	};
#ifndef LPCNOMACRO
	#define DSKW (getDesktopWidth())
	#define DSKH (getDesktopHeight())
#else//������ú궨�壬��ô�������Դﵽ���Ƶ�Ч��������������������������printf�޷���ȷ����ֵ��
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

class LPC::window {
public:
	using flag_t = uint8_t;
	/*���ڴ�С�ı���Ϣ�Ĵ��ڻص������������ǿͻ�����С��
	type�Ǵ�С�ı�����ͣ���ϸ���ݵ���:
	SIZE_RESTORED
	(Ctrl + ���������������ʿɵ���������)��*/
	typedef void (*SIZEPROC)(int w, int h, int type);
protected:
	//���ھ��
	HWND hwnd = nullptr;
	//���ڿͻ����������򻺴�
	RECT clientRect{};

	//�û��Զ�������
	LONG_PTR User = 0;
	MOUSEPROC mouseProc = nullptr;
	KEYPROC keyProc = nullptr;
	PAINTPROC paintProc = nullptr;
	SIZEPROC sizeProc = nullptr;
	WNDPROC defaultProc = DefWindowProc;//ʣ����Ϣ�Ĵ��ڻص����������֮ǰ�Ļص�����δ���ã�������Ϣû��ƥ��Ļص����������������ص�����
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
	//�ɹ����ط���ֵ:�д���ʱ����SetWindowTextW�ķ���ֵ���޴���ʱ����1
	template<typename char_t>
	BOOL setTitle(const char_t* title) {
		if (hwnd != nullptr)
			return SetWindowTextA(hwnd, title);
		return 1;
	}
	static void resizeWindow(HWND hwnd, int width, int height);//���ô��ڴ�С
	static void resizeClient(HWND hwnd, int width, int height);//���ô��ڿͻ�����С
	inline void resizeWindow(int width, int height) const;//���ô��ڴ�С
	inline void resizeClient(int width, int height) const;//���ô��ڿͻ�����С
	void swap(window& wnd);
	window(window&& wnd) noexcept;
	dexcoc destroy();
	~window();
	void show() const;
	void hide() const;
	void fullScreen() const;//ȫ��
	void maxShow() const;//��󻯣����������ʽ���ʵĻ���ֱ��ȫ��
	void restore() const;//�ָ�Ĭ�ϴ��ڴ�С����ʽ
	bool isFullScreen() const;//�Ƿ�ȫ��
	void msgBox(const char* text, const char* caption, UINT type = 0) const;
	void msgBox(const wchar_t* text, const wchar_t* caption, UINT type = 0) const;
	inline constexpr bool windowCreated() const {
		return hwnd != nullptr;
	}
	//Windows��Ĭ�ϵ�User��ռ���ˣ�����������ö����User Data
	//����ֵΪԭ����User Data
	inline LONG_PTR setUser(LONG_PTR newuser);
	//Windows��Ĭ�ϵ�User��ռ���ˣ����������ȡ�����User Data
	inline LONG_PTR getUser() const;
	//���������Ϣ�Ļص�����������ֵΪԭ���Ļص�����
	inline MOUSEPROC setMouseProc(MOUSEPROC proc);
	//���ü�����Ϣ�Ļص�����������ֵΪԭ���Ļص�����
	inline KEYPROC setKeyProc(KEYPROC proc);
	//���û�ͼ��Ϣ�Ļص�����������ֵΪԭ���Ļص�����
	inline PAINTPROC setPaintProc(PAINTPROC proc);
	//���ô��ڴ�С�ı���Ϣ�Ļص�����������ֵΪԭ���Ļص�����
	inline SIZEPROC setSizeProc(SIZEPROC proc);
	//Windows��Ĭ�ϵĴ�����Ϣ�ص�������ռ���ˣ�����������ö���Ĵ�����Ϣ�ص�����
	//����Ĭ�ϵĴ�����Ϣ�ص�����������ֵΪԭ���Ļص�����
	inline WNDPROC setDefaultProc(WNDPROC proc);
	//�ȴ����ڱ����١���������߳��Ǵ��������̣߳������ֻ��ȡ�ô��ڵ���Ϣ����Ϣѭ����������Ǹô��������̣߳���ֱ���˳���
	dexcoc waitForDestroy();
	//��ȡ���ڿͻ�����������
	inline const RECT &getClientRect() const;
	//��ȡ���ڿͻ������
	inline int getClientWidth() const;
	//��ȡ���ڿͻ����߶�
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
