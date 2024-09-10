
#include <LPCgamewnd.h>
#include <LPCcomplex.h>
#include <vector>

using namespace LPC;

std::vector<std::vector<int>> buf;

int width = 10, height = 10;
RECT maprect;//地图在屏幕上的矩形区域
int reserved_area = 2;//地图角落处的预留区域

void setmaprect() {
	/*
	解方程:
	left = (SCRW - x * width) / 2
	top = (SCRH - x * height) / 2
	right = (SCRW + x * width) / 2
	bottom = (SCRH + x * height) / 2
	left * top = reserved_area * x * x
	*/

	// (wh-4r)x^2-(Wh+wH)x+WH=0
	double a = width * height - 4 * reserved_area;
	double neg_b = SCRW * height + SCRH * width;
	double c = SCRW * SCRH;
	double x = (neg_b - sqrt(neg_b * neg_b - 4 * a * c)) * 0.5 / a;
	maprect.left = (SCRW - x * width) / 2;
	maprect.top = (SCRH - x * height) / 2;
	maprect.right = (SCRW + x * width) / 2;
	maprect.bottom = (SCRH + x * height) / 2;
}

void init() {

}

int event(unsigned int msg, intptr_t p1, intptr_t p2) {

}
