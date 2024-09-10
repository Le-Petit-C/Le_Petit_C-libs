
#include <windows.h>
#include <LPCcomplex.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>
#include <mutex>
#include <LPCwndex.hpp>

#define RAND_MOD (RAND_MAX + 1)

float randf() {
	return ((float)rand() / RAND_MOD + rand()) / RAND_MOD;
}

struct Object;
struct objectGroup;

LPC::complex<float> mapsize(64, 64);//地图大小
float foodregeneration = 1;//单个食物加的饱食度
float& mapwidth = mapsize._Re;
float& mapheight = mapsize._Im;
constexpr const float kf = -1;//阻力系数，无动力运动中速度大小与初始速度的比值应满足vt/v0=exp(kf*t)

enum {
	food_object,
	animal_object,
	attack_object,
	objecttype_count
};

//维持半径的值
struct objR {
	float R;
	float Rsquare;
	constexpr float getR() const { return R; }
	constexpr float getRsquare() const { return Rsquare; }
	constexpr void setR(float r) { R = r; Rsquare = r * r; }
	void setRsquare(float rsquare) { Rsquare = rsquare; R = sqrt(rsquare); }
	constexpr operator float() const{
		return Rsquare;
	}
	objR& operator=(float rsquare) {
		setRsquare(rsquare);
		return *this;
	}
	objR(float rsquare) {
		*this = rsquare;
	}
};

static constexpr const float stretch = 2.0f;

struct Object {
	std::atomic_bool dead = false;//是不是死了
	LPC::complex<float> pos;//自己的坐标
	LPC::complex<float> velocity;//速度
	objR objsize = 1.0f;//自身半径
	uint32_t color = 0;//颜色
	virtual void act(float dt) {}//定时器执行后的行动
	virtual void dieact() {}//死亡后的行动，比如animal可以留下尸体
	Object() {
		color = (rand() + rand() * RAND_MOD)&0xffffff;
		pos.real(randf() * mapwidth);
		pos.imag(randf() * mapheight);
	}
	Object(const Object& v) {
		dead = (bool)v.dead;
		pos = v.pos;
		velocity = v.velocity;
		objsize = v.objsize;
		color = v.color ^ ((rand() & 0xff) << 8);
	}
	virtual ~Object() {}
	void timer(float dt) {
		pos += velocity * dt;
		float r = objsize.getR();
		if (pos._Re < r) pos._Re = r;
		else if (pos._Re > mapwidth - r) pos._Re = mapwidth - r;
		if (pos._Im < r) pos._Im = r;
		else if (pos._Im > mapheight- r) pos._Im = mapheight - r;
		this->act(dt);
	}
	//多线程的timer以外单线程调用的timer
	virtual void aftertimer(float dt) {}
	//在当前的LPCdefault环境下绘制自身
	virtual void paint() {
		LPC::Circle(pos._Re * stretch, pos._Im * stretch, sqrt(objsize) * stretch, color);
	}
};

namespace groupActThread {
	volatile std::atomic_uint8_t createdthreads = 0;
	volatile std::atomic_uint8_t waitingthreads = 0;
	bool threadcreated = false;
	std::mutex mtx;
	volatile std::atomic_size_t runningindex;
	objectGroup* volatile currentGroup;
	volatile float dt;
	void Group_act_thread();
	void init(uint8_t nthreads);
	void exit();
}

