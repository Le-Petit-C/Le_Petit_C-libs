
#ifndef LPCgamewnd_h
#define LPCgamewnd_h

#include "LPCwnd.h"
#include "LPCgraphics.h"
#include "LPCwinmain.h"
#include "LPCdebug.h"

#ifndef _LIB

//需要用户自定义的函数:

//接收比较常见的事件
//返回值非0表示已经处理完毕消息，函数库不再对本次消息进行后续消息分发。
//注:对于WND_MESSAGE消息，不保证一定在init被调用后才接收到，其他消息一定在init被调用后才接收到
extern int event(unsigned int type, intptr_t p1, intptr_t p2);

//入口函数，或者是程序开始时会调用一次的函数
extern void init();

#endif

namespace LPC {
	//获取消息循环中的结构体指针，无需释放
	inline const MSG* GetMSG();
}

#include "LPCwndex.h"

namespace LPC {
	//定义event回调函数的样子
	typedef int (*event_t)(unsigned int, intptr_t, intptr_t);
	//设置event回调函数，返回值为上一个event函数指针
	//关于event函数：接收比较常见的事件，返回值非0表示已经处理完毕消息，函数库不再对本次消息进行后续默认处理。
	event_t setevent(event_t event);

	//初始化LPCgamewnd
	void gamewndstartup(event_t event, const wchar_t* title = defaultWindowTitle);
	//释放LPCgamewnd
	void gamewndshutdown();
}

//常见的事件定义及对应参数含义：
enum {
	//空事件，我也不知道这能用来干啥
	LPC_NULLEVENT,
	//Windows标准回调函数收到的事件，此时p1是原wParam，p2是原lParam，可以用GetMSGCode()获取更多信息。
	WND_MESSAGE,
	//窗口大小改变，p1表示新窗口客户区宽度，p2表示新窗口客户区高度，函数库里默认的处理方式是在WND_MESSAGE后WD_SIZE前重设窗口缓冲区大小以适应客户区大小
	WD_SIZE,
	//键盘按键被按下，p1就是原wParam值，表示虚拟键值，p2就是原wParam值，应该不是很常用吧
	KY_DOWN,
	//键盘按键被按下，p1就是原wParam值，表示虚拟键值，p2就是原wParam值，应该不是很常用吧
	KY_UP,
	//WM_CHAR对应消息，没有想到什么好的简化方法，p1就是原wParam值，表示字符值，p2就是原wParam值
	KY_CHAR,
	//鼠标移动，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_MOVE,
	//鼠标左键按下，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_LDOWN,
	MS_DOWN = MS_LDOWN,
	//鼠标左键松开，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_LUP,
	MS_UP = MS_LUP,
	//鼠标右键按下，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_RDOWN,
	//鼠标右键松开，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_RUP,
	//鼠标中键按下，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_MDOWN,
	//鼠标中键松开，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0)
	MS_MUP,
	//鼠标滚轮滚动，(p1,p2)表示此刻鼠标相对于窗口客户区的坐标，取左上为(0,0),用GetWheel()获取滚动了多少格，用GetWheelPixel()获取滚动格子数乘以WHEEL_DELTA的值，向上滚动为正
	MS_WHEEL,
	//最后一个常用事件之后的值
	LPC_AFTERLASTEVENT
};

namespace LPC {
	//在event回调函数中时可以调用这个获取当前Windows回调函数中的message值
	UINT GetMSGCode();
	//处理MS_WHEEL时调用可获取滚轮滚顶的格子数，其他时候返回值未定义
	int GetWheel();
	//处理MS_WHEEL时调用可获取滚轮滚顶的格子数乘以WHEEL_DELTA的值，其他时候返回值未定义
	int GetWheelPixel();
}

namespace LPC {
	namespace priv {

#ifdef _LIB
		const MSG* pmsg;
#else
		extern const MSG* pmsg;
#endif
	}
}


inline const MSG* LPC::GetMSG() {
	return priv::pmsg;
}

#ifndef _LIB

namespace LPC {
	namespace priv {
		struct LPCgamewndinit_t {
			LPCgamewndinit_t();
			~LPCgamewndinit_t();
		} LPCgamewndinit_t_instance;
	}
}

LPC::priv::LPCgamewndinit_t::LPCgamewndinit_t() {
#ifndef LPCgamewndtitle
	gamewndstartup(event);
#else
	gamewndstartup(event, LPCgamewndtitle);
#endif
}

LPC::priv::LPCgamewndinit_t::~LPCgamewndinit_t() {
	gamewndshutdown();
}

int _cdecl main(int argc, char** argv) {
	init();
	MSG msg{};
	LPC::priv::pmsg = &msg;
	BOOL ret;
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			LPC::dprintf("Unexpected error happend, GetMessage returned %d.", ret);
		}
	}
}

#endif

#endif
