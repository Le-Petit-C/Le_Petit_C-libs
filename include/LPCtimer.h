
#ifndef LPCtimer_h
#define LPCtimer_h

#include <windows.h>

namespace LPC {
	class timer {
		HWND hwnd;
		UINT_PTR id;
	public:
		timer(HWND hwnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
		~timer();
	};
}

#ifndef _LIB

namespace LPC {
	namespace priv {
		std::unordered_map<UINT_PTR, void*> timerusermap;
	}
	template<typename... Args>
	class timerex {
	public:
		using func_t = void(Args...);
	protected:
		std::function<void(Args...)> func;
		UINT_PTR TimerId;
		static VOID CALLBACK TimerFunc(HWND hwnd, UINT uMsg, UINT_PTR nIDEvent, DWORD dwTime) {
			((timerex<Args...>*)priv::timerusermap[nIDEvent])->func();
		}
	public:
		timerex(UINT uElapse, func_t fp, Args... args) {
			func = std::bind(fp, std::forward<Args>(args)...);
			TimerId = SetTimer(0, 0, uElapse, TimerFunc);
			priv::timerusermap[TimerId] = this;
		}
		~timerex() {
			KillTimer(0, TimerId);
			priv::timerusermap.erase(TimerId);
		}
	};
}

#endif

#endif
