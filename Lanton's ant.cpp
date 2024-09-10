
#define LPCgamewndtitle L"Le_Petit_C的兰顿蚂蚁"

#include <LPCgamewnd.h>
#include <LPCcomplex.h>

#define BLKSLEN 32

using namespace LPC;

constexpr const complex<int> i(0, 1);

enum {
	UP = 1,
	DOWN
};

enum {
	LEFT = 1,
	RIGHT
};

struct block_t;
struct map_t;

struct pixel {
	bool& left, & top, & right, & bottom;
	uint8_t& b;
	pixel(bool& _left, bool& _top, bool& _right, bool& _bottom, uint8_t& _b)
		: left(_left), top(_top), right(_right), bottom(_bottom), b(_b) {}
	uint8_t& operator=(uint8_t v) { return b = v; }
	operator uint8_t&() { return b; }
	void enable(const complex<int>& d) {
		if (d._Re == 1) right = 1;
		if (d._Re == -1) left = 1;
		if (d._Im == 1) bottom = 1;
		if (d._Im == -1) top = 1;
	}
};

using mapdata_t = std::unordered_map<int, std::unordered_map<int, block_t*>>;

struct map_t : public mapdata_t{
	using mapdata_t::operator[];
	template<typename T>
	block_t& operator[](complex<T> index);
};

map_t map;
double stretch;
constexpr const int ncolor = 2;
block_t* currentblock;
static complex<int> blockpos = 0, position = 0, direction(0, -1);
static const complex<int> turns[2] = { complex<int>(0, -1) , complex<int>(0, 1) };
COLORREF colors[2] = { 0xffffff, 0x000000 };
double midx, midy;
struct {
	BITMAPINFOHEADER header = { 0 };
	COLORREF colors[2] = { 0xffffff, 0x000000 };
	operator BITMAPINFO* () { return (BITMAPINFO*)this; }
} info;

static void mytriangle(complex<double> pos, complex<double> rotate, COLORREF color) {
	rotate *= stretch * 0.3125;
	complex<double> pt[3] = { 1.0 * rotate + pos, (-0.5 + sqrt(0.75) * i) * rotate + pos, (-0.5 - sqrt(0.75) * i) * rotate + pos};
	POINT p[3];
	for (int a = 0; a < 3; ++a) {
		p[a].x = (int)pt[a]._Re;
		p[a].y = (int)pt[a]._Im;
	}
	Triangle(p, color);
}

bool painttriangles = false;

struct block_t {
	array2D<uint8_t, BLKSLEN, BLKSLEN> state = { 0 };
	array2D<bool, BLKSLEN, BLKSLEN>axisx = { 0 }, axisy = { 0 };
	block_t* left = nullptr, * top = nullptr;
	complex<int> blockpos;
	block_t(complex<int> pos) : blockpos(pos) {}
	template<typename T>
	pixel operator[](LPC::complex<T> index) {
		if (left == nullptr) left = &map[blockpos - 1];
		if (top == nullptr) top = &map[blockpos - i];
		return pixel(index._Re > 0 ? axisx[index - 1] : left->axisx[index + BLKSLEN - 1], index._Im > 0 ? axisy[index - i] : top->axisy[index + (BLKSLEN - 1) * i], axisx[index], axisy[index], state[index]);
	}
	void paint() const{
		int limx = blockpos._Re * BLKSLEN;
		int limy = blockpos._Im * BLKSLEN;
		int limx2 = limx + BLKSLEN;
		int limy2 = limy + BLKSLEN;
		int w = (int)(limx2 * stretch + midx) - (int)(limx * stretch + midx);
		int h = (int)(limy2 * stretch + midy) - (int)(limy * stretch + midy);
		StretchDIBits(*getDefaultBuffer(), (int)(limx * stretch + midx), (int)(limy * stretch + midy), w, h, 0, 0, BLKSLEN, BLKSLEN, &state, info, DIB_RGB_COLORS, SRCCOPY);
	}
	void painttriangles() {
		bool* p1 = axisx[0], * p2 = axisy[0];
		complex<double> pos;
		double hs = stretch * 0.5;
		pos._Im = blockpos._Im * BLKSLEN * stretch + midy + hs;
		for (int a = 0; a < BLKSLEN; ++a) {
			pos._Re = blockpos._Re * BLKSLEN * stretch + midx + hs;
			for (int b = 0; b < BLKSLEN; ++b) {
				if (*p1) {
					mytriangle(pos + hs, ((a + b) & 1) == 0 ? 1 : -1, 0x007fff);
				}
				if (*p2) {
					mytriangle(pos + hs * i, ((a + b) & 1) == 0 ? -i : i, 0x007fff);
				}
				++p1;
				++p2;
				pos._Re += stretch;
			}
			pos._Im += stretch;
		}
	}
};

template<typename T>
block_t& map_t::operator[](complex<T> index) {
	block_t*& p = (*this)[index._Im][index._Re];
	if (p == nullptr) p = new block_t(index);
	return *p;
}

static void timerproc1();
static timer<> _timer(1000 / 60, timerproc1, false);

