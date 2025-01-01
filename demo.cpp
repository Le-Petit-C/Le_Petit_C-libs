
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
	case WM_MOUSEMOVE://鼠标移动事件
		if (lastx != 0 || lasty != 0)
			thickLine(lastx, lasty, x, y, 16, colors[colorIndex]);//画一个粗线段
		lastx = x;
		lasty = y;
		ref();//刷新窗口
		break;
	case WM_LBUTTONDOWN://鼠标左键按下事件
		colorIndex = (colorIndex + 1) % colorCount;//循环设置颜色
		titleIndex = (titleIndex + 1) % titleCount;//循环设置标题
		setTitle(titles[titleIndex]);//设置窗口标题
		//只是为了好玩
		break;
	}
}

void keyProc(bool type, int key) {
	if (type) {
		switch (key) {
		case VK_ESCAPE://ESC键
			if (isFullScreen())
				restore();//恢复默认窗口大小和样式
			break;
		case VK_RETURN://回车键
			if (isFullScreen())
				restore();//恢复默认窗口大小和样式
			else
				fullScreen();//全屏
			break;
		}
	}
}

/*LRESULT CALLBACK myproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_SIZE://窗口大小改变事件
		refsize();//根据窗口客户区大小设置（刷新）缓冲区大小
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}*/

int main(int argc, char** argv) {
	defaultWindow wnd("Test Window");					//创建一个窗口
	Clear(255, 255, 255);								//清空窗口
	FillCircle(SCRW / 2, SCRH / 2, 100, 63, 63, 255);	//画一个圆
	FillRectangle(0, 0, SCRW / 2, SCRH / 2, 255, 0, 0);	//画一个矩形
	ref();												//刷新窗口
	wnd.setMouseProc(mouseProc);						//设置鼠标回调函数
	wnd.setKeyProc(keyProc);							//设置键盘回调函数
	//wnd.setDefaultProc(myproc);						//设置default回调函数
	//dprintf("测试\n");								//开启调试控制台并输出一行文字
	waitForDestroy();									//进入消息循环并等待窗口销毁
	return 0;
}
