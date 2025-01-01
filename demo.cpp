
#include <LPCgraphics_autoinit.h>
#include <LPCwndex.h>
#include <LPCwinmain.h>

using namespace LPC;

const char *titles[] = {
	"Test window",
	"Hello World",
	"Le_Petit_C demo window",
	"Code chan leave a message",
	"This! Is! A! Demo! Window!"
};
static const size_t titleCount = sizeof(titles) / sizeof(titles[0]);
int titleIndex = 0;

const Gdiplus::Color colors[] = {
	Gdiplus::Color(255, 0, 0),
	Gdiplus::Color(255, 255, 0),
	Gdiplus::Color(0, 255, 0),
	Gdiplus::Color(0, 255, 255),
	Gdiplus::Color(0, 0, 255),
	Gdiplus::Color(255, 0, 255)
};
static const size_t colorCount = sizeof(colors) / sizeof(colors[0]);
int colorIndex = 0;

void mouseProc(int type, int x, int y, int wheel) {
	switch (type) {
		static int lastx, lasty;
	case WM_MOUSEMOVE://����ƶ��¼�
		if (lastx != 0 || lasty != 0)
			thickLine(lastx, lasty, x, y, 16, colors[colorIndex]);//��һ�����߶�
		lastx = x;
		lasty = y;
		ref();//ˢ�´���
		break;
	case WM_LBUTTONDOWN://�����������¼�
		colorIndex = (colorIndex + 1) % colorCount;//ѭ��������ɫ
		titleIndex = (titleIndex + 1) % titleCount;//ѭ�����ñ���
		setTitle(titles[titleIndex]);//���ô��ڱ���
		//ֻ��Ϊ�˺���
		break;
	}
}

void keyProc(bool type, int key) {
	if (type) {
		switch (key) {
		case VK_ESCAPE://ESC��
			if (isFullScreen())
				restore();//�ָ�Ĭ�ϴ��ڴ�С����ʽ
			break;
		case VK_RETURN://�س���
			if (isFullScreen())
				restore();//�ָ�Ĭ�ϴ��ڴ�С����ʽ
			else
				fullScreen();//ȫ��
			break;
		}
	}
}

/*LRESULT CALLBACK myproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_SIZE://���ڴ�С�ı��¼�
		refsize();//���ݴ��ڿͻ�����С���ã�ˢ�£���������С
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}*/

int main(int argc, char** argv) {
	defaultWindow wnd("Test Window");					//����һ������
	Clear(255, 255, 255);								//��մ���
	FillCircle(SCRW / 2, SCRH / 2, 100, 63, 63, 255);	//��һ��Բ
	FillRectangle(0, 0, SCRW / 2, SCRH / 2, 255, 0, 0);	//��һ������
	ref();												//ˢ�´���
	wnd.setMouseProc(mouseProc);						//�������ص�����
	wnd.setKeyProc(keyProc);							//���ü��̻ص�����
	//wnd.setDefaultProc(myproc);						//����default�ص�����
	//dprintf("����\n");								//�������Կ���̨�����һ������
	waitForDestroy();									//������Ϣѭ�����ȴ���������
	return 0;
}
