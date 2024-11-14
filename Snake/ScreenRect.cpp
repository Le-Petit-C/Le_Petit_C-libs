
#include "ScreenRect.h"

void ScreenRect::ref(){
	/*
	½â·½³Ì:
	left = (SCRW - x * width) / 2
	top = (SCRH - x * height) / 2
	right = (SCRW + x * width) / 2
	bottom = (SCRH + x * height) / 2
	left * top = reserved_area * x * x
	*/

	// (wh-4r)x^2-(Wh+wH)x+WH=0
	size_t width = map->getWidth();
	size_t height = map->getHeight();
	double a = width * height - 4 * reserved_area;
	double neg_b = wnd->getClientWidth() * height + wnd->getClientHeight() * width;
	double c = wnd->getClientWidth() * wnd->getClientHeight();
	double x = (neg_b - sqrt(neg_b * neg_b - 4 * a * c)) * 0.5 / a;
	rect.left = (wnd->getClientWidth() - x * width) / 2;
	rect.top = (wnd->getClientHeight() - x * height) / 2;
	rect.right = (wnd->getClientWidth() + x * width) / 2;
	rect.bottom = (wnd->getClientHeight() + x * height) / 2;

	stepx = (rect.right - rect.left) / map->getWidth();
	resx = (rect.right - rect.left) % map->getWidth();
	stepy = (rect.bottom - rect.top) / map->getHeight();
	resy = (rect.bottom - rect.top) % map->getHeight();
}

ScreenRect::ScreenRect(LPC::window& wnd, Map& map, double reserved_area) {
	this->wnd = &wnd;
	this->map = &map;
	this->reserved_area = reserved_area;
	ref();
}

ScreenRect::rectIter::rectIter(ScreenRect& rect) {
	this->rect = &rect;
	this->out.data = &rect.map->first();
	out.x = out.y = 0;
	out.left = rect.rect.left;
	out.top = rect.rect.top;
	out.right = out.left + rect.stepx;
	out.bottom = out.top + rect.stepy;
	resx = rect.resx;
	resy = rect.resy;
}

ScreenRect::rectIter& ScreenRect::rectIter::operator++() {
	++out.data;
	int xlim = rect->map->getWidth();
	if (++out.x >= xlim) {
		out.x = 0;
		out.left = rect->rect.left;
		out.right = out.left + rect->stepx;
		resx = rect->resx;
		int ylim = rect->map->getHeight();
		++out.y;
		out.top = out.bottom;
		out.bottom += rect->stepy;
		if ((resy += rect->resy) >= ylim) {
			++out.bottom;
			resy -= ylim;
		}
	}
	else {
		out.left = out.right;
		out.right += rect->stepx;
		if ((resx += rect->resx) >= xlim) {
			++out.right;
			resx -= xlim;
		}
	}
	return *this;
}

ScreenRect::rectIter& ScreenRect::rectIter::operator+=(size_t count) {
	out.data += count;
	return *this;
}

ScreenRect::rectIter ScreenRect::beginex() {
	return rectIter(*this);
}

ScreenRect::rectIter ScreenRect::endex() {
	return rectIter(*this) += map->getSize();
}
