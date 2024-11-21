
#include <map>
#include <LPCgamewnd.h>
#include <LPCtimer.h>
#include <LPCcomplex.h>

using namespace LPC;

using pointptr_t = complex<intptr_t>;
using cdouble = complex<double>;

using timer_t = timerex<>;
std::unique_ptr<timer_t> timer;
double stretch = 64;//缩放大小，单位是像素，也是单个方块的边长。
const double maxStretch = 256;//最大缩放大小
const double minStretch = 1;//最小缩放大小
double camx = 0, camy = 0;//摄像机在地图中的x,y坐标，对应屏幕坐标原点
pointptr_t cur;

//旋转缩放平移后绘制多边形
static void shiftPolygon(cdouble x, cdouble y, cdouble shift, const cdouble (&points)[], int count, COLORREF color) {
	POINT *p = (POINT*)_malloca(sizeof(POINT) * count);
	if (!p) throw std::bad_alloc();
	for (size_t a = 0; a < count; ++a) {
		cdouble res = x * points[a].real() + y * points[a].imag() + shift;
		p[a].x = (int)floor(res.real());
		p[a].y = (int)floor(res.imag());
	}
	Polygon(p, count, color);
	_freea(p);
}

static void rotatePolygeon(const RECT &rect, cdouble direction, const cdouble (&points)[], int count, COLORREF color) {
	cdouble x = direction;
	cdouble y = direction * pointptr_t::i;
	x.real() *= (rect.right - rect.left) * 0.5;
	x.imag() *= (rect.bottom - rect.top) * 0.5;
	y.real() *= (rect.right - rect.left) * 0.5;
	y.imag() *= (rect.bottom - rect.top) * 0.5;
	cdouble shift((rect.left + rect.right) * 0.5, (rect.top + rect.bottom) * 0.5);
	shiftPolygon(x, y, shift, points, count, color);
}

//获取屏幕坐标(x,y)对应了哪个位置对象的哪个点
static void scrToPoint(intptr_t x, intptr_t y, pointptr_t *res1, double *resx, double *resy) {
	double sx = x / stretch + camx;
	double sy = y / stretch + camy;
	res1->real() = (intptr_t)std::floor(sx);
	res1->imag() = (intptr_t)std::floor(sy);
	*resx = sx - res1->real();
	*resy = sy - res1->imag();
}

class object;

static std::map<pointptr_t, std::unique_ptr<object>> datamap;//存储地图数据的map

