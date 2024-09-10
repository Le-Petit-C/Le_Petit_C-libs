
#ifndef LPCgamewnd_h
#define LPCgamewnd_h

#include "LPCgamewnd.hpp"
#include "LPCwnd.h"
#include "LPCgraphics.h"
#include "LPCwinmain.h"

//需要用户自定义的函数:

//接收比较常见的事件
//返回值非0表示已经处理完毕消息，函数库不再对本次消息进行后续消息分发。
//注:对于WND_MESSAGE消息，不保证一定在init被调用后才接收到，其他消息一定在init被调用后才接收到
int event(unsigned int type, intptr_t p1, intptr_t p2);

//入口函数，或者是程序开始时会调用一次的函数
void init();

namespace LPC {
	//获取消息循环中的结构体指针，无需释放
	inline const MSG* GetMSG();
}

#include "LPCgamewnd.inc"

#endif
