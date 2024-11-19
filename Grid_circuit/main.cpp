
#include <map>
#include <LPCgamewnd.h>
#include <LPCtimer.h>
#include <LPCcomplex.h>

using namespace LPC;

using pointptr_t = complex<intptr_t>;

using timer_t = timerex<>;
std::unique_ptr<timer_t> timer;
double stretch = 64;//缩放大小，单位是像素，也是单个方块的边长。
const double maxStretch = 256;//最大缩放大小
const double minStretch = 16;//最小缩放大小
double camx = 0, camy = 0;//摄像机在地图中的x,y坐标，对应屏幕坐标原点

//获取屏幕坐标(x,y)对应了哪个位置对象的哪个点
static void scrToPoint(intptr_t x, intptr_t y, pointptr_t* res1, double* resx, double* resy) {
	double sx = x / stretch + camx;
	double sy = y / stretch + camy;
	res1->real() = (intptr_t)sx;
	res1->imag() = (intptr_t)sy;
	*resx = sx - res1->real();
	*resy = sy - res1->imag();
}

class object {
public:
	virtual void queueUpdate() {};//由队列触发的更新
	virtual void stackUpdate() {};//由栈触发的更新
	virtual void paint(const RECT& rect) const = 0;//绘制对象, 应该将自身限制在rect对应的矩形范围内
	//执行一刻的事件
	static void tick() {
		updateList.swap(scheduleList);
		while (!updateList.empty()) {
			pointptr_t pos = updateList.front();
			updateList.pop_front();
			auto o = datamap.find(pos);
			if (o != datamap.end())
				o->second->stackUpdate();
		}
	}
	//绘制整个缓冲区
	static void paint(){
		Clear(0xffffff);
		intptr_t ystart = (intptr_t)std::floor(camy);
		intptr_t yend = (intptr_t)std::ceil(camy + SCRH / stretch);
		intptr_t xstart = (intptr_t)std::floor(camx);
		intptr_t xend = (intptr_t)std::ceil(camx + SCRW / stretch);
		pointptr_t pos;
		for (pos.imag() = ystart; pos.imag() < yend; ++pos.imag()) {
			RECT rect{};
			rect.top = (LONG)((pos.imag() - camy) * stretch);
			rect.bottom = (LONG)((pos.imag() + 1 - camy) * stretch);
			for (pos.real() = xstart; pos.real() < xend; ++pos.real()) {
				rect.left = (LONG)((pos.real() - camx) * stretch);
				rect.right = (LONG)((pos.real() + 1 - camx) * stretch);
				auto o = datamap.find(pos);
				if(o != datamap.end()) o->second->paint(rect);
			}
		}
	}
	//将pos处的对象设置为obj。obj为nullptr时清除对应位置的对象
	static void setblock(pointptr_t pos, object* obj) {
		if (!obj) {
			datamap.erase(pos);
		}
		else {
			obj->pos = pos;
			datamap[pos].reset(obj);
			auto o = datamap.find(pos - 1);
			if (o != datamap.end()) {
				obj->left = o->second.get();
				obj->left->right = obj;
			}
			o = datamap.find(pos - pointptr_t::i);
			if (o != datamap.end()) {
				obj->up = o->second.get();
				obj->up->down = obj;
			}
			o = datamap.find(pos + 1);
			if (o != datamap.end()) {
				obj->right = o->second.get();
				obj->right->left = obj;
			}
			o = datamap.find(pos + pointptr_t::i);
			if (o != datamap.end()) {
				obj->down = o->second.get();
				obj->down->up = obj;
			}
		}
		auto o = datamap.find(pos);
		if (o != datamap.end()) {
			o->second->stackUpdate();
		}
		o = datamap.find(pos - 1);
		if (o != datamap.end()) {
			o->second->stackUpdate();
		}
		o = datamap.find(pos - pointptr_t::i);
		if (o != datamap.end()) {
			o->second->stackUpdate();
		}
		o = datamap.find(pos + 1);
		if (o != datamap.end()) {
			o->second->stackUpdate();
		}
		o = datamap.find(pos + pointptr_t::i);
		if (o != datamap.end()) {
			o->second->stackUpdate();
		}
	}
	~object() {
		if (left) left->right = nullptr;
		if (up) up->down = nullptr;
		if (right) right->left = nullptr;
		if (down) down->up = nullptr;
	}
	//获取对应方向位置的方块，p是相对位置。没有方块或者不相邻就返回nullptr，注意处理
	object* getDirection(pointptr_t p) const {
		switch (p.real()) {
		case -1:
			if (p.imag() == 0)
				return left;
			else return nullptr;
		case 0:
			switch (p.imag()) {
			case -1:
				return up;
			case 1:
				return down;
			default:
				return nullptr;
			}
		case 1:
			if (p.imag() == 0)
				return right;
			else return nullptr;
		default:
			return nullptr;
		}
	}
protected:
	inline const pointptr_t& getPos() const { return pos; }//获取自身坐标
	//将自身加入更新队列中
	void queue() {
		if (inupdate) return;
		updateList.push_back(pos);
		inupdate = true;
	}
	//立刻触发obj对象的栈更新
	static void stack(object* obj) {
		obj->stackUpdate();
	}
	//用栈更新方法更新一阶毗邻内
	void stackUpdateNear() {
		if (left) left->stackUpdate();
		if (up) up->stackUpdate();
		if (right) right->stackUpdate();
		if (down) down->stackUpdate();
	}
	//将自己加入计划刻，下一刻触发队列更新
	void schedule() const {
		scheduleList.push_back(pos);
	}
private:
	object* left = nullptr, * up = nullptr, * right = nullptr, * down = nullptr;
	pointptr_t pos = {};//对象的坐标
	bool inupdate = false;//此刻是否已经在更新队列中了
	static std::map<pointptr_t, std::unique_ptr<object>> datamap;//存储地图数据的map
	static std::deque<pointptr_t> updateList;//此刻的更新队列
	static std::deque<pointptr_t> scheduleList;//计划刻更新队列
};

