// LPCgraphicsBase.cpp : 定义静态库的函数。
//

#include "pch.h"

void LPC::Bitmap::resize(int width, int height) {
	#ifdef NDEBUG
	throw std::exception("This class does not have resize method.");
	#else
	SetException("This class does not have resize method.");
	#endif
}

void LPC::Bitmap::SetBrushColor(Gdiplus::Color) {
#ifdef NDEBUG
	throw std::exception("This class does not have SetBrushColor method.");
#else
	SetException("This class does not have SetBrushColor method.");
#endif
}

void LPC::Bitmap::SetBrushColor(uint8_t r, uint8_t g, uint8_t b) {
	SetBrushColor(Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::SetBrushColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	SetBrushColor(Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::SetPenColor(Gdiplus::Color) {
#ifdef NDEBUG
	throw std::exception("This class does not have SetPenColor method.");
#else
	SetException("This class does not have SetPenColor method.");
#endif
}

void LPC::Bitmap::SetPenColor(uint8_t r, uint8_t g, uint8_t b) {
	SetPenColor(Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::SetPenColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	SetPenColor(Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::Clear(Gdiplus::Color color) {
#ifdef NDEBUG
	throw std::exception("This class does not have Clear method.");
#else
	SetException("This class does not have Clear method.");
#endif
}

void LPC::Bitmap::Clear(uint32_t color) {
	Clear(Gdiplus::Color(color | 0xff000000));
}

void LPC::Bitmap::Clear(uint8_t r, uint8_t g, uint8_t b) {
	Clear(Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillRectangle(const Gdiplus::Rect& rect) {
#ifdef NDEBUG
	throw std::exception("This class does not have FillRectangle method.");
#else
	SetException("This class does not have FillRectangle method.");
#endif
}

void LPC::Bitmap::FillRectangle(int x, int y, int w, int h) {
	Gdiplus::Rect rect(x, y, w, h);
	FillRectangle(rect);
}

void LPC::Bitmap::FillRectangle(const Gdiplus::Rect &rect, Gdiplus::Color color) {
	SetBrushColor(color);
	FillRectangle(rect);
}

void LPC::Bitmap::FillRectangle(int x, int y, int w, int h, Gdiplus::Color color) {
	FillRectangle(Gdiplus::Rect(x, y, w, h), color);
}

void LPC::Bitmap::FillRectangle(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b) {
	FillRectangle(rect, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
	FillRectangle(Gdiplus::Rect(x, y, w, h), Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillRectangle(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillRectangle(rect, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillRectangle(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillRectangle(Gdiplus::Rect(x, y, w, h), Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillPolygon(const Gdiplus::Point *points, int count) {
#ifdef NDEBUG
	throw std::exception("This class does not have FillPolygon method.");
#else
	SetException("This class does not have FillPolygon method.");
#endif
}

void LPC::Bitmap::FillPolygon(const POINT *points, int count) {
	Gdiplus::Point *gdiplusPoints = new Gdiplus::Point[count];
	for (int i = 0; i < count; i++) {
		gdiplusPoints[i].X = points[i].x;
		gdiplusPoints[i].Y = points[i].y;
	}
	FillPolygon(gdiplusPoints, count);
	delete[] gdiplusPoints;
}

void LPC::Bitmap::FillPolygon(const Gdiplus::Point *points, int count, Gdiplus::Color color) {
	SetBrushColor(color);
	FillPolygon(points, count);
}

void LPC::Bitmap::FillPolygon(const POINT *points, int count, Gdiplus::Color color) {
	SetBrushColor(color);
	FillPolygon(points, count);
}

void LPC::Bitmap::FillPolygon(const Gdiplus::Point *points, int count, uint8_t r, uint8_t g, uint8_t b) {
	FillPolygon(points, count, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillPolygon(const POINT *points, int count, uint8_t r, uint8_t g, uint8_t b) {
	FillPolygon(points, count, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillPolygon(const Gdiplus::Point *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillPolygon(points, count, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillPolygon(const POINT *points, int count, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillPolygon(points, count, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillEllipse(const Gdiplus::Rect &rect) {
#ifdef NDEBUG
	throw std::exception("This class does not have FillEllipse method.");
#else
	SetException("This class does not have FillEllipse method.");
#endif
}

void LPC::Bitmap::FillEllipse(int x, int y, int w, int h) {
	FillEllipse(Gdiplus::Rect(x, y, w, h));
}

void LPC::Bitmap::FillEllipse(const Gdiplus::Rect &rect, Gdiplus::Color color) {
	SetBrushColor(color);
	FillEllipse(rect);
}

void LPC::Bitmap::FillEllipse(int x, int y, int w, int h, Gdiplus::Color color) {
	FillEllipse(Gdiplus::Rect(x, y, w, h), color);
}

void LPC::Bitmap::FillEllipse(const Gdiplus::Rect &rect, uint8_t r, uint8_t g, uint8_t b) {
	FillEllipse(rect, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillEllipse(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
	FillEllipse(Gdiplus::Rect(x, y, w, h), Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillEllipse(const Gdiplus::Rect &rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillEllipse(rect, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillEllipse(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillEllipse(Gdiplus::Rect(x, y, w, h), Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::DrawLine(const Gdiplus::Point& p1, const Gdiplus::Point& p2) {
#ifdef NDEBUG
	throw std::exception("This class does not have DrawLine method.");
#else
	SetException("This class does not have DrawLine method.");
#endif
}

void LPC::Bitmap::DrawLine(int x1, int y1, int x2, int y2) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2);
	DrawLine(p1, p2);
}

void LPC::Bitmap::DrawLine(const Gdiplus::Point& p1, const Gdiplus::Point& p2, Gdiplus::Color color) {
	SetPenColor(color);
	DrawLine(p1, p2, color);
}

void LPC::Bitmap::DrawLine(int x1, int y1, int x2, int y2, Gdiplus::Color color) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2);
	DrawLine(p1, p2, color);
}

void LPC::Bitmap::DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t r, uint8_t g, uint8_t b) {
	DrawLine(p1, p2, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2);
	DrawLine(p1, p2, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	DrawLine(p1, p2, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::DrawLine(int x1, int y1, int x2, int y2, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2);
	DrawLine(p1, p2, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::toHDC(HDC hdc, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
#ifdef NDEBUG
	throw std::exception("This class does not have toHDC method.");
#else
	SetException("This class does not have toHDC method.");
#endif
}

void LPC::Bitmap::toHDC(HDC hdc, int x, int y, int w, int h, int dx, int dy) {
	toHDC(hdc, Gdiplus::Rect(x, y, w, h), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toHDC(HDC hdc, const Gdiplus::Point &dstPoint) {
	toHDC(hdc, Gdiplus::Rect(0, 0, getWidth(), getHeight()), dstPoint);
}

void LPC::Bitmap::toHDC(HDC hdc, int dx, int dy) {
	toHDC(hdc, Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toHDC(HDC hdc) {
	toHDC(hdc, Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(0, 0));
}

void LPC::Bitmap::toWindow(HWND hwnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
	HDC hdc = GetDC(hwnd);
	toHDC(hdc, srcRect, dstPoint);
	ReleaseDC(hwnd, hdc);
}

void LPC::Bitmap::toWindow(const window &wnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
	toWindow(wnd.gethwnd(), srcRect, dstPoint);
}

void LPC::Bitmap::toWindow(const window *wnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
	toWindow(wnd->gethwnd(), srcRect, dstPoint);
}

void LPC::Bitmap::toWindow(HWND hwnd, int x, int y, int w, int h, int dx, int dy) {
	toWindow(hwnd, Gdiplus::Rect(x, y, w, h), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(const window &wnd, int x, int y, int w, int h, int dx, int dy) {
	toWindow(wnd.gethwnd(), Gdiplus::Rect(x, y, w, h), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(const window *wnd, int x, int y, int w, int h, int dx, int dy) {
	toWindow(wnd->gethwnd(), Gdiplus::Rect(x, y, w, h), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(HWND hwnd, const Gdiplus::Point &dstPoint) {
	toWindow(hwnd, Gdiplus::Rect(0, 0, getWidth(), getHeight()), dstPoint);
}

void LPC::Bitmap::toWindow(const window &wnd, const Gdiplus::Point &dstPoint) {
	toWindow(wnd.gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), dstPoint);
}

void LPC::Bitmap::toWindow(const window *wnd, const Gdiplus::Point &dstPoint) {
	toWindow(wnd->gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), dstPoint);
}

void LPC::Bitmap::toWindow(HWND hwnd, int dx, int dy) {
	toWindow(hwnd, Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(const window &wnd, int dx, int dy) {
	toWindow(wnd.gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(const window *wnd, int dx, int dy) {
	toWindow(wnd->gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(dx, dy));
}

void LPC::Bitmap::toWindow(HWND hwnd) {
	toWindow(hwnd, Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(0, 0));
}

void LPC::Bitmap::toWindow(const window &wnd) {
	toWindow(wnd.gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(0, 0));
}

void LPC::Bitmap::toWindow(const window *wnd) {
	toWindow(wnd->gethwnd(), Gdiplus::Rect(0, 0, getWidth(), getHeight()), Gdiplus::Point(0, 0));
}

void LPC::Bitmap::FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3) {
	Gdiplus::Point points[3] = { p1, p2, p3 };
	FillPolygon(points, 3);
}

void LPC::Bitmap::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2), p3(x3, y3);
	FillTriangle(p1, p2, p3);
}

void LPC::Bitmap::FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, Gdiplus::Color color) {
	SetBrushColor(color);
	FillTriangle(p1, p2, p3);
}

void LPC::Bitmap::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Gdiplus::Color color) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2), p3(x3, y3);
	FillTriangle(p1, p2, p3, color);
}

void LPC::Bitmap::FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t r, uint8_t g, uint8_t b) {
	FillTriangle(p1, p2, p3, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2), p3(x3, y3);
	FillTriangle(p1, p2, p3, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillTriangle(const Gdiplus::Point &p1, const Gdiplus::Point &p2, const Gdiplus::Point &p3, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillTriangle(p1, p2, p3, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	Gdiplus::Point p1(x1, y1), p2(x2, y2), p3(x3, y3);
	FillTriangle(p1, p2, p3, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillCircle(const Gdiplus::Point &center, int radius) {
	FillEllipse(Gdiplus::Rect(center.X - radius, center.Y - radius, 2 * radius, 2 * radius));
}

void LPC::Bitmap::FillCircle(int x, int y, int radius) {
	FillCircle(Gdiplus::Point(x, y), radius);
}

void LPC::Bitmap::FillCircle(const Gdiplus::Point &center, int radius, Gdiplus::Color color) {
	SetBrushColor(color);
	FillCircle(center, radius);
}

void LPC::Bitmap::FillCircle(int x, int y, int radius, Gdiplus::Color color) {
	FillCircle(Gdiplus::Point(x, y), radius, color);
}

void LPC::Bitmap::FillCircle(const Gdiplus::Point &center, int radius, uint8_t r, uint8_t g, uint8_t b) {
	FillCircle(center, radius, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillCircle(int x, int y, int radius, uint8_t r, uint8_t g, uint8_t b) {
	FillCircle(x, y, radius, Gdiplus::Color(r, g, b));
}

void LPC::Bitmap::FillCircle(const Gdiplus::Point &center, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillCircle(center, radius, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::FillCircle(int x, int y, int radius, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	FillCircle(x, y, radius, Gdiplus::Color(a, r, g, b));
}

void LPC::Bitmap::thickLine(int x1, int y1, int x2, int y2, int thickness, Gdiplus::Color color) {
	if (x1 == x2 && y1 == y2) {
		FillCircle(x1, y1, thickness / 2, color);
		return;
	}
	double dx, dy, d;
	dx = x2 - x1;
	dy = y2 - y1;
	d = sqrt(dx * dx + dy * dy);
	dx = dx / d * thickness / 2;
	dy = dy / d * thickness / 2;
	Gdiplus::Point points[4] = {
		Gdiplus::Point((int)(x1 + dy), (int)(y1 - dx)),
		Gdiplus::Point((int)(x1 - dy), (int)(y1 + dx)),
		Gdiplus::Point((int)(x2 - dy), (int)(y2 + dx)),
		Gdiplus::Point((int)(x2 + dy), (int)(y2 - dx))
	};
	FillPolygon(points, 4, color);
	FillCircle(x1, y1, thickness / 2, color);
	FillCircle(x2, y2, thickness / 2, color);
}
