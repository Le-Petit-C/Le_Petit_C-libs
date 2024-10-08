
#ifndef LPCwndex_h
#define LPCwndex_h

#include "LPCwnd.h"
#include "LPCgraphics.h"

#ifndef _LIB
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\Le_Petit_C-wndex.lib")
		#else
			#pragma comment(lib, "..\\Release\\Le_Petit_C-wndex.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\Le_Petit_C-wndex.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\Le_Petit_C-wndex.lib")
		#endif
	#endif
#endif

namespace LPC {
	extern wndbuffer* defaultbuffer;
	struct { inline operator int() { return GetSystemMetrics(SM_CXSCREEN); } } DSKW;
	struct { inline operator int() { return GetSystemMetrics(SM_CYSCREEN); } } DSKH;
	struct { inline operator int() { return defaultbuffer->getwidth(); } } SCRW;
	struct { inline operator int() { return defaultbuffer->getheight(); } } SCRH;
	class setDefault {
		wndbuffer* lastbuffer;
		bool shoulddeletewindow = false;
		bool shoulddeletebuffer = false;
	public:
		setDefault(window& wnd);
		setDefault(window&& wnd);
		setDefault(wndbuffer& wnd);
		~setDefault();
	};
	inline wndbuffer* getDefaultBuffer() { return defaultbuffer; }
	inline window* getDefaultWindow() { return defaultbuffer->getwnd(); }
	inline LPC::dexcoc waitForDestroy() { return defaultbuffer->getwnd()->waitForDestroy(); }
	inline auto setProc(WNDPROC proc) { return defaultbuffer->getwnd()->setProc(proc); }
	template<typename ...Args> inline auto resize(Args... args) { return defaultbuffer->resize(args...); }
	template<typename ...Args> inline auto Clear(Args... args) { return defaultbuffer->Clear(args...); }
	template<typename ...Args> inline auto Rectangle(Args... args) { return defaultbuffer->Rectangle(args...); }
	template<typename ...Args> inline auto Ellipse(Args... args) { return defaultbuffer->Ellipse(args...); }
	template<typename ...Args> inline auto Circle(Args... args) { return defaultbuffer->Circle(args...); }
	template<typename ...Args> inline auto Polygon(Args... args) { return defaultbuffer->Polygon(args...); }
	template<typename ...Args> inline auto Triangle(Args... args) { return defaultbuffer->Triangle(args...); }
	template<typename ...Args> inline auto DrawLine(Args... args) { return defaultbuffer->DrawLine(args...); }
	template<typename ...Args> inline auto ref(Args... args) { return defaultbuffer->ref(args...); }
	template<typename ...Args> inline auto wndmsgBox(Args... args) { return defaultbuffer->getwnd()->msgBox(args...); }
}

#endif
