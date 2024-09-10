
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

size_t foodlimit;
//最大食物密度，单位为饱食度/单位面积
float maxfooddensity = 0.25;
//食物生成速度，单位为饱食度/(单位面积*秒)
float foodregenerate_densityspeed = 0.001;
//初始食物密度，单位为饱食度/(单位面积*秒)
float initfooddensity = 0.025;
//目标计算速率，单位为刻/秒
float aimedcalculatespeed = 50.0f;
//运行速度倍率
float ktimespeed = 4.0f;
//额外的计算线程数
int threadcount = 20;
//初始随机参数倍率
float initrandomk = 1.5f;
//初始每种颜色的被捕食者数量
int initpreycount = 128;
//初始每种颜色的捕食者数量
int initpredatorcount = 16;
//捕食者最大速度乘数
float predatormaxspeed = 1.05f;
//被捕食者最大速度乘数
float preymaxspeed = 1.0f;
//初始变异率
float initmutaterate = 0.002f;
//初始变异乘数
float initmutaterange = 0.125f;
//变异率的变异乘数
float mutatemutaterange = 0.125f;

#define RAND_MOD (RAND_MAX + 1)

float randf() {
	float f = ((float)rand() / RAND_MOD + rand()) / (RAND_MOD * 2.0f) + 0.5f;
	size_t k = rand();
	while (k & 1) {
		k >>= 1;
		f *= 0.5;
	}
	return f;
}

struct Object;
struct objectGroup;

LPC::complex<float> mapsize(64, 64);//地图大小
float foodregeneration = 8.0f;//单个食物加的饱食度
float& mapwidth = mapsize._Re;
float& mapheight = mapsize._Im;

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

float stretch = 2.0f;

