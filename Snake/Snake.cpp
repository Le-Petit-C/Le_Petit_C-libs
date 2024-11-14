
#include <LPCgamewnd.h>
#include <LPCcomplex.h>
#include "Map.h"
#include "ScreenRect.h"

using namespace LPC;

RECT maprect;//��ͼ����Ļ�ϵľ�������
int reserved_area = 2;//��ͼ���䴦��Ԥ������
Map* map;//�洢��ͼ���ݵĽṹ
ScreenRect* rect;

COLORREF getColor(int mapdata) {
	switch (mapdata) {
	case 0:
		return 0xbfbfbf;
	case -1:
		return 0xffff7f;
	default:
		return 0x7f7f7f;
	}
}

static void paint() {
	Clear(0xffffff);
	auto end = rect->end();
	int edgew = std::sqrt(SCRW * SCRH) / 256;
	for (auto it = rect->beginex(); it != end; ++it) {
		Rectangle(it->left, it->top, it->right - it->left, it->bottom - it->top, getColor(*it));
		Rectangle(it->left, it->top, edgew, it->bottom - it->top, 0x000000);
		Rectangle(it->right - edgew, it->top, edgew, it->bottom - it->top, 0x000000);
		Rectangle(it->left, it->top, it->right - it->left, edgew, 0x000000);
		Rectangle(it->left, it->bottom - edgew, it->right - it->left, edgew, 0x000000);
	}
	//Rectangle(0, 0, 100, 100, 0xff00ff);
	ref();
}

static void ftimer() {
	paint();
}

void init() {
	map = new Map(11, 11);
	rect = new ScreenRect(*getDefaultWindow(), *map, 2);
	paint();
}

int event(unsigned int msg, intptr_t p1, intptr_t p2) {
	switch (msg) {
	case WD_SIZE:
		rect->ref();
		paint();
		break;

	}
	return 0;
}
