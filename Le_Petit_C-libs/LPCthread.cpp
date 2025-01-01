
#include "pch.h"

LPC::todothread::todothread() {
	hthread = CreateThread(nullptr, 0, LPC::todothread::todothread_proc, this, 0, &threadid);
}

LPC::todothread::~todothread() {
	mtx.lock();
	shouldquit = true;
	try_resume();
	CloseHandle(hthread);
	mtx.unlock();
}

struct SendDataStruct {
	LPC::todo_func_t func;
	uint64_t* data;
	HANDLE threadhandle;
};

LPC::dexcoc LPC::todothread::Send(todo_func_t func, uint64_t* data) {
	mtx.lock();
	msgqueue.push_back((uint64_t)SendTodoProc);
	msgqueue.push_back((uint64_t)func);
	msgqueue.push_back((uint64_t)data);
	msgqueue.push_back((uint64_t)GetCurrentThreadId());
	mtx.unlock();
	return false;
}

DWORD WINAPI LPC::todothread::todothread_proc(void* data) {
	LPC::todothread*& me = (LPC::todothread*&)data;
	while (!me->shouldquit) {
		me->mtx.lock();
		while (me->msgqueue.size() > 0) {
			baseactfunc_t func = (baseactfunc_t)(uintptr_t)me->msgqueue.front();
			me->msgqueue.pop_front();
			func(me);
		}
		me->suspended = true;
		me->mtx.unlock();
		SuspendThread(me->hthread);
	}
	return 0;
}

//SendTodo,执行完成唤醒原线程
void LPC::todothread::SendTodoProc(todothread*& me)  _Requires_lock_held_(me->mtx) {
	//数据队列第一个存储函数指针
	todo_func_t func = (todo_func_t)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//数据队列第二个存储自定义参数
	uint64_t* data = (uint64_t*)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//数据队列第三个存储原线程句柄
	HANDLE hthread = (HANDLE)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//解锁后执行函数内容
	me->mtx.unlock();
	func(data);
	DWORD resumereturn;
	//执行完唤醒原线程
	do resumereturn = ResumeThread(hthread); while (resumereturn == 0);
	if (resumereturn < 0) {
		//唤醒失败，抛出异常
		setexception("SendTodoProc:ResumeThread failed unexpectedly.");
		throw;
	}
	//重新锁定，退回循环中进行下一步操作
	me->mtx.lock();
}

//内部执行时用的，锁定时执行
void LPC::todothread::baseact(baseactfunc_t func) {
	mtx.lock();
	msgqueue.push_back((uint64_t)func);
	mtx.unlock();
}

void LPC::todothread::try_resume() {
	if (suspended) {
		DWORD count;
		do {
			count = ResumeThread(hthread);
		} while (count != 1 && count != -1);
		if (count == -1) {
			setexception("Resume todothread failed! Check if you used todothread object after it has been destructed.");
			throw;
		}
		else suspended = false;
	}
}

/*********************************************************************************************
* 从这里开始是“时效性对象”的实现代码
**********************************************************************************************/

namespace LPC {
	namespace priv {
		namespace timelinessthread {
			struct threaddata;
			threaddata* thread;
			//这个锁是用来锁上面这个东西的
			std::mutex mtx;
			static DWORD WINAPI threadproc(void*);
		}
	}
}

using LPC::priv::timelinessthread::thread;
using LPC::priv::timelinessthread::threaddata;
using LPC::priv::timelinessthread::obj_inqueue;
#define staticmtx LPC::priv::timelinessthread::mtx

struct LPC::priv::timelinessthread::threaddata {
	DWORD threadid;//管理线程的线程id
	HANDLE hthread;//管理线程的线程句柄
	size_t objcount = 0;//时效性对象计数
	LPC::heap<obj_inqueue, std::vector<obj_inqueue>, std::less<obj_inqueue>> releasequeue;//等待释放的对象堆
	threaddata() {//创建线程
		hthread = CreateThread(nullptr, 0, threadproc, 0, 0, &threadid);
	}
	~threaddata() {//释放线程
		CloseHandle(hthread);
	}
};

LPC::priv::timelinessdata::timelinessdata(clock_t _clocksdelay, void (*_constructor)(void*), void (*_destructor)(void*), void* _dataaddress, std::shared_ptr<timelinessdata>* ptr)
	: clocksdelay(_clocksdelay), constructor(_constructor), destructor(_destructor), dataaddress(_dataaddress), pointer(ptr) {
	staticmtx.lock();
	//如果管理线程不存在则创建管理线程
	if (thread == nullptr)
		thread = new threaddata();
	//对象计数加一
	++thread->objcount;
	staticmtx.unlock();
}

