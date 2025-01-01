#pragma once

#include <LPCwnd.h>
#include <gdiplus.h>

#ifndef _LIB
	#pragma comment(lib, "gdiplus.lib")
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\LPCgraphicsBase.lib")
		#else
			#pragma comment(lib, "..\\Release\\LPCgraphicsBase.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\LPCgraphicsBase.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\LPCgraphicsBase.lib")
		#endif
	#endif
#endif

namespace LPC {
	class Bitmap {
	public:
		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
		virtual void resize(int width, int height);
		/*
		接下来是画图函数
		带*的是必须实现否则调用会抛出异常的
		其他的可以实现但是不实现时会默认转化参数后调用带*的
		*/
		//清屏
		virtual void Clear(Gdiplus::Color color);//*
		virtual void Clear(uint32_t color);
		virtual void Clear(uint8_t r, uint8_t g, uint8_t b);
		//画笔、画刷
		virtual void SetBrushColor(Gdiplus::Color color);//*
		virtual void SetBrushColor(uint8_t r, uint8_t g, uint8_t b);
		virtual void SetBrushColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void SetPenColor(Gdiplus::Color color);//*
		virtual void SetPenColor(uint8_t r, uint8_t g, uint8_t b);
		virtual void SetPenColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//填充与边框平行的矩形区域
		virtual void FillRectangle(const Gdiplus::Rect &rect);//*
		virtual void FillRectangle(int x, int y, int w, int h);
		virtual void FillRectangle(const Gdiplus::Rect& rect, Gdiplus::Color color);
		virtual void FillRectangle(int x, int y, int w, int h, Gdiplus::Color color);
		virtual void FillRectangle(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillRectangle(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillRectangle(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//填充多边形区域
		virtual void FillPolygon(const Gdiplus::Point *points, int count);//*
		virtual void FillPolygon(const POINT *points, int count);
		virtual void FillPolygon(const Gdiplus::Point *points, int count, Gdiplus::Color color);
		virtual void FillPolygon(const POINT *points, int count, Gdiplus::Color color);
		virtual void FillPolygon(const Gdiplus::Point *points, int count, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillPolygon(const POINT *points, int count, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillPolygon(const Gdiplus::Point *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillPolygon(const POINT *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//填充长短轴与边框平行的椭圆区域
		virtual void FillEllipse(const Gdiplus::Rect &rect);//*
		virtual void FillEllipse(int x, int y, int w, int h);
		virtual void FillEllipse(const Gdiplus::Rect &rect, Gdiplus::Color color);
		virtual void FillEllipse(int x, int y, int w, int h, Gdiplus::Color color);
		virtual void FillEllipse(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillEllipse(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillEllipse(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillEllipse(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//画线
		virtual void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2);//*
		virtual void DrawLine(int x1, int y1, int x2, int y2);
		virtual void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, Gdiplus::Color color);
		virtual void DrawLine(int x1, int y1, int x2, int y2, Gdiplus::Color color);
		virtual void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t r, uint8_t g, uint8_t b);
		virtual void DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
		virtual void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void DrawLine(int x1, int y1, int x2, int y2, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//不缩放地绘制到另一个HDC
		virtual void toHDC(HDC hdc, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);//*
		virtual void toHDC(HDC hdc, int x, int y, int w, int h, int dx, int dy);
		virtual void toHDC(HDC hdc, const Gdiplus::Point &dstPoint);
		virtual void toHDC(HDC hdc, int dx, int dy);
		virtual void toHDC(HDC hdc);
		//不缩放地绘制到窗口
		virtual void toWindow(HWND hwnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);
		virtual void toWindow(const window &wnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);
		virtual void toWindow(const window *wnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);
		virtual void toWindow(HWND hwnd, int x, int y, int w, int h, int dx, int dy);
		virtual void toWindow(const window &wnd, int x, int y, int w, int h, int dx, int dy);
		virtual void toWindow(const window *wnd, int x, int y, int w, int h, int dx, int dy);
		virtual void toWindow(HWND hwnd, const Gdiplus::Point &dstPoint);
		virtual void toWindow(const window &wnd, const Gdiplus::Point &dstPoint);
		virtual void toWindow(const window *wnd, const Gdiplus::Point &dstPoint);
		virtual void toWindow(HWND hwnd, int dx, int dy);
		virtual void toWindow(const window &wnd, int dx, int dy);
		virtual void toWindow(const window *wnd, int dx, int dy);
		virtual void toWindow(HWND hwnd);
		virtual void toWindow(const window &wnd);
		virtual void toWindow(const window *wnd);

		//接下来是一些拓展的绘制函数，不能保证所有的实现都是最优的。但是它们都是可以用的。
		//填充三角形区域
		virtual void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		virtual void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, Gdiplus::Color color);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Gdiplus::Color color);
		virtual void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//填充圆形区域
		virtual void FillCircle(const Gdiplus::Point &center, int radius);
		virtual void FillCircle(int x, int y, int radius);
		virtual void FillCircle(const Gdiplus::Point &center, int radius, Gdiplus::Color color);
		virtual void FillCircle(int x, int y, int radius, Gdiplus::Color color);
		virtual void FillCircle(const Gdiplus::Point &center, int radius, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillCircle(int x, int y, int radius, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillCircle(const Gdiplus::Point &center, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		virtual void FillCircle(int x, int y, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
		//画一条给定粗细和颜色的线
		//这个函数如果传入颜色为透明色会出现奇怪的效果
		virtual void thickLine(int x1, int y1, int x2, int y2, int thickness, Gdiplus::Color color);
	};
}