class object {
public:
	//调用该函数创建一个对象。
	virtual object *build() const = 0;
	virtual void setUpdate() { queueUpdate(); }//由setblock触发的更新，setblock只对被set位置的block使用此类更新，相邻位置的使用stackUpdate
	virtual void stackUpdate() {}//由栈触发的更新
	virtual void queueUpdate() { stackUpdate(); }//由队列触发的更新
	virtual void scheduleUpdate() { queueUpdate(); }//由计划刻触发的更新
	virtual void rightClicked(double x, double y) {}
	virtual void entered() {}
	virtual void paint(const RECT &rect) const = 0;//绘制对象, 应该将自身限制在rect对应的矩形范围内
	//执行一刻的事件
	static void tick() {
		std::deque<pointptr_t> list;
		list.swap(scheduleList);
		while (!list.empty()) {
			pointptr_t pos = list.front();
			list.pop_front();
			auto o = datamap.find(pos);
			if (o != datamap.end()) {
				o->second->scheduleUpdate();
				o->second->scheduled = false;
			}
		}
		while (!updateList.empty()) {
			pointptr_t pos = updateList.front();
			updateList.pop_front();
			auto o = datamap.find(pos);
			if (o != datamap.end()) {
				o->second->queueUpdate();
				o->second->inupdate = false;
			}
		}
	}
	//绘制整个缓冲区
	static void paint() {
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
				if (o != datamap.end()) o->second->paint(rect);
			}
		}
	}
	//将pos处的对象设置为obj。obj为nullptr时清除对应位置的对象
	static void setblock(pointptr_t pos, object *obj) {
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
			o->second->setUpdate();
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
	object *getDirection(pointptr_t p) const {
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
	//将自身加入更新队列中
	void queue() {
		if (inupdate) return;
		updateList.push_back(pos);
		inupdate = true;
	}
protected:
	inline const pointptr_t &getPos() const { return pos; }//获取自身坐标
	//用栈更新方法更新一阶毗邻内
	void stackUpdateNear() {
		if (left) left->stackUpdate();
		if (up) up->stackUpdate();
		if (right) right->stackUpdate();
		if (down) down->stackUpdate();
	}
	//用队列更新方法更新一阶毗邻内
	void queueUpdateNear() {
		if (left) left->queue();
		if (up) up->queue();
		if (right) right->queue();
		if (down) down->queue();
	}
	//将自己加入计划刻，下一刻触发队列更新
	void schedule() {
		if (scheduled) return;
		scheduleList.push_back(pos);
		scheduled = true;
	}
	//旁边有没有被充能了的方块或者能充能自己这个位置的红石原件
	bool is_powered() const;
	object *left = nullptr, *up = nullptr, *right = nullptr, *down = nullptr;
private:
	pointptr_t pos = {};//对象的坐标
	bool inupdate = false;//此刻是否已经在更新队列中了
	bool scheduled = false;//此刻是否已经在计划刻队列中了
	static std::deque<pointptr_t> updateList;//此刻的更新队列
	static std::deque<pointptr_t> scheduleList;//计划刻更新队列
};

class Item {
public:
	//绘制在快捷栏中的样子
	virtual void paint(const RECT &) const = 0;
	//选中了该物品后右键空白格子会调用该函数创建一个对象。pos是格子坐标
	virtual void use(pointptr_t pos, double x, double y) const {
		auto it = datamap.find(pos);
		if (it != datamap.end())
			it->second->rightClicked(x, y);
	}
	virtual void entered(pointptr_t pos) {
		auto it = datamap.find(pos);
		if (it != datamap.end())
			it->second->entered();
	}
};

class blockItem : public Item {
public:
	blockItem(object *obj) : obj(obj) {}
	virtual void use(pointptr_t pos, double x, double y) const override {
		auto it = datamap.find(pos);
		if (it != datamap.end())
			it->second->rightClicked(x, y);
		else {
			object *_obj = obj->build();
			_obj->rightClicked(x, y);
			object::setblock(pos, _obj);
		}
	}
	virtual void entered(pointptr_t pos) {
		auto it = datamap.find(pos);
		if (it != datamap.end())
			it->second->entered();
		else {
			object *_obj = obj->build();
			object::setblock(pos, _obj);
		}
	}
	virtual void paint(const RECT &rect) const {
		obj->paint(rect);
	}
	std::unique_ptr<object> obj;
};

std::deque<pointptr_t> object::updateList;
std::deque<pointptr_t> object::scheduleList;

class Redstone : public object {
public:
	static bool NONE;
	//获取向对应方向的激活状态
	bool &directionPower(pointptr_t p) {
		switch (p.real()) {
		case -1:
			if (p.imag() == 0)
				return left;
			else return NONE = false;
		case 0:
			switch (p.imag()) {
			case -1:
				return up;
			case 1:
				return down;
			default:
				return NONE = false;
			}
		case 1:
			if (p.imag() == 0)
				return right;
			else return NONE = false;
		default:
			return NONE = false;
		}
	}
	//获取对应方向是否吸引红石粉
	bool &directionPull(pointptr_t p) {
		switch (p.real()) {
		case -1:
			if (p.imag() == 0)
				return pullLeft;
			else return NONE = false;
		case 0:
			switch (p.imag()) {
			case -1:
				return pullUp;
			case 1:
				return pullDown;
			default:
				return NONE = false;
			}
		case 1:
			if (p.imag() == 0)
				return pullRight;
			else return NONE = false;
		default:
			return NONE = false;
		}
	}
	//检测一个obj周围有没有能充能它的红石原件
	static bool is_powered(object *obj) {
		if (!obj) return false;
		bool ret = false;
		pointptr_t d = 1;
		do {
			Redstone *objr = dynamic_cast<Redstone *>(obj->getDirection(d));
			if (objr) {
				bool p = objr->directionPower(-d);
				if (p) return true;
			}
			d *= pointptr_t::i;
		} while (d != (pointptr_t)1);
		return ret;
	}
	bool left = false, up = false, right = false, down = false;//是否能激活对应位置方块以及激活种类
	bool pullLeft = false, pullUp = false, pullRight = false, pullDown = false;//是否吸引对应方向的红石粉
};

bool Redstone::NONE = false;

bool object::is_powered() const {
	pointptr_t d = 1;
	do {
		object *o = getDirection(d);
		if (!o) continue;
		Redstone *o_r = dynamic_cast<Redstone *>(o);
		if (o_r)
			if (o_r->directionPower(-d))
				return true;
	} while (d != (pointptr_t)1);
	return false;
}

//灯
class light_bulb : public Redstone {
public:
	light_bulb() {
		pointptr_t direction = 1;
		do {
			directionPull(direction) = true;
			direction *= pointptr_t::i;
		} while (direction != (pointptr_t)1);
	}
	virtual object *build() const override {
		return new light_bulb;
	}
	virtual void paint(const RECT &rect) const override {
		constexpr const int minlim = 10;
		int x = rect.left;
		int y = rect.top;
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;
		if (w < minlim) {
			x -= (minlim - w) >> 1;
			w = minlim;
		}
		if (h < minlim) {
			y -= (minlim - h) >> 1;
			h = minlim;
		}
		Rectangle(x, y, w, h, powered ? 0xafffff : 0x1f2f3f);
	}
	virtual void stackUpdate() override {
		schedule();
	}
	virtual void scheduleUpdate() override {
		powered = is_powered(this);
	}
private:
	bool powered = false;
};

//逻辑门
class logic_gates : public Redstone {
public:
	void setConnect(bool left, bool back, bool right) {
		this->left = left;
		this->back = back;
		this->right = right;
	}
	void updateConnect() {
		directionPull(direction) = true;
		bool connection_changed = false;
		if (back) {
			bool &b = directionPull(-direction);
			bool last = b;
			Redstone *r = dynamic_cast<Redstone *>(getDirection(-direction));
			b = r && r->directionPull(direction);
			if (b != last) connection_changed = true;
		}
		else directionPull(-direction) = false;
		if (right) {
			bool &b = directionPull(direction * pointptr_t::i);
			bool last = b;
			Redstone *r = dynamic_cast<Redstone *>(getDirection(direction * pointptr_t::i));
			b = r && r->directionPull(direction * -pointptr_t::i);
			if (b != last) connection_changed = true;
		}
		else directionPull(direction * pointptr_t::i) = false;
		if (left) {
			bool &b = directionPull(-direction * pointptr_t::i);
			bool last = b;
			Redstone *r = dynamic_cast<Redstone *>(getDirection(-direction * pointptr_t::i));
			b = r && r->directionPull(direction * pointptr_t::i);
			if (b != last) connection_changed = true;
		}
		else directionPull(-direction * pointptr_t::i) = false;
		if (connection_changed) stackUpdateNear();
	}
	void init(double x, double y) {
		x = x - 0.5;
		y = y - 0.5;
		direction = 0;
		if (std::abs(x) >= std::abs(y))
			direction.real() = x >= 0 ? 1 : -1;
		else
			direction.imag() = y >= 0 ? 1 : -1;
		updateConnect();
	}
	logic_gates(double x, double y, bool left, bool back, bool right) {
		setConnect(left, back, right);
		init(x, y);
	}
	virtual void rightClicked(double x, double y) override {
		init(x, y);
		stackUpdate();
		stackUpdateNear();
	}
	virtual void entered() override {
		direction *= -pointptr_t::i;
		updateConnect();
		stackUpdate();
		stackUpdateNear();
	}
	virtual void scheduleUpdate() override {
		bool inputs[3]{};
		int in = 0;
		if (right) {
			Redstone *obj = dynamic_cast<Redstone *>(getDirection(direction * pointptr_t::i));
			if (obj != nullptr) inputs[in] = obj->directionPower(direction * -pointptr_t::i);
			else inputs[in] = false;
			++in;
		}
		if (back) {
			Redstone *obj = dynamic_cast<Redstone *>(getDirection(-direction));
			if (obj != nullptr) inputs[in] = obj->directionPower(direction);
			else inputs[in] = false;
			++in;
		}
		if (left) {
			Redstone *obj = dynamic_cast<Redstone *>(getDirection(direction * -pointptr_t::i));
			if (obj != nullptr) inputs[in] = obj->directionPower(direction * pointptr_t::i);
			else inputs[in] = false;
			++in;
		}
		directionPower(direction * pointptr_t::i) = false;
		directionPower(direction * -pointptr_t::i) = false;
		directionPower(-direction) = false;
		directionPower(direction) = powered = logic(inputs);
		object *pobj = getDirection(direction);
		if (pobj) pobj->stackUpdate();
	}
	virtual void stackUpdate() override {
		schedule();
		updateConnect();
	}
	virtual bool logic(const bool *inputs) = 0;
	bool powered = false;
	pointptr_t direction = pointptr_t(0, 0);
private:
	bool left, back, right;
};

//一个输入的逻辑门
class single_input_gates : public logic_gates {
public:
	single_input_gates(double x, double y, COLORREF colorPowered, COLORREF colorUnpowered)
		: logic_gates(x, y, false, true, false),
		colorPowered(colorPowered),
		colorUnpowered(colorUnpowered)
	{}
	virtual void paint(const RECT &rect) const override {
		cdouble points[] = { cdouble(0.75, 0.0), cdouble(-0.75, -0.75), cdouble(-0.75, 0.75) };
		rotatePolygeon(rect, direction, points, 3, powered ? colorPowered : colorUnpowered);
	}
	COLORREF colorPowered, colorUnpowered;
};

//非门
class not_gate : public single_input_gates {
public:
	virtual object *build() const override {
		return new not_gate(0.5, 0.5);
	}
	not_gate(double x, double y) : single_input_gates(x, y, 0xafffff, 0x1f2f3f) { powered = true; }
	virtual bool logic(const bool *inputs) override {
		return !*inputs;
	}
};

//中继器
class repeater : public single_input_gates {
public:
	virtual object *build() const override {
		return new repeater(0.5, 0.5);
	}
	repeater(double x, double y) : single_input_gates(x, y, 0xffffaf, 0x3f2f1f) {}
	virtual bool logic(const bool *inputs) override {
		return *inputs;
	}
};

//三个输入的逻辑门
class triple_input_gates : public logic_gates {
public:
	triple_input_gates(double x, double y) : logic_gates(x, y, true, true, true) {}
	virtual void stackUpdate() override {
		logic_gates::stackUpdate();
		pointptr_t d = 1;
		for (int a = 0; a < 4; ++a) {
			Redstone *r = dynamic_cast<Redstone *>(getDirection(d));
			if (r) attracted_by[a] = r->directionPull(-d);
			else attracted_by[a] = false;
			d *= pointptr_t::i;
		}
	}
	bool attracted_by[4]{};//right, down, left, up
};

//或门与或非门
class or_gates : public triple_input_gates {
public:
	or_gates(double x, double y, COLORREF colorPowered, COLORREF colorUnpowered)
		: triple_input_gates(x, y),
		colorPowered(colorPowered),
		colorUnpowered(colorUnpowered)
	{}
	virtual void paint(const RECT &rect) const override {
		COLORREF color = powered ? colorPowered : colorUnpowered;
		const cdouble points[] = {
			cdouble(-0.5, -0.5),
			cdouble(0.5, -0.5),
			cdouble(1, 0),
			cdouble(0.5, 0.5),
			cdouble(-0.5, 0.5)
		};
		rotatePolygeon(rect, direction, points, 5, color);
		const cdouble points2[] = {
			cdouble(1, 0.3),
			cdouble(0, 0.3),
			cdouble(0, -0.3),
			cdouble(1, -0.3)
		};
		pointptr_t d = 1;
		for (int a = 0; a < 4; ++a) {
			if(d != direction && attracted_by[a])
				rotatePolygeon(rect, d, points2, 4, color);
			d *= pointptr_t::i;
		}
	}
	COLORREF colorPowered, colorUnpowered;
};

//或门
class or_gate : public or_gates {
public:
	virtual object *build() const override {
		return new or_gate(0.5, 0.5);
	}
	or_gate(double x, double y) : or_gates(x, y, 0xffffaf, 0x3f2f1f) {}
	virtual bool logic(const bool *inputs) override {
		return inputs[0] || inputs[1] || inputs[2];
	}
};

//或非门
class or_not_gate : public or_gates {
public:
	virtual object *build() const override {
		return new or_not_gate(0.5, 0.5);
	}
	or_not_gate(double x, double y) : or_gates(x, y, 0xafffff, 0x1f2f3f) { powered = true; }
	virtual bool logic(const bool *inputs) override {
		return !(inputs[0] || inputs[1] || inputs[2]);
	}
};

//与门与与非门
class and_gates : public triple_input_gates {
public:
	and_gates(double x, double y, COLORREF colorPowered, COLORREF colorUnpowered)
		: triple_input_gates(x, y),
		colorPowered(colorPowered),
		colorUnpowered(colorUnpowered)
	{}
	virtual void paint(const RECT &rect) const override {
		COLORREF color = powered ? colorPowered : colorUnpowered;
		const cdouble points[] = {
			cdouble(-0.3, -0.3),
			cdouble(-0.5, -0.5),
			cdouble(0.5, -0.5),
			cdouble(1, 0),
			cdouble(0.5, 0.5),
			cdouble(-0.5, 0.5),
			cdouble(-0.5, -0.5),
			cdouble(-0.3, -0.3),
			cdouble(-0.3, 0.3),
			cdouble(0.3, 0.3),
			cdouble(0.3, -0.3)
		};
		rotatePolygeon(rect, direction, points, 11, color);
		const cdouble points2[] = {
			cdouble(1, 0.3),
			cdouble(0.4, 0.3),
			cdouble(0.4, -0.3),
			cdouble(1, -0.3)
		};
		pointptr_t d = 1;
		for (int a = 0; a < 4; ++a) {
			if (d != direction && attracted_by[a])
				rotatePolygeon(rect, d, points2, 4, color);
			d *= pointptr_t::i;
		}
	}
	COLORREF colorPowered, colorUnpowered;
};

//与门
class and_gate : public and_gates {
public:
	virtual object *build() const override {
		return new and_gate(0.5, 0.5);
	}
	and_gate(double x, double y) : and_gates(x, y, 0xffffaf, 0x3f2f1f) {}
	virtual bool logic(const bool *inputs) override {
		return inputs[0] && inputs[1] && inputs[2];
	}
};

//与非门
class and_not_gate : public and_gates {
public:
	virtual object *build() const override {
		return new and_not_gate(0.5, 0.5);
	}
	and_not_gate(double x, double y) : and_gates(x, y, 0xafffff, 0x1f2f3f) { powered = true; }
	virtual bool logic(const bool *inputs) override {
		return !(inputs[0] && inputs[1] && inputs[2]);
	}
};

//异或门与同或门
class xor_gates : public triple_input_gates {
public:
	xor_gates(double x, double y, COLORREF colorPowered, COLORREF colorUnpowered)
		: triple_input_gates(x, y),
		colorPowered(colorPowered),
		colorUnpowered(colorUnpowered)
	{}
	virtual void paint(const RECT &rect) const override {
		COLORREF color = powered ? colorPowered : colorUnpowered;
		const cdouble points[] = {
			cdouble(-0.3, -0.3),
			cdouble(-0.5, -0.5),
			cdouble(0.5, -0.5),
			cdouble(1, 0),
			cdouble(0.5, 0.5),
			cdouble(-0.5, 0.5),
			cdouble(-0.5, -0.5),
			cdouble(-0.3, -0.3),
			cdouble(0, 0),
			cdouble(-0.3, 0.3),
			cdouble(0.3, 0.3),
			cdouble(0, 0),
			cdouble(0.3, -0.3)
		};
		rotatePolygeon(rect, direction, points, 13, color);
		const cdouble points2[] = {
			cdouble(1, 0.3),
			cdouble(0.4, 0.3),
			cdouble(0.4, -0.3),
			cdouble(1, -0.3)
		};
		pointptr_t d = 1;
		for (int a = 0; a < 4; ++a) {
			if (d != direction && attracted_by[a])
				rotatePolygeon(rect, d, points2, 4, color);
			d *= pointptr_t::i;
		}
	}
	COLORREF colorPowered, colorUnpowered;
};

//异或门
class xor_gate : public xor_gates {
public:
	virtual object *build() const override {
		return new xor_gate(0.5, 0.5);
	}
	xor_gate(double x, double y) : xor_gates(x, y, 0xffffaf, 0x3f2f1f) {}
	virtual bool logic(const bool *inputs) override {
		return inputs[0] == inputs[1] == inputs[2];
	}
};

//同或门
class xnor_gate : public xor_gates {
public:
	virtual object *build() const override {
		return new xnor_gate(0.5, 0.5);
	}
	xnor_gate(double x, double y) : xor_gates(x, y, 0xafffff, 0x1f2f3f) { powered = true; }
	virtual bool logic(const bool *inputs) override {
		return !(inputs[0] == inputs[1] == inputs[2]);
	}
};

class wire : public Redstone {
public:
	virtual object *build() const override {
		return new wire();
	}
	wire() {
		pointptr_t direction = 1;
		do {
			directionPull(direction) = true;
			direction *= pointptr_t::i;
		} while (direction != (pointptr_t)1);
	}
	virtual void paint(const RECT &rect) const override {
		COLORREF color = powered == 0 ? 0x3f2f1f : 0xff7f3f;
		double midx, midy;
		midx = (rect.left + rect.right) * 0.5;
		midy = (rect.top + rect.bottom) * 0.5;
		double w, h;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
		int x1 = (int)std::floor(midx - w * 0.15);
		int w1 = (int)std::floor(midx + w * 0.15) - x1;
		int y1 = (int)std::floor(midy - h * 0.15);
		int h1 = (int)std::floor(midy + h * 0.15) - y1;
		int mx = (int)midx, my = (int)midy;
		int lw = (rect.right - rect.left) / 16;
		int lh = (rect.bottom - rect.top) / 16;
		Rectangle(x1 - lw, y1 - lh, w1 + lw * 2, h1 + lh * 2, color);
		if (connects[0]) Rectangle(mx, y1, rect.right - mx, h1, color);
		if (connects[1]) Rectangle(x1, my, w1, rect.bottom - my, color);
		if (connects[2]) Rectangle(rect.left, y1, mx - rect.left, h1, color);
		if (connects[3]) Rectangle(x1, rect.top, w1, my - rect.top, color);
	}
private:
	//获取相邻激活红石粉或激活原件对自己的最小激活距离，若没有激活返回0
	uintptr_t nearPower() {
		uintptr_t ret = -1;
		pointptr_t direction = 1;
		for (int index = 0; index < 4; ++index) {
			if (connects[index]) {
				object *o = getDirection(direction);
				wire *w = dynamic_cast<wire *>(o);
				if (w) {
					if (w->powered && w->powered < ret) ret = w->powered;
				}
				else {
					Redstone *obj = dynamic_cast<Redstone *>(o);
					if (obj) {
						if (obj->directionPower(-direction))
							return 1;
					}
				}
			}
			direction *= pointptr_t::i;
		}
		return ret + 1;
	}
	void updateConnect() {
		pointptr_t direction = 1;
		for (int a = 0; a < 4; ++a) {
			connects[a] = false;
			Redstone *r = dynamic_cast<Redstone *>(getDirection(direction));
			if (r) {
				if (r->directionPull(-direction)) {
					connects[a] = true;
				}
			}
			bool &bef = directionPower(direction);
			bool aft;
			if (connects[a] && powered != 0)
				aft = true;
			else aft = false;
			if (bef != aft) {
				bef = aft;
				object *o = getDirection(direction);
				if (o) o->stackUpdate();
			}
			else bef = aft;
			direction *= pointptr_t::i;
		}
	}
	void wireUpdate() {
		uintptr_t power = nearPower();
		if (power == powered) return;
		if (powered) {
			if (power > powered) {
				power = powered + 1;
				powered = 0;
				pointptr_t direction = 1;
				for (int a = 0; a < 4; ++a) {
					object *o = getDirection(direction);
					if (o) {
						wire *w = dynamic_cast<wire *>(o);
						if (w) {
							if (w->powered == power)
								w->wireUpdate();
						}
						else o->queue();
					}
					direction *= pointptr_t::i;
				}
			}
		}
		power = powered;
		powered = nearPower();
		if (power != powered) {
			updateConnect();
			//stackUpdateNear();
		}
	}
public:
	virtual void stackUpdate() override {
		queue();
	}
	virtual void queueUpdate() override {
		updateConnect();
		wireUpdate();
	}
	uintptr_t powered = 0;//是否已被激活以及激活距离
	bool connects[4]{};//right, down, left, up
};

std::unique_ptr<Item> Items[] = {
	std::unique_ptr<Item>(new blockItem(new repeater(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new not_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new or_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new or_not_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new and_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new and_not_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new xor_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new xnor_gate(0.5, 0.5))),
	std::unique_ptr<Item>(new blockItem(new wire())),
	std::unique_ptr<Item>(new blockItem(new light_bulb))
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
		rect.left += edge1 / 2;
		rect.top += edge1 / 2;
		rect.right -= edge1 / 2;
		rect.bottom -= edge1 / 2;
		Items[a]->paint(rect);
	}
}

static void paint() {
	Clear(0xdfdfdf);
	object::paint();
	paintItems();
	RECT rect{};
	int w = (int)(stretch / 16);
	if (w == 0) w = 1;
	rect.left = (int)std::floor((cur.real() - camx) * stretch);
	rect.top = (int)std::floor((cur.imag() - camy) * stretch);
	rect.right = (int)std::floor((cur.real() + 1 - camx) * stretch);
	rect.bottom = (int)std::floor((cur.imag() + 1 - camy) * stretch);
	Rectangle(rect.left - w, rect.top - w, w, rect.bottom - rect.top + w * 2, 0x000000);
	Rectangle(rect.right, rect.top - w, w, rect.bottom - rect.top + w * 2, 0x000000);
	Rectangle(rect.left - w, rect.top - w, rect.right - rect.left + w * 2, w, 0x000000);
	Rectangle(rect.left - w, rect.bottom, rect.right - rect.left + w * 2, w, 0x000000);
	ref();
}

static void ftimer() {
	object::tick();
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

static void moveToCur() {
	if (camx < cur.real() - SCRW / stretch + 1) camx = cur.real() - SCRW / stretch + 1;
	if (camx > cur.real()) camx = (double)cur.real();
	if (camy < cur.imag() - SCRH / stretch + 1) camy = cur.imag() - SCRH / stretch + 1;
	if (camy > cur.imag()) camy = (double)cur.imag();
}

int event(unsigned int type, intptr_t p1, intptr_t p2) {
	static bool shouldDelete, touching;
	static intptr_t lastx, lasty, startx, starty;
	static clock_t startt;
	switch (type) {
	case MS_RDOWN:
	{
		pointptr_t pos;
		double px, py;
		scrToPoint(p1, p2, &pos, &px, &py);
		Items[selected]->use(pos, px, py);
		paint();
	}
		break;
	case MS_LDOWN:
		shouldDelete = !touching;
		touching = true;
		lastx = startx = p1;
		lasty = starty = p2;
		startt = clock();
		break;
	case MS_MOVE:
		if(touching)
		{
			intptr_t dx = p1 - lastx;
			intptr_t dy = p2 - lasty;
			camx -= dx / stretch;
			camy -= dy / stretch;
			lastx += dx;
			lasty += dy;
			dx = lastx - startx;
			dy = lasty - starty;
			if (shouldDelete && (clock() - startt >= CLOCKS_PER_SEC / 10 || (dx * dx + dy * dy) * 65536 >= (intptr_t)SCRW * SCRH))
				shouldDelete = false;
			paint();
		}
		break;
	case MS_LUP:
		touching = false;
		if(shouldDelete)
		{
			pointptr_t pos;
			double px, py;
			scrToPoint(p1, p2, &pos, &px, &py);
			object::setblock(pos, nullptr);
			paint();
		}
		break;
	case MS_WHEEL:
	{
		double k = exp((double)GetWheelPixel() / DSKH);
		if (k * stretch > maxStretch) k = maxStretch / stretch;
		else if (k * stretch < minStretch) k = minStretch / stretch;
		stretch *= k;
		double x = p1 / stretch;
		double y = p2 / stretch;
		x = (k - 1) * x;
		y = (k - 1) * y;
		camx += x;
		camy += y;
		paint();
	}
		break;
	case KY_DOWN:
		switch (p1) {
		case VK_OEM_PLUS:
		case 'E':
			if (++selected >= itemCount)
				selected = 0;
			paint();
			break;
		case VK_OEM_MINUS:
		case 'Q':
			if (--selected < 0)
				selected = (intptr_t)itemCount - 1;
			paint();
			break;
		case VK_LEFT:
		case 'A':
			--cur.real();
			moveToCur();
			paint();
			break;
		case VK_UP:
		case 'W':
			--cur.imag();
			moveToCur();
			paint();
			break;
		case VK_RIGHT:
		case 'D':
			++cur.real();
			moveToCur();
			paint();
			break;
		case VK_DOWN:
		case 'S':
			++cur.imag();
			moveToCur();
			paint();
			break;
		case VK_RETURN:
			Items[selected]->entered(cur);
			paint();
			break;
		case VK_BACK:
			object::setblock(cur, nullptr);
			paint();
			break;
		default:
			if ('0' <= p1 && p1 <= '9') {
				selected = p1 - '1';
				if (selected < 0) selected = 9;
			}
		}
	}
	return 0;
}
