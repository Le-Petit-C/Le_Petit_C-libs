
#ifndef LPCwndex_h
#define LPCwndex_h

#include "LPCwnd.h"
#include "LPCgdiplusgraphics.h"

#ifndef _LIB
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\LPCwndex.lib")
		#else
			#pragma comment(lib, "..\\Release\\LPCwndex.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\LPCwndex.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\LPCwndex.lib")
		#endif
	#endif
#endif

namespace LPC {
	//��һ����������ΪĬ�ϴ��ڲ�����һ����ͼ������
	class setDefaultWindow {
		gdiplusBitmap *lastbuffer;
		window *lastwindow;
		bool shoulddeletewindow;//�Ƿ�Ӧ��ɾ������
		bool shoulddeletebuffer;//�Ƿ�Ӧ��ɾ�������������ڿ���û���ã��ƺ�ʼ��Ϊtrue
	public:
		setDefaultWindow(const setDefaultWindow &) = delete;
		setDefaultWindow(window *wnd, bool resetPaintProc = true, bool resetSizeProc = true);
		setDefaultWindow(window &wnd, bool resetPaintProc = true, bool resetSizeProc = true);
		setDefaultWindow(window &&wnd, bool resetPaintProc = true, bool resetSizeProc = true);
		~setDefaultWindow();
		void reset(window *wnd, bool resetPaintProc = true, bool resetSizeProc = true);
		void reset(window &wnd, bool resetPaintProc = true, bool resetSizeProc = true);
		void reset(window &&wnd, bool resetPaintProc = true, bool resetSizeProc = true);
	private:
		void set(window *wnd, bool resetPaintProc = true, bool resetSizeProc = true);//������Ĭ�ϴ��ںʹ�����������������should delete
	};
	//����һ�����ڲ���������ΪĬ�ϴ���
	class defaultWindow : public window, public setDefaultWindow{
	public:
		defaultWindow();
		defaultWindow(const char *title);
		defaultWindow(const char *title, int clientWidth, int clientHeight);
		defaultWindow(const char *title, int x, int y, int clientWidth, int clientHeight);
		defaultWindow(const char *title, RECT clientRect);
		defaultWindow(const wchar_t *title);
		defaultWindow(const wchar_t *title, int clientWidth, int clientHeight);
		defaultWindow(const wchar_t *title, int x, int y, int clientWidth, int clientHeight);
		defaultWindow(const wchar_t *title, RECT clientRect);
		defaultWindow(specificWindowStyles style);
		defaultWindow(const defaultWindow &) = delete;
		defaultWindow(defaultWindow &&) = delete;
	};
	enum resizeProcMode {
		ALIGN_MIDDLE = 0,
		ALIGN_LEFT = 1,
		ALIGN_TOP = 2,
		ALIGN_LEFT_TOP = 3,
		ALIGN_RIGHT = 4,
		STRETCH_X = 5,
		ALIGN_RIGHT_TOP = 6,
		ALIGN_TOP_STRETCH_X = 7,
		ALIGN_BOTTOM = 8,
		ALIGN_LEFT_BOTTOM = 9,
		STRETCH_Y = 10,
		ALIGN_LEFT_STRETCH_Y = 11,
		ALIGN_RIGHT_BOTTOM = 12,
		ALIGN_BOTTOM_STRETCH_X = 13,
		ALIGN_RIGHT_STRETCH_Y = 14,
		STRETCH_XY = 15
	};
	inline resizeProcMode setResizeProcMode(resizeProcMode mode);
	inline int getScreenWidth();
	inline int getScreenHeight();
#ifndef LPCNOMACRO
#define SCRW (getScreenWidth())
#define SCRH (getScreenHeight())
#else//������ú궨�壬��ô�������Դﵽ���Ƶ�Ч��������������������������printf�޷���ȷ����ֵ��
	struct { inline operator int() const { return getScreenWidth(); } } SCRW;
	struct { inline operator int() const { return getScreenHeight(); } } SCRH;
#endif
	inline gdiplusBitmap *getDefaultBuffer();
	inline window *getDefaultWindow();
	inline LPC::dexcoc waitForDestroy();
	inline MOUSEPROC setMouseProc(MOUSEPROC proc);
	inline KEYPROC setKeyProc(KEYPROC proc);
	inline WNDPROC setDefaultProc(WNDPROC proc);
	inline BOOL setTitle(const char *title);
	void resizeWindow(int width, int height);//���贰�ڴ�С����Ӧ�����ô��ڻ�������С
	void resizeClient(int width, int height);//���贰�ڿͻ�����С����Ӧ�����ô��ڻ�������С
	inline void fullScreen() { getDefaultWindow()->fullScreen(); }//ȫ������
	inline void maxShow() { getDefaultWindow()->maxShow(); }//��󻯴���
	inline void restore() { getDefaultWindow()->restore(); }//�ָ�Ĭ�ϴ��ڴ�С����ʽ
	inline bool isFullScreen() { return getDefaultWindow()->isFullScreen(); }//�Ƿ�ȫ��
	//��ͼ����
	void ref();//ˢ�´���
	void refsize();//���ݴ��ڿͻ�����С���ã�ˢ�£���������С
	//����
	inline void Clear(Gdiplus::Color color) { getDefaultBuffer()->Clear(color); }
	inline void Clear(uint32_t color) { getDefaultBuffer()->Clear(color); }
	inline void Clear(uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->Clear(r, g, b); }
	//���ʡ���ˢ
	inline void SetBrushColor(Gdiplus::Color color) { getDefaultBuffer()->SetBrushColor(color); }
	inline void SetBrushColor(uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->SetBrushColor(r, g, b); }
	inline void SetBrushColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->SetBrushColor(a, r, g, b); }
	inline void SetPenColor(Gdiplus::Color color) { getDefaultBuffer()->SetPenColor(color); }
	inline void SetPenColor(uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->SetPenColor(r, g, b); }
	inline void SetPenColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->SetPenColor(a, r, g, b); }
	//�����߿�ƽ�еľ�������
	inline void FillRectangle(const Gdiplus::Rect &rect) { getDefaultBuffer()->FillRectangle(rect); }
	inline void FillRectangle(int x, int y, int w, int h) { getDefaultBuffer()->FillRectangle(x, y, w, h); }
	inline void FillRectangle(const Gdiplus::Rect &rect, Gdiplus::Color color) { getDefaultBuffer()->FillRectangle(rect, color); }
	inline void FillRectangle(int x, int y, int w, int h, Gdiplus::Color color) { getDefaultBuffer()->FillRectangle(x, y, w, h, color); }
	inline void FillRectangle(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillRectangle(rect, r, g, b); }
	inline void FillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillRectangle(x, y, w, h, r, g, b); }
	inline void FillRectangle(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillRectangle(rect, a, r, g, b); }
	inline void FillRectangle(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillRectangle(x, y, w, h, a, r, g, b); }
	//�����������
	inline void FillPolygon(const Gdiplus::Point *points, int count) { getDefaultBuffer()->FillPolygon(points, count); }
	inline void FillPolygon(const POINT *points, int count) { getDefaultBuffer()->FillPolygon(points, count); }
	inline void FillPolygon(const Gdiplus::Point *points, int count, Gdiplus::Color color) { getDefaultBuffer()->FillPolygon(points, count, color); }
	inline void FillPolygon(const POINT *points, int count, Gdiplus::Color color) { getDefaultBuffer()->FillPolygon(points, count, color); }
	inline void FillPolygon(const Gdiplus::Point *points, int count, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillPolygon(points, count, r, g, b); }
	inline void FillPolygon(const POINT *points, int count, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillPolygon(points, count, r, g, b); }
	inline void FillPolygon(const Gdiplus::Point *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillPolygon(points, count, a, r, g, b); }
	inline void FillPolygon(const POINT *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillPolygon(points, count, a, r, g, b); }
	//��䳤������߿�ƽ�е���Բ����
	inline void FillEllipse(const Gdiplus::Rect &rect) { getDefaultBuffer()->FillEllipse(rect); }
	inline void FillEllipse(int x, int y, int w, int h) { getDefaultBuffer()->FillEllipse(x, y, w, h); }
	inline void FillEllipse(const Gdiplus::Rect &rect, Gdiplus::Color color) { getDefaultBuffer()->FillEllipse(rect, color); }
	inline void FillEllipse(int x, int y, int w, int h, Gdiplus::Color color) { getDefaultBuffer()->FillEllipse(x, y, w, h, color); }
	inline void FillEllipse(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillEllipse(rect, r, g, b); }
	inline void FillEllipse(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillEllipse(x, y, w, h, r, g, b); }
	inline void FillEllipse(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillEllipse(rect, a, r, g, b); }
	inline void FillEllipse(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillEllipse(x, y, w, h, a, r, g, b); }
	//����
	inline void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2) { getDefaultBuffer()->DrawLine(p1, p2); }
	inline void DrawLine(int x1, int y1, int x2, int y2) { getDefaultBuffer()->DrawLine(x1, y1, x2, y2); }
	inline void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, Gdiplus::Color color) { getDefaultBuffer()->DrawLine(p1, p2, color); }
	inline void DrawLine(int x1, int y1, int x2, int y2, Gdiplus::Color color) { getDefaultBuffer()->DrawLine(x1, y1, x2, y2, color); }
	inline void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->DrawLine(p1, p2, r, g, b); }
	inline void DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->DrawLine(x1, y1, x2, y2, r, g, b); }
	inline void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->DrawLine(p1, p2, a, r, g, b); }
	inline void DrawLine(int x1, int y1, int x2, int y2, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->DrawLine(x1, y1, x2, y2, a, r, g, b); }
	//�������������
	inline void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3) { getDefaultBuffer()->FillTriangle(p1, p2, p3); }
	inline void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) { getDefaultBuffer()->FillTriangle(x1, y1, x2, y2, x3, y3); }
	inline void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, Gdiplus::Color color) { getDefaultBuffer()->FillTriangle(p1, p2, p3, color); }
	inline void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Gdiplus::Color color) { getDefaultBuffer()->FillTriangle(x1, y1, x2, y2, x3, y3, color); }
	inline void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillTriangle(p1, p2, p3, r, g, b); }
	inline void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillTriangle(x1, y1, x2, y2, x3, y3, r, g, b); }
	inline void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillTriangle(p1, p2, p3, a, r, g, b); }
	inline void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillTriangle(x1, y1, x2, y2, x3, y3, a, r, g, b); }
	//���Բ������
	inline void FillCircle(const Gdiplus::Point &center, int radius) { getDefaultBuffer()->FillCircle(center, radius); }
	inline void FillCircle(int x, int y, int radius) { getDefaultBuffer()->FillCircle(x, y, radius); }
	inline void FillCircle(const Gdiplus::Point &center, int radius, Gdiplus::Color color) { getDefaultBuffer()->FillCircle(center, radius, color); }
	inline void FillCircle(int x, int y, int radius, Gdiplus::Color color) { getDefaultBuffer()->FillCircle(x, y, radius, color); }
	inline void FillCircle(const Gdiplus::Point &center, int radius, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillCircle(center, radius, r, g, b); }
	inline void FillCircle(int x, int y, int radius, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillCircle(x, y, radius, r, g, b); }
	inline void FillCircle(const Gdiplus::Point &center, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillCircle(center, radius, a, r, g, b); }
	inline void FillCircle(int x, int y, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { getDefaultBuffer()->FillCircle(x, y, radius, a, r, g, b); }
	//��һ��������ϸ����ɫ����
	inline void thickLine(int x1, int y1, int x2, int y2, int thickness, Gdiplus::Color color) { getDefaultBuffer()->thickLine(x1, y1, x2, y2, thickness, color); }
	//template<typename ...Args> inline auto wndmsgBox(Args... args) { return getDefaultBuffer()->getwnd()->msgBox(args...); }
}

