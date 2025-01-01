#pragma once

#include "LPCgraphicsBase.h"
#include <memory>
#include <gdiplus.h>
#include <LPCwnd.h>

//Le_Petit_C:感觉或许应该把gdiplusBitmap拆开，一个gdiplusBitmap，另一个gdiplusWindow
//Le_Petit_C:或许还是暂时不整gdiplusWindow了
//Le_Petit_C:感觉有toWindow已经暂时够用了

#ifndef _LIB
	#ifndef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "..\\Debug\\LPCgdiplusgraphics.lib")
		#else
			#pragma comment(lib, "..\\Release\\LPCgdiplusgraphics.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "..\\x64\\Debug\\LPCgdiplusgraphics.lib")
		#else
			#pragma comment(lib, "..\\x64\\Release\\LPCgdiplusgraphics.lib")
		#endif
	#endif
#endif

namespace LPC {
	class LPCgdiplusPixelContainer;
	class LPCgdiplusPixelContainer_bitmap;
	class LPCgdiplusPixelContainer_window;
	class gdiplusBitmap;
}

//bitmap
namespace LPC {
	class gdiplusBitmap : public Bitmap {
	public:
		gdiplusBitmap(int width, int height, Gdiplus::PixelFormat format = PixelFormat32bppARGB);
		gdiplusBitmap(HBITMAP hbitmap);
		gdiplusBitmap(const gdiplusBitmap &bmp, Gdiplus::Rect rect);
		gdiplusBitmap(const gdiplusBitmap &bmp, int x, int y, int w, int h);
		virtual int getWidth() override;
		virtual int getHeight() override;
		virtual void resize(int width, int height) override;
		virtual void Clear(Gdiplus::Color color) override;
		using Bitmap::Clear;
		virtual void SetBrushColor(Gdiplus::Color color) override;
		using Bitmap::SetBrushColor;
		virtual void SetPenColor(Gdiplus::Color color) override;
		using Bitmap::SetPenColor;
		virtual void FillRectangle(const Gdiplus::Rect &rect) override;
		using Bitmap::FillRectangle;
		virtual void FillPolygon(const Gdiplus::Point *points, int count) override;
		using Bitmap::FillPolygon;
		virtual void FillEllipse(const Gdiplus::Rect &rect) override;
		using Bitmap::FillEllipse;
		virtual void DrawLine(const Gdiplus::Point &p1, const Gdiplus::Point &p2) override;
		using Bitmap::DrawLine;
		virtual void toHDC(HDC hdc, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);
		using Bitmap::toHDC;
		virtual void toWindow(HWND hwnd, const Gdiplus::Rect &srcRect, const Gdiplus::Point &dstPoint);
		using Bitmap::toWindow;
		//GDI+的Bitmap专属部分
		inline HDC GetHDC();
		inline void ReleaseHDC(HDC hdc);
		std::unique_ptr<Gdiplus::Bitmap> bmp;
		std::unique_ptr<Gdiplus::SolidBrush> brush;
		std::unique_ptr<Gdiplus::Pen> pen;
		std::unique_ptr<Gdiplus::Graphics> grp;
	};
}

namespace LPC {
	inline HDC gdiplusBitmap::GetHDC() {
		return grp->GetHDC();
	}
	inline void gdiplusBitmap::ReleaseHDC(HDC hdc) {
		return grp->ReleaseHDC(hdc);
	}
}