struct objectGroup : public std::vector<std::unique_ptr<Object>> {
	//杀死Group内索引为index的Object
	void kill(size_t index) {
		(*this)[index].swap(back());
		pop_back();
	}
	/*
	//将距离某点最近的几个Object放在容器最后（越靠后越近），返回值表示最终放到了最后面的Object数（count大于size()时总是回返回size()）
	size_t searchClosest(LPC::complex<float> point, int count) {
		setsort(point);
		size_t sizeindex = size();
		std::make_heap(begin(), end(), compare);
		if (count > size()) count = size();
		for (int n = 0; n < count; ++n) {
			std::pop_heap(begin(), end() - n);
		}
		return count;
	}*/
	//查找距离某点最近的一个点，返回值是最近点的索引，没有东西就返回-1
	size_t getClosest(LPC::complex<float> point) const {
		size_t index = -1;
		float mindstsquare = INFINITY;
		for (size_t a = 0; a < size(); ++a) {
			float dstsquare = ((*this)[a]->pos - point).abssquare();
			if (mindstsquare > dstsquare) {
				mindstsquare = dstsquare;
				index = a;
			}
		};
		return index;
	}
	//查找一定范围内距离某点最近的一个点，返回值是最近点的索引，没有东西就返回-1
	size_t getClosestInRange(LPC::complex<float> point, float range) const {
		size_t index = -1;
		register float mindstsquare = INFINITY;
		for (size_t a = 0; a < size(); ++a) {
			const LPC::complex<float>& p = (*this)[a]->pos;
			register float x = p._Re - point._Re;
			if (x < -range) continue;
			if (x > range) continue;
			register float y = p._Im - point._Im;
			if (y < -range) continue;
			if (y > range) continue;
			register float dstsquare = x * x + y * y;
			if (mindstsquare > dstsquare) {
				mindstsquare = dstsquare;
				index = a;
			}
		};
		return index;
	}
	//查找距离某点最近的一个点(考虑对象大小)，返回值是最近点的索引，没有东西就返回-1
	size_t getClosest_withR(LPC::complex<float> point) const {
		size_t index = -1;
		float mindstsquare = INFINITY;
		for (size_t a = 0; a < size(); ++a) {
			float dstsquare = (back()->pos - point).abssquare();
			if (mindstsquare > dstsquare) {
				mindstsquare = dstsquare;
				index = a;
			}
		};
		return index;
	}
	//获取距离某点一定范围内的Object数量(仅判断中心，不判断半径)，dst表示距离
	size_t getCount(LPC::complex<float> point, float dst) const {
		size_t count = 0;
		float dstsquare = dst * dst;
		for (size_t a = 0; a < size(); ++a) {
			LPC::complex<float> current = (*this)[a]->pos - point;
			if (current._Re < -dst || current._Re > dst) continue;
			if (current._Im < -dst || current._Im > dst) continue;
			if (current.abssquare() > dstsquare) continue;
			++count;
		}
		return count;
	}
	//获取一定范围内左边右边Object数量
	//其中direction不仅包含方向信息还要包含最大探测距离信息
	//range表示最大偏移多少弧度可以算在内
	void getRangeCount(LPC::complex<float> point, LPC::complex<float> direction, float range, size_t* buf) const {
		float tanrange = tan(range);
		buf[0] = buf[1] = 0;
		float dstsquare = direction.abssquare();
		float dst = sqrt(dstsquare);
		for (size_t a = 0; a < size(); ++a) {
			LPC::complex<float> current = (*this)[a]->pos - point;
			if (current._Re < -dst || current._Re > dst) continue;
			if (current._Im < -dst || current._Im > dst) continue;
			if (current.abssquare() > dstsquare) continue;
			float t = current._Re * direction._Re + current._Im * direction._Im;
			if (t <= 0) continue;
			float s = (current._Im * direction._Re - current._Re * direction._Im);
			if (s > 0) {
				if (s < tanrange * t) ++buf[0];
			}
			else {
				if (s > -tanrange * t) ++buf[1];
			}
		}
	}
	//检测一下当前Group中有没有object死了
	void testdead() {
		for (size_t a = 0; a < size(); ++a) {
			if ((*this)[a]->dead){
				(*this)[a]->dieact();
				back().swap((*this)[a]);
				pop_back();
				--a;
			}
		}
		extern ::objectGroup foods;
	}
	//用多线程方法让里面所有Object执行一次timer
	virtual void timer(float dt) {
		groupActThread::dt = dt;
		groupActThread::currentGroup = this;
		groupActThread::runningindex = size();
		groupActThread::mtx.unlock();
		while (groupActThread::runningindex) std::this_thread::yield();
		groupActThread::mtx.lock();
		while (groupActThread::waitingthreads != groupActThread::createdthreads) std::this_thread::yield();
	}
	//用单线程方法让里面所有Object执行一次aftertimer
	virtual void aftertimer(float dt) {
		for (size_t a = 0; a < size(); ++a) {
			(*this)[a]->aftertimer(dt);
		}
	}
	//让里面所有Object执行一次paint
	void paint() {
		for (size_t a = 0; a < size(); ++a) {
			(*this)[a]->paint();
		}
	}
};

namespace groupActThread {
	void Group_act_thread() {
		while (true) {
			++waitingthreads;
			mtx.lock();
			--waitingthreads;
			if (!threadcreated) {
				mtx.unlock();
				return;
			}
			if (!runningindex) {
				mtx.unlock();
				continue;
			}
			size_t index = --runningindex;
			mtx.unlock();
			(*currentGroup)[index]->timer(dt);
			index = 0;
		}
	}
	void init(uint8_t nthreads) {
		mtx.lock();
		threadcreated = true;
		while (createdthreads < nthreads) {
			std::thread(Group_act_thread).detach();
			++createdthreads;
		}
	}
	void exit() {
		threadcreated = false;
		mtx.unlock();
	}
	thread_local std::unique_ptr<float> threadbuffer;
	thread_local size_t threadbuffersize = 0;
	float* ensurethreadbuffer(size_t size) {
		if (size > threadbuffersize)
			threadbuffer.reset((float*)malloc(sizeof(float) * size));
		return threadbuffer.get();
	}
}

