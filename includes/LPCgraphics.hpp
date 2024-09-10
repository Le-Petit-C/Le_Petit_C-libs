
#ifndef LPCgraphics_h
#define LPCgraphics_h

#include <windows.h>
#include <time.h>
#include <gdiplus.h>
#include "LPCdebug.h"
#include "LPCwnd.hpp"

namespace LPC {
	class unique_dc;
	class wndbuffer;
}

class LPC::unique_dc {
public:
	inline unique_dc(HDC _hdc) : hdc(CreateCompatibleDC(_hdc)){}
	inline ~unique_dc() { DeleteDC(hdc); }
protected:
	HDC hdc = nullptr;
	inline unique_dc() {}
};

class LPC::wndbuffer : private unique_dc, public Gdiplus::Graphics , public Gdiplus::SolidBrush {
public:
	wndbuffer(window*);
	wndbuffer(window&);
	~wndbuffer();
	void ref();
	inline operator HDC() { return hdc; }
	inline window* getwnd() { return wnd; }
	void resize(int w, int h);
	inline void resize() { resize(wnd->getClientWidth(), wnd->getClientHeight()); }
	inline int getwidth() const { return width; }
	inline int getheight() const { return height; }
	/***********************************************************************************************/
	using Graphics::FillRectangle;
	inline auto FillRectangle(const RECT& rect) { return FillRectangle(this, (int)rect.left, (int)rect.top, (int)(rect.right - rect.left), (int)(rect.bottom - rect.top)); }
	inline auto FillRectangle(const RECT& rect, COLORREF color) {
		SetDCBrushColor(hdc, color);
		return ::Rectangle(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
	}
	inline auto FillRectangle(int x, int y, int w, int h, COLORREF color) {
		SetDCBrushColor(hdc, color);
		return ::Rectangle(hdc, x, y, x + w + 1, y + h + 1);
	}
	inline auto FillRectangle(const Gdiplus::Rect& rect, COLORREF color) { return FillRectangle(rect.X, rect.Y, rect.Width, rect.Height, color); }
	inline auto FillRectangle(const RECT& rect, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(rect, RGB(r, g, b)); }
	inline auto FillRectangle(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(x, y, w, h, RGB(r, g, b)); }
	inline auto FillRectangle(const Gdiplus::Rect& rect, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(rect.X, rect.Y, rect.Width, rect.Height, RGB(r, g, b)); }
	inline auto FillRectangle(int x, int y, int w, int h, const Gdiplus::Color& color) {
		regbr.SetColor(color);
		return FillRectangle(&regbr, x, y, w, h);
	}
	inline auto FillRectangle(const RECT &rect, const Gdiplus::Color& color) {
		regbr.SetColor(color);
		return FillRectangle(&regbr, (int)rect.left, (int)rect.top, (int)(rect.right - rect.left), (int)(rect.bottom - rect.top));
	}
	inline auto FillRectangle(const Gdiplus::Rect& rect, Gdiplus::Color color) { return FillRectangle(rect.X, rect.Y, rect.Width, rect.Height, color); }
	inline auto FillRectangle(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(Gdiplus::Rect(x, y, w, h), Gdiplus::Color(a, r, g, b)); }
	inline auto FillRectangle(const RECT& rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(rect, Gdiplus::Color(a, r, g, b)); }
	inline auto FillRectangle(const Gdiplus::Rect& rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillRectangle(rect, Gdiplus::Color(a, r, g, b)); }
	template<typename ...Args> inline auto Rectangle(Args... args) -> decltype(FillRectangle(this, args...)) { return FillRectangle(this, args...); }
	template<typename ...Args> inline auto Rectangle(Args... args) -> decltype(FillRectangle(args...)) { return FillRectangle(args...); }
	/***********************************************************************************************/
	using Graphics::FillEllipse;
	inline auto FillEllipse(const RECT& rect) { return FillEllipse(this, (int)rect.left, (int)rect.top, (int)(rect.right - rect.left), (int)(rect.bottom - rect.top)); }
	inline auto FillEllipse(int x, int y, int w, int h, COLORREF color) {
		SetDCBrushColor(hdc, color);
		return ::Ellipse(hdc, x, y, x + w + 1, y + h + 1);
	}
	inline auto FillEllipse(const RECT& rect, COLORREF color) {
		SetDCBrushColor(hdc, color);
		return ::Ellipse(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
	}
	inline auto FillEllipse(const Gdiplus::Rect& rect, COLORREF color) { return FillEllipse(rect.X, rect.Y, rect.Width, rect.Height, color); }
	inline auto FillEllipse(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(x, y, w, h, RGB(r, g, b)); }
	inline auto FillEllipse(const RECT& rect, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(rect, RGB(r, g, b)); }
	inline auto FillEllipse(const Gdiplus::Rect& rect, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(rect, RGB(r, g, b)); }
	inline auto FillEllipse(int x, int y, int w, int h, Gdiplus::Color color) {
		regbr.SetColor(color);
		return FillEllipse(&regbr, x, y, w, h);
	}
	inline auto FillEllipse(const RECT& rect, Gdiplus::Color color) {
		regbr.SetColor(color);
		return FillEllipse(&regbr, (int)rect.left, (int)(rect.top), (int)(rect.right - rect.left), (int)(rect.bottom - rect.top));
	}
	inline auto FillEllipse(const Gdiplus::Rect& rect, Gdiplus::Color color) { return FillEllipse(rect.X, rect.Y, rect.Width, rect.Height, color); }
	inline auto FillEllipse(const RECT& rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(rect, Gdiplus::Color(a, r, g, b)); }
	inline auto FillEllipse(int x, int y, int w, int h, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(x, y, w, h, Gdiplus::Color(a, r, g, b)); }
	inline auto FillEllipse(const Gdiplus::Rect& rect, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return FillEllipse(rect, Gdiplus::Color(a, r, g, b)); }
	template<typename ...Args> inline auto Ellipse(Args... args) -> decltype(FillEllipse(this, std::forward<Args>(args)...)) { return FillEllipse(this, std::forward<Args>(args)...); }
	template<typename ...Args> inline auto Ellipse(Args... args) -> decltype(FillEllipse(std::forward<Args>(args)...)) { return FillEllipse(std::forward<Args>(args)...); }
	/***********************************************************************************************/
	inline void Circle(int x, int y, int R) { Ellipse(x - R, y - R, 2 * R, 2 * R); }
	inline void Circle(int x, int y, int R, COLORREF color) { Ellipse(x - R, y - R, 2 * R, 2 * R, color); }
	inline void Circle(int x, int y, int R, uint8_t r, uint8_t g, uint8_t b) { Ellipse(x - R, y - R, 2 * R, 2 * R, r, g, b); }
	inline void Circle(int x, int y, int R, Gdiplus::Color color) { Ellipse(x - R, y - R, 2 * R, 2 * R, color); }
	inline void Circle(int x, int y, int R, uint8_t a, uint8_t r, uint8_t g, uint8_t b) { Ellipse(x - R, y - R, 2 * R, 2 * R, a, r, g, b); }
	/***********************************************************************************************/
	using Graphics::Clear;
	inline auto Clear(uint8_t r, uint8_t g, uint8_t b) { return Clear(Gdiplus::Color(r, g, b)); }
	inline auto Clear(COLORREF color) { return Clear(Gdiplus::Color(color | 0xff000000)); }
	/***********************************************************************************************/
	using Graphics::FillPolygon;
	inline auto FillPolygon(const Gdiplus::Point* points, int count, Gdiplus::Color color) {
		this->SetColor(color);
		FillPolygon(this, points, count);
	}
	inline auto FillPolygon(const POINT* points, int count, COLORREF color) {
		SetDCBrushColor(hdc, color);
		::Polygon(hdc, points, count);
	}
	template<typename = std::enable_if_t<sizeof(Gdiplus::Point) == sizeof(POINT)>> inline auto FillPolygon(const POINT* points, int count) { FillPolygon(this, (const Gdiplus::Point*)points, count); }
	template<typename = std::enable_if_t<sizeof(Gdiplus::Point) == sizeof(POINT)>> inline auto FillPolygon(const POINT* points, int count, Gdiplus::Color color) { FillPolygon((const Gdiplus::Point*)points, count, color); }
	template<typename = std::enable_if_t<sizeof(Gdiplus::Point) == sizeof(POINT)>> inline auto FillPolygon(const Gdiplus::Point* points, int count, COLORREF color) { FillPolygon((const POINT*)points, count, color); }
	template<typename ...Args> inline auto Polygon(Args... args) -> decltype(FillPolygon(this, std::forward<Args>(args)...)) { return FillPolygon(this, std::forward<Args>(args)...); }
	template<typename ...Args> inline auto Polygon(Args... args) -> decltype(FillPolygon(std::forward<Args>(args)...)) { return FillPolygon(std::forward<Args>(args)...); }
	template<typename T1, typename T2> inline auto Triangle(T1 arg1, T2 arg2) { return Polygon(arg1, 3, arg2); }
	template<typename T1> inline auto Triangle(T1 arg1) { return Polygon(arg1, 3); }
	/***********************************************************************************************/
	inline auto MoveToEx(int x, int y, LPPOINT pt = nullptr) { ::MoveToEx(hdc, x, y, pt); }
	inline auto LineTo(int x, int y) { ::LineTo(hdc, x, y); }
	inline auto DrawLine(int x1, int y1, int x2, int y2, COLORREF color) {
		HGDIOBJ obj = SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, color);
		MoveToEx(x1, y1);
		LineTo(x2, y2);
		SelectObject(hdc, obj);
	}
	/***********************************************************************************************/
private:
	HBITMAP bufbmp;
	HBITMAP oldbmp;
	HBRUSH oldbrush;
	HPEN oldpen;
	window* wnd;
	Gdiplus::SolidBrush regbr;
	HDC initfunc(window& wnd);
	int width, height;
};

#endif