struct Object {
	std::atomic_bool dead = false;//是不是死了
	LPC::complex<float> pos;//自己的坐标
	LPC::complex<float> velocity;//速度
	objR objsize = stretch * 4.0f;//自身半径
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
		LPC::Circle(pos._Re * stretch, pos._Im * stretch, objsize.getR() * stretch, color);
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
		float mindstsquare = range * range;
		for (size_t a = 0; a < size(); ++a) {
			const LPC::complex<float>& p = (*this)[a]->pos;
			float x = p._Re - point._Re;
			if (x < -range) continue;
			if (x > range) continue;
			float y = p._Im - point._Im;
			if (y < -range) continue;
			if (y > range) continue;
			float dstsquare = x * x + y * y;
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
	//查找objsize.getRsquare()小于一个阈值的最近的一个Object，返回值是最近点的索引，没有东西就返回-1
	size_t getSmallest_range(LPC::complex<float> point, float limit) const {
		size_t index = -1;
		float mindstsquare = INFINITY;
		for (size_t a = 0; a < size(); ++a) {
			const Object& obj = *(*this)[a];
			if (obj.objsize >= limit) continue;
			float dstsquare = (obj.pos - point).abssquare();
			if (mindstsquare > dstsquare) {
				mindstsquare = dstsquare;
				index = a;
			}
		};
		return index;
	}
	//查找objsize.getRsquare()大于一个阈值的最近(减去半径后的距离)的一个Object，返回值是最近点的索引，没有东西就返回-1
	size_t getNearstBiggest_range(LPC::complex<float> point, float limit) const {
		size_t index = -1;
		float mindstsquare = INFINITY;
		for (size_t a = 0; a < size(); ++a) {
			const Object& obj = *(*this)[a];
			if (obj.objsize.getRsquare() <= limit) continue;
			float dst = ((*this)[a]->pos - point).abs() - (*this)[a]->objsize.getR();
			if (mindstsquare > dst) {
				mindstsquare = dst;
				index = a;
			}
		};
		return index;
	}
	//查找有没有能够覆盖某点且半径平方大于某阈值的最大的一个Object
	bool getCovered_Biggest(LPC::complex<float> point, float limit) const {
		for (size_t a = 0; a < size(); ++a) {
			if ((*this)[a]->objsize.getRsquare() <= limit) continue;
			if (((*this)[a]->pos - point).abssquare() <= (*this)[a]->objsize.getRsquare())
				return true;
		};
		return false;
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

objectGroup foods, predators, prey;

struct food : public Object {
	float existTime = 256;
	void act(float dt) override {
		existTime -= dt;
		if (existTime <= 0) dead = true;
	}
};

struct animal;

template<typename T>
LPC::complex<T> randdirection(){
	LPC::complex<T> f;
	while (true) {
		f._Re = randf() * 2.0f - 0.5f;
		f._Im = randf() * 2.0f - 0.5f;
		T s = f.abs();
		if (s != 0) return f / s;
	}
}

struct animal : public Object {
	bool needclone = false;
	bool eaten = false;
	bool is_predator;
	static constexpr const float copyfoodlimit = 256;
	static constexpr const float copyfoodsub = 128;
	float health;
	float food;
	float mutaterate = initmutaterate;//变异频率
	float mutaterange = initmutaterate;//变异大小
	float* brainbuffer;
	size_t brainbuffersize;
	size_t outputendindex;
	LPC::complex<float> direction = randdirection<float>();//当前自己朝向的方向
	LPC::complex<float> detectpoint;
	virtual void furtheract(float dt) = 0;
	virtual void dieact() override{
		if (eaten) return;
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
		/*if (detectpoint._Re != 0 || detectpoint._Im != 0)
			LPC::DrawLine(pos._Re * stretch, pos._Im * stretch, detectpoint._Re * stretch, detectpoint._Im * stretch, RGB(255, 255, 255));
		*/
	}
	virtual void act(float dt) override {
		size_t index;
		//检测有没有能吃的食物
		if (!is_predator && food < objsize && foods.size()) {
			index = foods.getClosestInRange(pos, objsize.getR());
			if (index != -1 && !foods[index]->dead.exchange(true)) {
				food += foodregeneration;
				foods[index]->dead = true;
			}
		}
		//有没有其他动物能被自己吃掉
		if(is_predator && food < objsize) {
		index = prey.getClosestInRange(pos, objsize.getR());
			if (index != -1) {
				if ((prey[index]->pos - pos).abssquare() <= objsize) {
					if (!prey[index]->dead.exchange(true)) {
						food += prey[index]->objsize * 0.25;
						((animal*)prey[index].get())->eaten = true;
					}
				}
			}
		}
		//复制分裂
		needclone = (food >= objsize && health * 2.0f >= objsize);
		//恢复生命值
		if (health < objsize && food * 2 > objsize) {
			food -= dt;
			health += dt;
		}
		float* input = brainbuffer;
		float* output = brainbuffer + outputendindex;

		//*input++ += objsize;
		*input++ += food / objsize;
		*input++ += health / objsize;
		//输入探测点
		size_t buf[2] = { 0, 0 };
		float f1;
		f1 = randf() * (3.14159265358979 * 2.0f);
		/*detectpoint._Re = *--output;
		detectpoint._Im = *--output;
		//detectpoint._Re = 0;
		//detectpoint._Im = 0;
		//detectpoint += 0.5f;
		detectpoint._Re = cos(f1);
		detectpoint._Im = sin(f1);
		detectpoint *= log(randf());
		//detectpoint *= direction;
		//detectpoint *= objsize.getR() * 16.0f;
		detectpoint *= objsize.getR();
		detectpoint *= *--output * 16.0f;
		detectpoint += pos;*/
		detectpoint = pos;
		//获取探测点范围内的数据
		//离探测点最近的食物
		index = foods.getClosest(detectpoint);
		if (index != -1) {
			LPC::complex<float> p = (foods[index]->pos - pos) * direction.conjugate() / objsize.getR();
			input[0] += p._Re;
			input[1] += p._Im;
		}
		input += 2;
		//离探测点最近的捕食者
		index = predators.getClosest(detectpoint);
		if (index != -1) {
			LPC::complex<float> p = (predators[index]->pos - pos) * direction.conjugate() / objsize.getR();
			input[0] += p._Re;
			input[1] += p._Im;
		}
		input += 2;
		//离探测点最近的被捕食者
		index = prey.getClosest(detectpoint);
		if (index != -1) {
			LPC::complex<float> p = (prey[index]->pos - pos) * direction.conjugate() / objsize.getR();
			input[0] += p._Re;
			input[1] += p._Im;
		}
		input += 2;
		//还原早期判断方式
		/*
		index = predators.getClosest(detectpoint);
		Object* obj1 = predators[index].get();
		index = prey.getClosest(detectpoint);
		Object* obj2 = prey[index].get();
		if ((obj2->pos - pos).abssquare() < (obj1->pos - pos).abssquare()) std::swap(obj1, obj2);
		LPC::complex<float> p = (obj1->pos - pos) * direction.conjugate() / objsize.getR();
		input[0] += p._Re;
		input[1] += p._Im;*/

		//内部运算
		furtheract(dt);

		//输出
		float s;
		float sqrtR = sqrt(objsize.getR());
		//移动
		s = *--output + 0.5;
		velocity = s * direction * 16.0f * (is_predator ? predatormaxspeed : preymaxspeed);
		food -= dt * velocity.abssquare() / 8192.0f * (s < 0 ? 1.25f : 1.0f) * sqrtR;
		//转向
		s = *--output * dt;
		//food -= abs(s);
		s *= 4.0f;
		LPC::complex<float> f(cos(s), sin(s));
		direction *= f;
		direction /= direction.abs();

		//静息消耗饥饿值
		food -= dt * objsize.getR() / 32.0f;
		//受到饥饿伤害
		if (food <= 0) {
			health += food;
			food = 0;
		}
		if (health <= 0)
			dead = true;
	}
	virtual void aftertimer(float dt) override {
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
		mutaterate *= exp((randf() * 2.0f - 1.0f) * mutatemutaterange);
		mutaterange *= exp((randf() * 2.0f - 1.0f) * mutatemutaterange);
		//objsize.setR(objsize.getR() * pow(2.0f, randf() * 0.25f));
	}
	virtual animal* Giveclone() = 0;
	void clone() {
		objectGroup* p = is_predator ? &predators : &prey;
		p->emplace_back(Giveclone());
		halfdata();
	}
	animal(size_t brainsize, size_t endindex, bool _is_predator) {
		is_predator = _is_predator;
		objsize = is_predator ? 64 : 16;
		food = health = objsize * 0.5;
		outputendindex = endindex;
		brainbuffersize = brainsize;
		brainbuffer = (float*)calloc(sizeof(float), brainsize);
		if (brainbuffer == nullptr) throw std::bad_alloc();
		for (size_t a = 0; a < brainbuffersize; ++a) {
			brainbuffer[a] = (randf() - 0.5f) * initrandomk;
		}
	}
	animal(const animal& v) : Object(v){
		is_predator = v.is_predator;
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
struct AIanimal1 : public animal {
	uint8_t k;
	size_t buffersize;
	AIanimal1(uint8_t _k, bool is_predator) : k(_k), buffersize(intpow(2, _k)), animal(intpow(2, _k)* (_k + 3), intpow(2, _k), is_predator){
		color = RGB(255, rand() & 0xff, 0);
	}

	AIanimal1(const AIanimal1& v) : animal(v) {
		k = v.k;
		buffersize = v.buffersize;
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
	
	virtual animal* Giveclone() override {
		return new AIanimal1(*this);
	}

};

//分层神经元网络的AI
//k表示内容有多少层
struct AIanimal2 : public animal {
	size_t layersize;
	AIanimal2(size_t _layersize, bool is_predator) : layersize(_layersize),
		animal(_layersize + _layersize * (_layersize + 1) * _layersize, _layersize, is_predator) {
		color = RGB(0, rand() % 256, 255);
	}

	AIanimal2(const AIanimal2& v) : animal(v) {
		layersize = v.layersize;
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		//初始化
		float* buffer = groupActThread::ensurethreadbuffer(layersize);
		float* factorbuffer = brainbuffer + layersize - 1;

		//中间层在思考
		for (int a = 0; a < layersize; ++a) {
			for (int b = 0; b < layersize; ++b) {
				float& buf = buffer[b] = 0;
				buf += *++factorbuffer;
				for(size_t c = 0; c < layersize; ++c)
					buf += brainbuffer[c] * *++factorbuffer;
			}
			for (int b = 0; b < layersize; ++b) {
				brainbuffer[b] = makedata(buffer[b]);
			}
		}
	}

	virtual animal* Giveclone() override {
		return new AIanimal2(*this);
	}

};

struct AIanimal3 : public animal {
	size_t layersize;
	AIanimal3(size_t _layersize, bool is_predator) : layersize(_layersize),
		animal(_layersize + _layersize * (_layersize + 1), _layersize, is_predator) {
		color = RGB(255, rand() & 0xff, 255);
	}

	AIanimal3(const AIanimal3& v) : animal(v) {
		layersize = v.layersize;
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		//初始化
		float* buffer = groupActThread::ensurethreadbuffer(layersize);
		float* factorbuffer = brainbuffer + layersize - 1;

		//中间层在思考
		for (size_t a = 0; a < layersize; ++a) {
			float& buf = buffer[a] = 0;
			buf += *++factorbuffer;
			for (int b = 0; b < layersize; ++b)
				buf += brainbuffer[b] * *++factorbuffer;
		}
		for (int b = 0; b < layersize; ++b) {
			brainbuffer[b] = makedata(buffer[b]);
		}
	}

	virtual animal* Giveclone() override {
		return new AIanimal3(*this);
	}

};

struct programmedanimal : public animal {
	programmedanimal(bool is_predator) : animal(8, 8, is_predator) {
		color = RGB(63, rand() % 256, 63);
	}

	programmedanimal(const programmedanimal& v) : animal(v) {
		randomMutate();
		halfdata();
	}

	virtual void furtheract(float dt) override {
		if (!is_predator) {
			float f = brainbuffer[2];
			if (f <= 0) brainbuffer[6] = brainbuffer[3] > 0 ? 1.0f : -1.0f;
			else {
				f = brainbuffer[3] / f;
				if (f > 1.0f) f = 1.0f;
				else if (f < -1.0f) f = -1.0f;
				brainbuffer[6] = f;
			}
			brainbuffer[6] += (randf() - 0.5f) * 0.25f;
			if (brainbuffer[4] * brainbuffer[4] + brainbuffer[5] * brainbuffer[5] < 64) {
				brainbuffer[6] = 0;
				f = brainbuffer[4];
				if (f >= 0) brainbuffer[6] -= brainbuffer[5] > 0 ? 1.0f : -1.0f;
				else {
					f = brainbuffer[5] / f;
					if (f > 1.0f) f = 1.0f;
					else if (f < -1.0f) f = -1.0f;
					brainbuffer[6] += f;
				}
			}
		}
		else {
			brainbuffer[6] = brainbuffer[7] - 1.0f + randf() - 0.5f;
			brainbuffer[6] = makedata(brainbuffer[6]);
			/*if ((clock() & 0xff) == 0)
				brainbuffer[6] += randf() - 0.5f;*/
		}
		brainbuffer[7] = 1.0f;
		brainbuffer[2] = 0;
		brainbuffer[3] = 0;
		brainbuffer[4] = 0;
		brainbuffer[5] = 0;
	}

	virtual animal* Giveclone() override {
		return new programmedanimal(*this);
	}

};