LPC::priv::timelinessdata::~timelinessdata(){
	//dprintf("timelinessdata destructor called.\n");
	//析构时要么是已经释放了而没有在排队，要么是在队列里的排队残余被弹出，就不用再管queueplace了
	mtx.lock();
	//对象计数减一
	--thread->objcount;
	if (constructed) {
		//释放资源
		destructor(dataaddress);
		constructed = false;
	}
	mtx.unlock();
}

static DWORD WINAPI LPC::priv::timelinessthread::threadproc(void*) {
	constexpr const int t = 3;
	int count = t;//计线程退出时间
	while(true) {
		Sleep(1000);
		//dprintf("123\n");
		staticmtx.lock();
		if (thread->objcount == 0) {
			//没有东西供线程管理，倒计时减1
			//若倒计时结束则退出线程
			if (--count == 0) {
				delete thread;
				thread = nullptr;
				staticmtx.unlock();
				return 0;
			}
			staticmtx.unlock();
			continue;
		}
		else while (true) {
			if (thread->releasequeue.size() == 0) {//队列里没东西了，当然要退出
				staticmtx.unlock();
				break;
			}
			std::shared_ptr<LPC::priv::timelinessdata> data(thread->releasequeue.top().data);
			// 要非常小心死锁！
			// 规定要同时锁队列和对象时必须先锁队列
			// 锁完对象后不得锁队列，只能释放队列，除非先释放了对象
			data->mtx.lock();
			if (data->lock_count != 0) {//这个对象正在使用中，那就先踢出队列
				thread->releasequeue.pop();
				data->mtx.unlock();
				continue;
			}
			if (clock() - data->timeoutclock < 0) {//超时时间最近的尚未超时，说明没有超时的了，退出循环
				staticmtx.unlock();
				data->mtx.unlock();
				break;
			}
			//没有正在使用，没有“未超时”，那只能是超时未被使用了
			//超时未被使用，析构该对象
			thread->releasequeue.pop();
			staticmtx.unlock();
			data->destructor(data->dataaddress);
			data->constructed = false;
			data->mtx.unlock();
			staticmtx.lock();
		}
		count = t;//重置倒计时
	};
}

//对象排队等待被释放，需要时刻更新相对位置
LPC::priv::timelinessthread::obj_inqueue::obj_inqueue(std::shared_ptr<timelinessdata>* _data) : data(*_data){
	data->queueplace = this;
}

LPC::priv::timelinessthread::obj_inqueue::obj_inqueue(obj_inqueue&& _data) noexcept {
	*this = std::move(_data);
}

LPC::priv::timelinessthread::obj_inqueue& LPC::priv::timelinessthread::obj_inqueue::operator=(obj_inqueue&& _data) noexcept {
	data = _data.data;
	if(data != nullptr)
		data->queueplace = this;
	_data.data = nullptr;
	return *this;
}

LPC::priv::timelinessthread::obj_inqueue::~obj_inqueue() {
	if (data == nullptr) return;
	data->queueplace = nullptr;
	data = nullptr;
}

bool std::less<LPC::priv::timelinessthread::obj_inqueue>::operator()(const obj_inqueue& v1, const obj_inqueue& v2) const {
	return v1.data->timeoutclock > v2.data->timeoutclock;
}

void LPC::priv::timelinessdata::get() {//锁定计数加1并确保对象存在
	mtx.lock();
	if (!constructed) {
		constructor(dataaddress);
		constructed = true;
	}
	++lock_count;
	mtx.unlock();
}

void LPC::priv::timelinessdata::release() {//锁定计数减1并确保加入等待释放的队列中
	mtx.lock();
	timeoutclock = clock() + clocksdelay;//重置计时
	--lock_count;
	if (lock_count == 0 && queueplace == nullptr) {
		//如果还没进到堆里就把自己放到堆里去
		mtx.unlock();
		staticmtx.lock();
		thread->releasequeue.emplace(pointer);
		//dprintf("emplaced\n");
		staticmtx.unlock();
	}
	else mtx.unlock();
}

void LPC::priv::timelinessdata::nextrelease() {//所属的timelinessobject析构了，让它尽快释放
	mtx.lock();
	if (queueplace == nullptr){
		mtx.unlock();
		return;
	}
	mtx.unlock();
	staticmtx.lock();
	mtx.lock();
	timeoutclock = clock();
	thread->releasequeue.resort_node_toroot((size_t)(&thread->releasequeue[0] - queueplace) / sizeof(*queueplace));
	mtx.unlock();
	staticmtx.unlock();
}

void LPC::priv::timelinessthread::obj_inqueue::swap(obj_inqueue& obj) {
	data.swap(obj.data);
}

/*********************************************************************************************
* “时效性对象”的实现代码到这里结束
**********************************************************************************************/