namespace LPC {
	extern gdiplusBitmap *defaultbuffer;
	extern window *defaultwindow;
	extern resizeProcMode resizeMode;
	inline resizeProcMode setResizeProcMode(resizeProcMode mode) {
		resizeProcMode lastMode = resizeMode;
		resizeMode = mode;
		return lastMode;
	}
	inline int getScreenWidth() {
		return getDefaultBuffer()->getWidth();
	}
	inline int getScreenHeight() {
		return getDefaultBuffer()->getHeight();
	}
	inline gdiplusBitmap *getDefaultBuffer() { return defaultbuffer; }
	inline window *getDefaultWindow() { return defaultwindow; }
	inline LPC::dexcoc waitForDestroy() { return defaultwindow->waitForDestroy(); }
	inline BOOL setTitle(const char *title) { return defaultwindow->setTitle(title); }
	inline MOUSEPROC setMouseProc(MOUSEPROC proc) { return defaultwindow->setMouseProc(proc); }
	inline KEYPROC setKeyProc(KEYPROC proc) { return defaultwindow->setKeyProc(proc); }
	inline PAINTPROC setPaintProc(PAINTPROC proc) { return defaultwindow->setPaintProc(proc); }
	inline WNDPROC setDefaultProc(WNDPROC proc) { return defaultwindow->setDefaultProc(proc); }
}

#endif
