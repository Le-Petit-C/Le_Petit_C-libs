
#include "Map.h"

void Map::resize(size_t w, size_t h) {
	if (buf) delete[] buf;
	if (w == 0 && h == 0) buf = nullptr;
	else buf = new int[w * h];
	width = w;
	height = h;
}