std::map<pointptr_t, std::unique_ptr<object>> object::datamap;
std::deque<pointptr_t> object::updateList;
std::deque<pointptr_t> object::scheduleList;

class Item {
public:
	//绘制在快捷栏中的样子
	virtual void paint(const RECT&) const = 0;
	//选中了该物品后右键空白格子会调用该函数创建一个对象。pos是格子坐标
	virtual object* build(pointptr_t pos, double x, double y) const = 0;
};

//普通方块
class Block : public object {
public:
	bool can_power() const {
		return can_power_b;
	}
protected:
	Block(bool can_power) : can_power_b(can_power){}
private:
	bool can_power_b = true;//能否被充能
};

class Redstone : public object {
protected:
	enum powerType {
		None,
		Weak_Power,
		Strong_Power
	};
	//获取向对应方向的激活状态
	powerType directionPower(pointptr_t p) const {
		switch (p.real()) {
		case -1:
			if (p.imag() == 0)
				return left;
			else return None;
		case 0:
			switch (p.imag()) {
			case -1:
				return up;
			case 1:
				return down;
			default:
				return None;
			}
		case 1:
			if (p.imag() == 0)
				return right;
			else return None;
		default:
			return None;
		}
	}
	//检测一个obj周围有没有能充能它的红石原件
	static powerType is_powered(object* obj) {
		if (!obj) return None;
		powerType ret = None;
		pointptr_t d = 1;
		do {
			Redstone* objr = dynamic_cast<Redstone*>(obj->getDirection(d));
			if (objr) {
				powerType p = objr->directionPower(-d);
				if (p == Strong_Power) return Strong_Power;
				else if (p == Weak_Power) ret = Weak_Power;
			}
			d *= pointptr_t::i;
		} while (d != (pointptr_t)1);
		return ret;
	}
	//旁边有没有被充能了的方块或者能充能自己这个位置的红石原件
	bool is_next_to_power() {
		pointptr_t d = 1;
		do {
			object* o = getDirection(d);
			if (!o) continue;
			Redstone* o_r = dynamic_cast<Redstone*>(o);
			if (o_r)
				if (o_r->directionPower(-d) != None)
					return true;
			Block* o_b = dynamic_cast<Block*>(o);
			if (o_b)
				if (o_b->can_power() && is_powered(o_b) != None)
					return true;
		} while (d != (pointptr_t)1);
		return false;
	}
	powerType left = None, up = None, right = None, down = None;//是否能激活对应位置方块以及激活种类
};

