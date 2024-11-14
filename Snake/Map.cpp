
#include "Map.h"

void Map::resize(size_t w, size_t h) {
	if (buf) delete[] buf;
	if (w == 0 && h == 0) buf = nullptr;
	else buf = new int[w * h]();
	width = w;
	height = h;
}

void Map::clear() {
	int* p1 = buf;
	int* p2 = p1 + width * height;
	while (p1 != p2) {
		*p1 = 0;
		++p1;
	}
}