objectGroup foods, animals;
objectGroup attacks, defences;

struct food : public Object {
	float existTime = 64;
	void act(float dt) override {
		existTime -= dt;
		if (existTime <= 0) dead = true;
	}
};

struct animal;

//攻击物
struct attack : Object {
	attack(const animal& obj);
	virtual void act(float dt) override;
	virtual void paint() override;
};

//防护盾
struct defence : Object {
	float time;
	defence(const animal& obj, float angle);
	virtual void act(float dt) override;
	virtual void paint();
};

template<typename T>
LPC::complex<T> randdirection(){
	LPC::complex<T> f;
	while (true) {
		f._Re = randf();
		f._Im = randf();
		T s = f.abs();
		if (s != 0) return f / s;
	}
}

struct animal : public Object {
	bool need_emplace_attack = false;
	bool need_emplace_defence = false;
	bool needclone = false;
	static constexpr const float copyfoodlimit = 256;
	static constexpr const float copyfoodsub = 128;
	float health = 8;
	float food = 8;
	float mutaterate = 1.0f / (1 << 9);//变异频率
	float mutaterange = 0.125f;//变异大小
	float* brainbuffer;
	size_t brainbuffersize;
	size_t outputendindex;
	LPC::complex<float> direction = randdirection<float>();//当前自己朝向的方向
	virtual void furtheract(float dt) = 0;
	virtual void dieact() override{
		for (int a = (int)(objsize / foodregeneration); a > 0; --a) {
			foods.emplace_back(new Object);
			float f1 = randf() * 3.1415926f, f2 = randf() * objsize.getR();
			LPC::complex<float> p(cos(f1) * f2, sin(f1) * f2);
			foods.back()->pos = pos + p;
		}
	};
	//在当前的LPCdefault环境下绘制自身
	virtual void paint() override {
		Object::paint();
		LPC::Rectangle(
			(pos._Re - objsize.getR()) * stretch,
			(pos._Im - objsize.getR() * 1.5f) * stretch,
			(objsize.getR() * 2.0f) * health / objsize * stretch,
			2,//(objsize.getR() * 0.0625f) * stretch,
			0x00ff00);
		LPC::Rectangle(
			(pos._Re - objsize.getR()) * stretch,
			(pos._Im - objsize.getR() * (1.5f /* - 0.0625f*/)) * stretch + 2,
			(objsize.getR() * 2.0f) * food / objsize * stretch,
			2,//(objsize.getR() * 0.0625f) * stretch,
			0x007fff);
		LPC::Circle(
			(pos._Re + direction._Re * objsize.getR() * 0.75f) * stretch,
			(pos._Im + direction._Im * objsize.getR() * 0.75f) * stretch,
			objsize.getR() * 0.25f * stretch,
			0x000000);
	}
	virtual void act(float dt) override {
		size_t index;
		//检测有没有能吃的食物
		if (food < objsize && foods.size()) {
			index = foods.getClosestInRange(pos, objsize.getR());
			if (index != -1 && !foods[index]->dead.exchange(true)) {
				food += foodregeneration;
				foods[index]->dead = true;
			}
		}
		
		//检测有没有别人发射的攻击打到自己了
		index = attacks.getClosestInRange(pos, objsize.getR());
		if (index != -1) {
			LPC::complex<float> f = velocity - attacks[index]->velocity;
			health -= f.abssquare() * 0.0625;
			attacks[index]->dead = true;
		}
		//复制分裂
		needclone = (food >= objsize && health * 2.0f >= objsize);
		//恢复生命值
		if (health < objsize && food * 2 > objsize) {
			food -= dt * 0.25;
			health += dt * 0.25;
		}
		/*
		//输入探测点
		LPC::complex<float> point(brainbuffer[outputendindex - 1], brainbuffer[outputendindex - 2]);
		point *= objsize.getR() * 16.0f * direction;
		point += pos;
		//获取探测点范围内的数据
		animals.getClosest_withR(point);
		int count = foods.getCount(point, brainbuffer[outputendindex - 3]);
		brainbuffer[0] += objsize;*/

		brainbuffer[0] += objsize;
		brainbuffer[1] += food;
		brainbuffer[2] += health;
		//输入探测点
		size_t buf[2] = { 0, 0 };
		float dst = brainbuffer[outputendindex - 1];
		//获取探测点范围内的数据
		index = foods.getClosest(pos);
		if (index != -1) {
			LPC::complex<float> p = foods[index]->pos - pos;
			brainbuffer[3] += direction._Re * p._Im - direction._Im * p._Re;
			brainbuffer[4] += p.abs();
		}
		index = animals.getClosest(pos);
		if (index != -1) {
			LPC::complex<float> p = animals[index]->pos - pos;
			brainbuffer[5] += direction._Re * p._Im - direction._Im * p._Re;
			brainbuffer[6] += p.abs();
		}
		index = attacks.getClosest(pos);
		if (index != -1) {
			LPC::complex<float> p = attacks[index]->pos - pos;
			brainbuffer[7] += direction._Re * p._Im - direction._Im * p._Re;
			brainbuffer[8] += p.abs();
		}

		//内部运算
		furtheract(dt);

		//输出
		float s;
		//加速
		s = brainbuffer[outputendindex - 4] + 0.5;
		velocity = s * direction * objsize.getR();
		food -= dt * velocity.abssquare() * objsize.getR() / 256.0f * (s < 0 ? 1.25f : 1.0f);
		//转向
		s = brainbuffer[outputendindex - 5] * dt;
		//food -= abs(s);
		s *= 4.0f;
		LPC::complex<float> f(cos(s), sin(s));
		direction *= f;
		direction /= direction.abs();
		
		//攻击
		need_emplace_attack = (brainbuffer[outputendindex - 6] > (1.0f - 1.0f / 64));
		//防御
		need_emplace_defence = (brainbuffer[outputendindex - 7] > (1.0f - 1.0f / 64));

		//静息消耗饥饿值
		food -= dt * objsize.getR() / 64.0f;
		if (need_emplace_attack) food -= 0.5;
		if (need_emplace_defence) food -= 0.125;
		//受到饥饿伤害
		if (food <= 0) {
			health += food;
			food = 0;
		}
		if (health <= 0)
			dead = true;
	}
	void emplace_attack() {
		attacks.emplace_back(new attack(*this));
	}
	void emplace_defence() {
		defences.emplace_back(new defence(*this, brainbuffer[outputendindex - 8]));
	}
	virtual void aftertimer(float dt) override {
		if (need_emplace_attack)
			emplace_attack();
		if (need_emplace_defence)
			emplace_defence();
		if (needclone)
			clone();
	}
	void halfdata() {
		health *= 0.5;
		food *= 0.5;
	}
	//进行随机变异
	void randomMutate() {
		int a;
		for (a = 0; a < outputendindex; ++a)
			brainbuffer[a] = 0;
		float s = a;
		while (true) {
			s -= log(randf()) / mutaterate;
			if (!(s < brainbuffersize)) break;
			a = s;
			brainbuffer[a] += (randf() * 2.0f - 1.0f) * mutaterange;
		}
		mutaterate *= pow(2.0f, randf() * 0.0625);
		mutaterange *= pow(2.0f, randf() * 0.0625);
		objsize.setR(objsize.getR() * pow(2.0f, randf() * 0.0625));
	}
	virtual void clone() = 0;
	animal(size_t brainsize, size_t endindex) {
		objsize = 16;
		outputendindex = endindex;
		brainbuffersize = brainsize;
		brainbuffer = (float*)malloc(sizeof(float) * brainsize);
		if (brainbuffer == nullptr) throw std::bad_alloc();
		for (size_t a = 0; a < brainbuffersize; ++a) {
			brainbuffer[a] = randf() * 2.0f - 1.0f;
		}
	}
	animal(const animal& v) : Object(v){
		direction = v.direction;
		health = v.health;
		food = v.food;
		mutaterate = v.mutaterate;
		mutaterange = v.mutaterange;
		brainbuffersize = v.brainbuffersize;
		outputendindex = v.outputendindex;
		brainbuffer = (float*)malloc(sizeof(float) * brainbuffersize);
		if (!brainbuffer) throw std::bad_alloc();
		memcpy(brainbuffer, v.brainbuffer, sizeof(float) * brainbuffersize);
	}
	virtual ~animal() override {
		free(brainbuffer);
	}
};