//最基础的可充能方块
class gray_block : public Block {
public:
	gray_block() : Block(true){}
	virtual void paint(const RECT& rect) const override {
		Rectangle(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0x7f7f7f);
	}
};

class gray_block_item : public Item {
public:
	virtual void paint(const RECT& rect) const override {
		int a = rect.right - rect.left;
		a /= 12;
		Rectangle(rect.left + a, rect.top + a, rect.right - rect.left - a * 2, rect.bottom - rect.top - a * 2, 0x7f7f7f);
	}
	virtual object* build(pointptr_t pos, double x, double y) const {
		return new gray_block();
	}
};

//红石火把
class redstone_torch : public Redstone {
public:
	redstone_torch(double x, double y) {
		x = x * 3 - 1.5;
		y = y * 3 - 1.5;
		if (std::abs(x) > std::abs(y))
			direction.real() = (intptr_t)x;
		else
			direction.imag() = (intptr_t)y;
		if(!dynamic_cast<Block*>(getDirection(direction)))
			direction = 0;
	}
	virtual void paint(const RECT& rect) const override {
		int midx = (rect.left + rect.right) / 2;
		int midy = (rect.top + rect.bottom) / 2;
		if (direction != pointptr_t(0, 0)) {

		}
	}
	pointptr_t direction = pointptr_t(0, 0);
};

std::unique_ptr<Item> Items[] = {
	std::unique_ptr<Item>(new gray_block_item),
	std::unique_ptr<Item>(new gray_block_item)
};
constexpr const int itemCount = sizeof(Items) / sizeof(Items[0]);
intptr_t selected = 0;

//绘制整个快捷栏
static void paintItems() {
	int height = SCRH / 6;
	int edge1 = height / 12;
	int height2 = height - edge1 * 2;
	//int edge2 = edge1 * 5 / 6;
	for (intptr_t a = 0; a < itemCount; ++a) {
		RECT rect{};
		rect.top = SCRH - height + edge1;
		rect.bottom = SCRH - edge1;
		rect.left = (LONG)(SCRW / 2 + height2 * (a * 2 - itemCount) / 2);
		rect.right = (LONG)(SCRW / 2 + height2 * (a * 2 - itemCount + 2) / 2);
		Rectangle(rect.left - edge1 / 2, rect.top - edge1 / 2, rect.right - rect.left + edge1, rect.bottom - rect.top + edge1, 0x7f7f7f);
		rect.left += edge1 / 2;
		rect.top += edge1 / 2;
		rect.right -= edge1 / 2;
		rect.bottom -= edge1 / 2;
		Rectangle(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, a == selected ? 0xefefef : 0xafafaf);
		Items[a]->paint(rect);
	}
}

static void paint() {
	object::paint();
	paintItems();
	ref();
}

static void ftimer() {
	paint();
}

static void starttimer() {
	::timer.reset(new timer_t(1000 / 10, ftimer));
}

static void endtimer() {
	::timer.release();
}

void init() {
	starttimer();
}

int event(unsigned int type, intptr_t p1, intptr_t p2) {
	switch (type) {
	case MS_RDOWN:
	{
		pointptr_t pos;
		double px, py;
		scrToPoint(p1, p2, &pos, &px, &py);
		object::setblock(pos, Items[selected]->build(pos, px, py));
		paint();
	}
		break;
	case KY_DOWN:
		switch (p1) {
		case VK_OEM_PLUS:
			if (++selected >= itemCount)
				selected = 0;
			paint();
			break;
		case VK_OEM_MINUS:
			if (--selected < 0)
				selected = (intptr_t)itemCount - 1;
			paint();
			break;
		}
	}
	return 0;
}
