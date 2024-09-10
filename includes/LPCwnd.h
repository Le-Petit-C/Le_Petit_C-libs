
#ifndef LPCwnd_h
#define LPCwnd_h

#include "LPCwnd.hpp"

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
