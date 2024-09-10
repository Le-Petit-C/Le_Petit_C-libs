

#include <memory>
#include "LPCgraphics.hpp"
#include "LPCdebug.h"
#include "LPCwnd.hpp"

HDC LPC::wndbuffer::initfunc(window& _wnd) {
    wnd = &_wnd;
    HDC hdcwnd = GetDC(*wnd);
    hdc = CreateCompatibleDC(hdcwnd);
    width = wnd->getClientWidth();
    height = wnd->getClientHeight();
    bufbmp = CreateCompatibleBitmap(hdcwnd, width, height);
    ReleaseDC(*wnd, hdcwnd);
    oldbmp = (HBITMAP)SelectObject(hdc, bufbmp);
    oldpen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
    oldbrush = (HBRUSH)SelectObject(hdc, GetStockObject(DC_BRUSH));
    return hdc;
}

LPC::wndbuffer::wndbuffer(window& _wnd) :
    Graphics(initfunc(_wnd)), SolidBrush(0) , regbr(0){}

LPC::wndbuffer::wndbuffer(window* _wnd) :
    Graphics(initfunc(*_wnd)), SolidBrush(0) , regbr(0){}

LPC::wndbuffer::~wndbuffer() {
    DeleteObject(SelectObject(hdc, oldbrush));
    DeleteObject(SelectObject(hdc, oldpen));
    DeleteObject(SelectObject(hdc, oldbmp));
}

void LPC::wndbuffer::resize(int w, int h) {
    width = w, height = h;
    HDC newdc = CreateCompatibleDC(hdc);
    bufbmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP newoldbmp = (HBITMAP)SelectObject(newdc, bufbmp);
    //ReleaseHDC(hdc);
    this->Graphics::~Graphics();
    this->Graphics::Graphics(newdc);
    DeleteObject(SelectObject(hdc, oldbmp));
    oldbmp = newoldbmp;
    oldbrush = (HBRUSH)SelectObject(hdc, oldbrush);
    oldbrush = (HBRUSH)SelectObject(newdc, oldbrush);
    oldpen = (HPEN)SelectObject(hdc, oldpen);
    oldpen = (HPEN)SelectObject(newdc, oldpen);
    DeleteDC(hdc);
    hdc = newdc;
    //Gdiplus::Graphics newgraphics(hdc);
    //(Gdiplus::Graphics&)*this = std::move(newgraphics);
    //this->Graphics::ReleaseHDC(hdc);
    //FromHDC(nullptr);
}

void LPC::wndbuffer::ref() {
    HDC hdcwnd = GetDC(*wnd);
    this->Flush(Gdiplus::FlushIntentionSync);
    BitBlt(hdcwnd, 0, 0, wnd->getClientWidth(), wnd->getClientHeight(), hdc, 0, 0, SRCCOPY);
    ReleaseDC(*wnd, hdc);
}