static void paint() {
	Clear(0x7f7f7f);
	double s = stretch * BLKSLEN;
	int minx = (int)std::floor(-midx / s);
	int miny = (int)std::floor(-midy / s);
	int maxx = (int)std::ceil((SCRW - midx) / s);
	int maxy = (int)std::ceil((SCRH - midy) / s);
	for (int a = miny; a < maxy; ++a) {
		auto& col = map[a];
		for (int b = minx; b < maxx; ++b) {
			auto*& pblock = col[b];
			if (pblock == nullptr) pblock = new block_t(b + a * i);
			pblock->paint();
		}
	}
	if (painttriangles) {
		for (int a = miny; a < maxy; ++a) {
			auto& col = map[a];
			for (int b = minx; b < maxx; ++b) {
				auto*& pblock = col[b];
				if (pblock == nullptr) pblock = new block_t(b + a * i);
				pblock->painttriangles();
			}
		}
	}
	mytriangle((position + blockpos * BLKSLEN + (0.5 + 0.5 * i)) * stretch + midx + midy * i, direction, 0xff7f00);
	ref();
	return;
}

static void testposition() {
	if (position._Re < 0) {
		position += BLKSLEN;
		blockpos -= 1;
		currentblock = &map[blockpos];
	}
	else if (position._Re >= BLKSLEN) {
		position -= BLKSLEN;
		blockpos += 1;
		currentblock = &map[blockpos];
	}
	else if (position._Im < 0) {
		position += BLKSLEN * i;
		blockpos -= i;
		currentblock = &map[blockpos];
	}
	else if (position._Im >= BLKSLEN) {
		position -= BLKSLEN * i;
		blockpos += i;
		currentblock = &map[blockpos];
	}
}

static double speed = 20;
static clock_t lastt = 0;
static bool stepmode = true;

static void step() {
	pixel n = (*currentblock)[position];
	if (stepmode) {
		direction *= turns[n];
		if (++n >= ncolor) n = 0;
	}
	else {
		n.enable(direction);
		position += direction;
		testposition();
	}
	stepmode = !stepmode;
}

static void timerproc1() {
	static double a = 0, lim = 0;
	clock_t thist = clock();
	if (lastt != 0) lim += speed * (thist - lastt) / CLOCKS_PER_SEC;
	lastt = thist;
	while(a < lim) {
		step();
		++a;
	}
	paint();
}

int event(unsigned int type, intptr_t p1, intptr_t p2) {
	static bool downed = false, rightdowned = false;
	static complex<int> lastRx;
	static int lastx, lasty;
	static int lastadddown = 0;
	switch (type) {
	case WD_SIZE:
		paint();
		break;
	case KY_DOWN:
		switch (p1) {
		case VK_SPACE:
			_timer.change();
			lastt = clock();
			break;
		case VK_RETURN:
			painttriangles = !painttriangles;
			paint();
			break;
		case VK_RIGHT:
			speed *= 1.1;
			break;
		case VK_LEFT:
			speed /= 1.1;
			break;
		case VK_OEM_PLUS:
			step();
			if(lastadddown != 0) step();
			if(++lastadddown > 1) lastadddown = 2;
			paint();
			break;
		}
		break;
	case KY_UP:
		switch (p1) {
		case VK_OEM_PLUS:
			if(lastadddown == 2 && !stepmode) step();
			lastadddown = 0;
			paint();
			break;
		}
		break;
	case MS_WHEEL:
		{
			double p = pow(1.1, GetWheel());
			stretch *= p;
			if (stretch <= 1) {
				p /= stretch;
				stretch = 1;
			}
			midx = p1 + (midx - p1) * p;
			midy = p2 + (midy - p2) * p;
			paint();
		}
		break;
	case MS_MOVE:
		if (downed) {
			midx += p1 - lastx;
			midy += p2 - lasty;
			lastx = (int)p1;
			lasty = (int)p2;
			paint();
		}
		if (rightdowned)
			goto RDOWN;
		break;
	case MS_DOWN:
		downed = true;
		lastx = (int)p1;
		lasty = (int)p2;
		break;
	case MS_UP:
		downed = false;
		break;
	case MS_RDOWN:
	RDOWN:
		rightdowned = true;
		{
			complex<int> pos((int)std::floor((p1 - midx) / stretch) , (int)std::floor((p2 - midy) / stretch));
			if (type == MS_MOVE && pos == lastRx) break;
			lastRx = pos;
			complex<int> pos2 = pos / BLKSLEN;
			pos -= pos2 * BLKSLEN;
			while (pos._Re < 0) {
				pos._Re += BLKSLEN;
				--pos2._Re;
			}
			while (pos._Im < 0) {
				pos._Im += BLKSLEN;
				--pos2._Im;
			}
			uint8_t& n = map[pos2][pos];
			if (++n >= ncolor) n = 0;
		}
		paint();
		break;
	case MS_RUP:
		rightdowned = false;
		break;
	}
	return 0;
}

void init() {

	auto& h = info.header;
	h.biSize = sizeof(BITMAPINFOHEADER);
	h.biWidth = BLKSLEN;
	h.biHeight = -BLKSLEN;
	h.biPlanes = 1;
	h.biBitCount = 8;
	h.biCompression = BI_RGB;
	h.biClrUsed = 2;
	h.biClrImportant = 2;

	currentblock = map[0][0] = new block_t(0);
	stretch = sqrt((double)(SCRW * SCRW + SCRH * SCRH)) * 0.0625;
	midx = (SCRW - stretch) * 0.5;
	midy = (SCRH - stretch) * 0.5;
	LPC::wndmsgBox("按空格继续/暂停\n鼠标左键可以拖动，滚轮缩放\n鼠标右键更改格子颜色\n按回车显示/隐藏方向提示\n按加号键让蚂蚁一步一步走\n初始时默认速度时10Hz", "操作提示");
	paint();
}
