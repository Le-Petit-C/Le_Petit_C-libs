// LPCgdiplusgraphics.cpp : 定义静态库的函数。
//

#include "pch.h"

//bitmap
namespace LPC {
	Gdiplus::Bitmap *cloneGdiplusBitmapFromHBITMAP(HBITMAP hbitmap) {
		Gdiplus::Bitmap bmp(hbitmap, nullptr);
		return bmp.Clone(0, 0, bmp.GetWidth(), bmp.GetHeight(), PixelFormat24bppRGB);
	}
	//实验发现GDI+的Bitmap类中的克隆只支持克隆出自己或自己的一部分，这个函数可以不论大小克隆出一个新的Bitmap
	Gdiplus::Bitmap *cloneGdiplusBitmapRegardlessOfSize(Gdiplus::Bitmap *bmp, const Gdiplus::Rect& rect, COLORREF backgroundColor) {
		if (rect.X >= 0 && rect.Y >= 0 && rect.GetRight() <= (INT)bmp->GetWidth() && rect.GetBottom() <= (INT)bmp->GetHeight()) {
			return bmp->Clone(rect, PixelFormat24bppRGB);
		}
		else {
			HBITMAP hbitmap;
			bmp->GetHBITMAP(Gdiplus::Color(backgroundColor), &hbitmap);
			HDC hdc = GetDC(nullptr);
			HDC memdc = CreateCompatibleDC(hdc);
			HBITMAP oldbitmap = (HBITMAP)SelectObject(memdc, hbitmap);
			HBITMAP newbitmap = CreateCompatibleBitmap(hdc, rect.Width, rect.Height);
			HDC newmemdc = CreateCompatibleDC(hdc);
			HBITMAP oldnewbitmap = (HBITMAP)SelectObject(newmemdc, newbitmap);
			HBRUSH brush = CreateSolidBrush(backgroundColor);
			brush = (HBRUSH)SelectObject(newmemdc, brush);
			Rectangle(newmemdc, 0, 0, rect.Width, rect.Height);
			BitBlt(newmemdc, -rect.X, -rect.Y, bmp->GetWidth(), bmp->GetHeight(), memdc, 0, 0, SRCCOPY);
			DeleteObject(SelectObject(newmemdc, brush));
			DeleteObject(SelectObject(memdc, oldbitmap));
			SelectObject(newmemdc, oldnewbitmap);
			DeleteDC(newmemdc);
			DeleteDC(memdc);
			ReleaseDC(nullptr, hdc);
			Gdiplus::Bitmap *ret = cloneGdiplusBitmapFromHBITMAP(newbitmap);
			DeleteObject(newbitmap);
			return ret;
		}
	}
	gdiplusBitmap::gdiplusBitmap(int width, int height, Gdiplus::PixelFormat format) :
		bmp(new Gdiplus::Bitmap(width, height, format)),
		brush(new Gdiplus::SolidBrush(Gdiplus::Color(0))),
		pen(new Gdiplus::Pen(Gdiplus::Color(0))),
		grp(new Gdiplus::Graphics(bmp.get())){}

	gdiplusBitmap::gdiplusBitmap(HBITMAP hbitmap) :
		bmp(cloneGdiplusBitmapFromHBITMAP(hbitmap)),
		brush(new Gdiplus::SolidBrush(Gdiplus::Color(0))),
		pen(new Gdiplus::Pen(Gdiplus::Color(0))),
		grp(new Gdiplus::Graphics(bmp.get())) {}

	gdiplusBitmap::gdiplusBitmap(const gdiplusBitmap &bmp, Gdiplus::Rect rect) :
		bmp(cloneGdiplusBitmapRegardlessOfSize(bmp.bmp.get(), rect, 0)),
		brush(new Gdiplus::SolidBrush(Gdiplus::Color(0))),
		pen(new Gdiplus::Pen(Gdiplus::Color(0))),
		grp(new Gdiplus::Graphics(this->bmp.get())) {}

