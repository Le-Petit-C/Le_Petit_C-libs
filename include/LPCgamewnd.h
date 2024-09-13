
#ifndef LPCgamewnd_h
#define LPCgamewnd_h

#include "LPCwnd.h"
#include "LPCgraphics.h"
#include "LPCwinmain.h"
#include "LPCdebug.h"

#ifndef _LIB

//��Ҫ�û��Զ���ĺ���:

//���ձȽϳ������¼�
//����ֵ��0��ʾ�Ѿ����������Ϣ�������ⲻ�ٶԱ�����Ϣ���к�����Ϣ�ַ���
//ע:����WND_MESSAGE��Ϣ������֤һ����init�����ú�Ž��յ���������Ϣһ����init�����ú�Ž��յ�
extern int event(unsigned int type, intptr_t p1, intptr_t p2);

//��ں����������ǳ���ʼʱ�����һ�εĺ���
extern void init();

#endif

namespace LPC {
	//��ȡ��Ϣѭ���еĽṹ��ָ�룬�����ͷ�
	inline const MSG* GetMSG();
}

#include "LPCwndex.h"

namespace LPC {
	//����event�ص�����������
	typedef int (*event_t)(unsigned int, intptr_t, intptr_t);
	//����event�ص�����������ֵΪ��һ��event����ָ��
	//����event���������ձȽϳ������¼�������ֵ��0��ʾ�Ѿ����������Ϣ�������ⲻ�ٶԱ�����Ϣ���к���Ĭ�ϴ���
	event_t setevent(event_t event);

	//��ʼ��LPCgamewnd
	void gamewndstartup(event_t event, const wchar_t* title = defaultWindowTitle);
	//�ͷ�LPCgamewnd
	void gamewndshutdown();
}

//�������¼����弰��Ӧ�������壺
enum {
	//���¼�����Ҳ��֪������������ɶ
	LPC_NULLEVENT,
	//Windows��׼�ص������յ����¼�����ʱp1��ԭwParam��p2��ԭlParam��������GetMSGCode()��ȡ������Ϣ��
	WND_MESSAGE,
	//���ڴ�С�ı䣬p1��ʾ�´��ڿͻ�����ȣ�p2��ʾ�´��ڿͻ����߶ȣ���������Ĭ�ϵĴ���ʽ����WND_MESSAGE��WD_SIZEǰ���贰�ڻ�������С����Ӧ�ͻ�����С
	WD_SIZE,
	//���̰��������£�p1����ԭwParamֵ����ʾ�����ֵ��p2����ԭwParamֵ��Ӧ�ò��Ǻܳ��ð�
	KY_DOWN,
	//���̰��������£�p1����ԭwParamֵ����ʾ�����ֵ��p2����ԭwParamֵ��Ӧ�ò��Ǻܳ��ð�
	KY_UP,
	//WM_CHAR��Ӧ��Ϣ��û���뵽ʲô�õļ򻯷�����p1����ԭwParamֵ����ʾ�ַ�ֵ��p2����ԭwParamֵ
	KY_CHAR,
	//����ƶ���(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_MOVE,
	//���������£�(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_LDOWN,
	MS_DOWN = MS_LDOWN,
	//�������ɿ���(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_LUP,
	MS_UP = MS_LUP,
	//����Ҽ����£�(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_RDOWN,
	//����Ҽ��ɿ���(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_RUP,
	//����м����£�(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_MDOWN,
	//����м��ɿ���(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0)
	MS_MUP,
	//�����ֹ�����(p1,p2)��ʾ�˿��������ڴ��ڿͻ��������꣬ȡ����Ϊ(0,0),��GetWheel()��ȡ�����˶��ٸ���GetWheelPixel()��ȡ��������������WHEEL_DELTA��ֵ�����Ϲ���Ϊ��
	MS_WHEEL,
	//���һ�������¼�֮���ֵ
	LPC_AFTERLASTEVENT
};

namespace LPC {
	//��event�ص�������ʱ���Ե��������ȡ��ǰWindows�ص������е�messageֵ
	UINT GetMSGCode();
	//����MS_WHEELʱ���ÿɻ�ȡ���ֹ����ĸ�����������ʱ�򷵻�ֵδ����
	int GetWheel();
	//����MS_WHEELʱ���ÿɻ�ȡ���ֹ����ĸ���������WHEEL_DELTA��ֵ������ʱ�򷵻�ֵδ����
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