//求和后用这个把数值化归回(-1,1)范围内
inline float makedata(float x) {
	return tanh(x);
}

int constexpr intpow(int a, int k) {
	int ret = 1;
	if (!k) return 1;
	while (true) {
		if (k & 1) ret *= a;
		k >>= 1;
		if (!k) return ret;
		a *= a;
	}
}

//自己的小AI
//k表示内容有多少层
template<uint8_t k>
struct AIanimal1 : public animal {
	static constexpr const int buffersize = intpow(2, k);
	AIanimal1() : animal(buffersize * (k + 3), buffersize){
		color = RGB(255, rand() % 256, 0);
	}

	AIanimal1(const AIanimal1& v) : animal(v) {
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		//初始化
		float* buffer = groupActThread::ensurethreadbuffer(buffersize);
		float* factorbuffer = brainbuffer + buffersize - 1;
		memset(buffer, 0, buffersize * sizeof(float));

		//中间层在思考
		for (int a = 0; a < buffersize; ++a) {
			float& buf = buffer[a];
			buf += brainbuffer[a] * *++factorbuffer;
			buf += *++factorbuffer;
			for (uint8_t b = 0; b < k; ++b) {
				buf += brainbuffer[a ^ (1 << b)] * *++factorbuffer;
			}
		}
		for (int a = 0; a < buffersize; ++a) {
			brainbuffer[a] = makedata(buffer[a]);
		}
	}

