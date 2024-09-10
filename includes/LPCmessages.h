
#ifndef LPCMessages_h
#define LPCMessages_h

#include <windows.h>
#include <string>
#include "LPCdebug.h"

namespace LPC {
	//消息处理函数
	typedef VOID(CALLBACK* EVENTPROC)(UINT, WPARAM, LPARAM);
	//注册一个进程内的消息，返回值为实际消息值（记作msg），mapid为映射到的消息id，可用getEventMapId(msg)获取到该值
	//PostThreadMessage时请用msg而不是mapid
	//在目标线程中请使用LPCDispatchMessage替换DispatchMessage，除非你不想使用回调函数
	//不支持注销一个消息
	//proc可以为nullptr,此时不使用回调函数
	UINT registerEvent(const char* description, UINT mapid, EVENTPROC proc);
	//记得把消息循环中的DispatchMessage改成这个，除非你的这个线程不想用我的消息机制
	//以及我的这个消息机制会占用WM_USER之后的所有值（依次增加地占用）
	//所以一个线程千万不要同时使用我的消息接收机制和接收自己的常量消息
	LRESULT LPCDispatchMessage(const MSG* lpMsg);
	//获取msg映射到的自定义消息id，是为了方便switch case设计的
	UINT getEventMapId(UINT msg);//未完善
	const char* getEventMapDescription(UINT msg);
}

#endif
