
#ifndef LPCgamewnd_h
#define LPCgamewnd_h

#include "LPCgamewnd.hpp"
#include "LPCwnd.h"
#include "LPCgraphics.h"
#include "LPCwinmain.h"

//��Ҫ�û��Զ���ĺ���:

//���ձȽϳ������¼�
//����ֵ��0��ʾ�Ѿ����������Ϣ�������ⲻ�ٶԱ�����Ϣ���к�����Ϣ�ַ���
//ע:����WND_MESSAGE��Ϣ������֤һ����init�����ú�Ž��յ���������Ϣһ����init�����ú�Ž��յ�
int event(unsigned int type, intptr_t p1, intptr_t p2);

//��ں����������ǳ���ʼʱ�����һ�εĺ���
void init();

namespace LPC {
	//��ȡ��Ϣѭ���еĽṹ��ָ�룬�����ͷ�
	inline const MSG* GetMSG();
}

#include "LPCgamewnd.inc"

#endif
