
#ifndef LPCMessages_h
#define LPCMessages_h

#include <windows.h>
#include <string>
#include "LPCdebug.h"

namespace LPC {
	//��Ϣ������
	typedef VOID(CALLBACK* EVENTPROC)(UINT, WPARAM, LPARAM);
	//ע��һ�������ڵ���Ϣ������ֵΪʵ����Ϣֵ������msg����mapidΪӳ�䵽����Ϣid������getEventMapId(msg)��ȡ����ֵ
	//PostThreadMessageʱ����msg������mapid
	//��Ŀ���߳�����ʹ��LPCDispatchMessage�滻DispatchMessage�������㲻��ʹ�ûص�����
	//��֧��ע��һ����Ϣ
	//proc����Ϊnullptr,��ʱ��ʹ�ûص�����
	UINT registerEvent(const char* description, UINT mapid, EVENTPROC proc);
	//�ǵð���Ϣѭ���е�DispatchMessage�ĳ�����������������̲߳������ҵ���Ϣ����
	//�Լ��ҵ������Ϣ���ƻ�ռ��WM_USER֮�������ֵ���������ӵ�ռ�ã�
	//����һ���߳�ǧ��Ҫͬʱʹ���ҵ���Ϣ���ջ��ƺͽ����Լ��ĳ�����Ϣ
	LRESULT LPCDispatchMessage(const MSG* lpMsg);
	//��ȡmsgӳ�䵽���Զ�����Ϣid����Ϊ�˷���switch case��Ƶ�
	UINT getEventMapId(UINT msg);//δ����
	const char* getEventMapDescription(UINT msg);
}

#endif
