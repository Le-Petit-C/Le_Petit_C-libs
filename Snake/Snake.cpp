
#define LPCgamewndtitle "Snake"

#include <LPCgamewnd.h>
#include <LPCcomplex.h>
#include <LPCtimer.h>
#include <deque>
#include "Map.h"
#include "ScreenRect.h"

using namespace LPC;

RECT maprect;//地图在屏幕上的矩形区域
int reserved_area = 2;//地图角落处的预留区域
//存储地图数据的结构
static Map& map() {
	static Map map(11, 11);
	return map;
}
ScreenRect* rect;
int direction;
std::deque<int> directionQueue;
int posx, posy;
int tailx, taily;
timerex<>* mytimer;
bool paused;

COLORREF getColor(int mapdata) {
	switch (mapdata) {
	case 0:
		return 0xdfdfdf;
	case -1:
		return 0x7fffff;
	default:
		return 0x3f3f7f;
	}
}

static void paint() {
	Clear(0x7fdf7f);
	auto end = rect->end();
	int edgew = std::sqrt(SCRW * SCRH) / 256;
	for (auto it = rect->beginex(); it != end; ++it)
		Rectangle(it->left, it->top, it->right - it->left, it->bottom - it->top, getColor(*it));
	const RECT& rec = rect->getRect();
	for (size_t a = 0; a <= map().getWidth(); ++a) {
		int x = (rec.left * (map().getWidth() - a) + rec.right * a) / map().getWidth();
		Rectangle(x - edgew, rec.top, edgew << 1, rec.bottom - rec.top, 0x000000);
	}
	for (size_t a = 0; a <= map().getHeight(); ++a) {
		int y = (rec.top * (map().getHeight() - a) + rec.bottom * a) / map().getHeight();
		Rectangle(rec.left, y - edgew, rec.right - rec.left, edgew << 1, 0x000000);
	}
	//Rectangle(0, 0, 100, 100, 0xff00ff);
	ref();
}

void randomfood() {
	while (true) {
		int x = rand() % map().getWidth();
		int y = rand() % map().getHeight();
		if (map()(x, y) == 0) {
			map()(x, y) = -1;
			break;
		}
	}
}

void move_by_direction(int& x, int& y, int d) {
	switch (d) {
	case VK_LEFT:
		--x;
		break;
	case VK_UP:
		--y;
		break;
	case VK_RIGHT:
		++x;
		break;
	case VK_DOWN:
		++y;
		break;
	}
}

static void ftimer() {
	if (paused) return;
	if (!directionQueue.empty()) {
		direction = directionQueue.front();
		directionQueue.pop_front();
	}
	map()(posx, posy) = direction;
	move_by_direction(posx, posy, direction);
	if (posx < 0 || posy < 0 || posx >= map().getWidth() || posy >= map().getHeight()) {
		delete mytimer;
		mytimer = nullptr;
	}
	else {
		switch (map()(posx, posy)) {
		case -1:
			randomfood();
			break;
		case 0:
		{
			int d = map()(tailx, taily);
			map()(tailx, taily) = 0;
			move_by_direction(tailx, taily, d);

		}
			break;
		default:
			delete mytimer;
			mytimer = nullptr;
			break;
		}
		map()(posx, posy) = 1;
	}
	paint();
}

void init() {
	msgBox("按空格暂停/继续\n按方向键/WASD控制方向以及取消暂停", "提示");
	map().clear();
	if (rect) delete rect;
	rect = new ScreenRect(*getDefaultWindow(), map(), 2);
	posx = tailx = map().getWidth() / 2;
	posy = taily = map().getHeight() / 2;
	map()(posx, posy) = 1;
	mytimer = new timerex<>(1000 / 10, ftimer);
	paused = true;
	direction = VK_RIGHT;
	randomfood();
	paint();
}

int event(unsigned int msg, intptr_t p1, intptr_t p2) {
	switch (msg) {
	case WD_SIZE:
		rect->ref();
		paint();
		break;
	case KY_DOWN:
		if (!mytimer) {
			if (p1 == VK_SPACE) init();
			break;
		}
		int lastd;
		if (directionQueue.empty()) lastd = direction;
		else lastd = directionQueue.back();
		switch (p1) {
		case VK_LEFT:
		case 'A':
			paused = false;
			if (lastd != VK_RIGHT)
				directionQueue.push_back(VK_LEFT);
			break;
		case VK_UP:
		case 'W':
			paused = false;
			if (lastd != VK_DOWN)
				directionQueue.push_back(VK_UP);
			break;
		case VK_RIGHT:
		case 'D':
			paused = false;
			if (lastd != VK_LEFT)
				directionQueue.push_back(VK_RIGHT);
			break;
		case VK_DOWN:
		case 'S':
			paused = false;
			if (lastd != VK_UP)
				directionQueue.push_back(VK_DOWN);
			break;
		case VK_SPACE:
			paused = !paused;
			break;
		}
		break;
	}
	return 0;
}