	virtual void clone() override {
		animals.emplace_back(new AIanimal1<k>(*this));
		halfdata();
	}

};

//分层神经元网络的AI
//k表示内容有多少层
template<uint8_t k>
struct AIanimal2 : public animal {
	static constexpr const int buffersize = k;
	AIanimal2() : animal(k + k * (k + 1) * k, buffersize) {
		color = RGB(0, rand() % 256, 255);
	}

	AIanimal2(const AIanimal2& v) : animal(v) {
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		//初始化
		float* buffer = groupActThread::ensurethreadbuffer(k);
		float* factorbuffer = brainbuffer + k - 1;
		memset(buffer, 0, k * sizeof(float));

		//中间层在思考
		for (int a = 0; a < k; ++a) {
			float& buf = buffer[a];
			buf += *++factorbuffer;
			for(int b = 0; b < k; ++b)
				buf += brainbuffer[b] * *++factorbuffer;
			for (int b = 0; b < buffersize; ++b) {
				brainbuffer[b] = makedata(buffer[b]);
			}
		}
	}

	virtual void clone() override {
		animals.emplace_back(new AIanimal2<k>(*this));
		halfdata();
	}

};

struct programmedanimal : public animal {
	programmedanimal() : animal(8, 8) {
		color = RGB(255, rand() % 256, 255);
	}

	programmedanimal(const programmedanimal& v) : animal(v) {
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		brainbuffer[3] /= 16.0f;
		brainbuffer[4] = 1;
	}

	virtual void clone() override {
		animals.emplace_back(new programmedanimal(*this));
		halfdata();
	}

};

attack::attack(const animal& obj) {
	pos = obj.pos;
	pos += obj.direction * obj.objsize.getR() * 1.5f;
	velocity = obj.velocity + obj.direction * obj.objsize.getR() * 2.0f;
	objsize = 0;
}

void attack::act(float dt) {
	if (pos._Re <= 0 || pos._Re >= mapwidth || pos._Im <= 0 || pos._Im >= mapheight) {
		dead = true;
		return;
	}
}

void attack::paint() {
	POINT points[3] = {};
	for (int a = 0; a < 3; ++a) {
		constexpr float f = 2.0943951f;
		LPC::complex<float> p(cos(a * f), sin(a * f));
		p *= velocity * 0.125;
		points[a].x = (int)((pos._Re + p._Re) * stretch);
		points[a].y = (int)((pos._Im + p._Im) * stretch);
	}
	LPC::Triangle(points, 0x0000ff);
}

defence::defence(const animal& obj, float angle) : Object((const Object&)obj) {
	pos += obj.direction * objsize.getR() * 2.0f * LPC::complex<float>(cos(angle), sin(angle));
	objsize.setR(objsize.getR() * 0.5);
	//color = 0x7fff7f;
	time = 16;
}

void defence::act(float dt) {
	if (time <= 0) dead = true;
	size_t index = attacks.getClosestInRange(pos, objsize.getR());
	if (index != -1) {
		attacks[index]->dead = true;
		dead = true;
	}
	time -= dt;
}

void defence::paint() {
	if (time <= 0) return;
	LPC::Circle(pos._Re * stretch, pos._Im * stretch, objsize.getR() * stretch, Gdiplus::Color(time / 16.0f * 255, 127, 127, 255));
}