	gdiplusBitmap::gdiplusBitmap(const gdiplusBitmap &bmp, int x, int y, int w, int h) :
		bmp(cloneGdiplusBitmapRegardlessOfSize(bmp.bmp.get(), Gdiplus::Rect(x, y, w, h), 0)),
		brush(new Gdiplus::SolidBrush(Gdiplus::Color(0))),
		pen(new Gdiplus::Pen(Gdiplus::Color(0))),
		grp(new Gdiplus::Graphics(this->bmp.get())) {}

	int gdiplusBitmap::getWidth() {
		return bmp->GetWidth();
	}

	int gdiplusBitmap::getHeight() {
		return bmp->GetHeight();
	}

	void gdiplusBitmap::resize(int width, int height) {
		Gdiplus::PixelFormat format = bmp->GetPixelFormat();
		delete grp.release();
		delete bmp.release();
		bmp.reset(new Gdiplus::Bitmap(width, height, format));
		grp.reset(new Gdiplus::Graphics(bmp.get()));
	}

	void gdiplusBitmap::Clear(Gdiplus::Color color) {
		grp->Clear(color);
	}

	void gdiplusBitmap::SetBrushColor(Gdiplus::Color color) {
		brush->SetColor(color);
	}

	void gdiplusBitmap::SetPenColor(Gdiplus::Color color) {
		pen->SetColor(color);
	}

	void gdiplusBitmap::FillRectangle(const Gdiplus::Rect &rect) {
		grp->FillRectangle(brush.get(), rect);
	}
	
	void gdiplusBitmap::FillPolygon(const Gdiplus::Point *points, int count) {
		grp->FillPolygon(brush.get(), points, count);
	}

	void gdiplusBitmap::FillEllipse(const Gdiplus::Rect &rect) {
		grp->FillEllipse(brush.get(), rect);
	}

	void gdiplusBitmap::DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2) {
		grp->DrawLine(pen.get(), p1, p2);
	}

	void gdiplusBitmap::toHDC(HDC hdc, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
		if (bmp->GetPixelFormat() & PixelFormatAlpha) {
			Gdiplus::Graphics grp(hdc);
			grp.DrawImage(bmp.get(), dstPoint.X, dstPoint.Y, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, Gdiplus::UnitPixel);
		}
		else {
			HDC memdc = CreateCompatibleDC(hdc);
			HBITMAP hbitmap;
			bmp->GetHBITMAP(Gdiplus::Color(0), &hbitmap);
			HBITMAP oldbitmap = (HBITMAP)SelectObject(memdc, hbitmap);
			BitBlt(hdc, dstPoint.X, dstPoint.Y, srcRect.Width, srcRect.Height, memdc, srcRect.X, srcRect.Y, SRCCOPY);
			SelectObject(memdc, oldbitmap);
			DeleteObject(hbitmap);
			DeleteDC(memdc);
		}
	}

	void gdiplusBitmap::toWindow(HWND hwnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint) {
		if (bmp->GetPixelFormat() & PixelFormatAlpha) {
			Gdiplus::Graphics grp(hwnd);
			grp.DrawImage(bmp.get(), dstPoint.X, dstPoint.Y, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, Gdiplus::UnitPixel);
		}
		else {
			HDC hdc = GetDC(hwnd);
			HDC memdc = CreateCompatibleDC(hdc);
			HBITMAP hbitmap;
			bmp->GetHBITMAP(Gdiplus::Color(0), &hbitmap);
			HBITMAP oldbitmap = (HBITMAP)SelectObject(memdc, hbitmap);
			BitBlt(hdc, dstPoint.X, dstPoint.Y, srcRect.Width, srcRect.Height, memdc, srcRect.X, srcRect.Y, SRCCOPY);
			SelectObject(memdc, oldbitmap);
			DeleteObject(hbitmap);
			DeleteDC(memdc);
			ReleaseDC(hwnd, hdc);
		}
	}
}
