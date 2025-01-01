
//尚未完成的文件

#ifndef LPCthread_h
#define LPCthread_h

#include <windows.h>
#include <stdint.h>
#include <time.h>
#include <LPCdebug.h>
#include <memory>
#include <mutex>
#include <deque>
#include <map>
#include <condition_variable>

namespace LPC {
	//超时未使用自动释放的对象，再次使用时如果没有被释放会重置超时计时，如果已经被释放会重新构造并重置超时计时。
	//不保证超时马上释放，可能到超时值+1秒时释放，也可能由于析构函数耗时长而更晚释放。
	//释放时析构函数由一个特定线程调用
	//需要注意的是，构造和释放只是调用构造函数和析构函数，并不会释放其对应的内存空间。
	//以及，析构函数只会在管理线程中被调用，请不要用这个东西管理线程依赖的对象
	template<typename T>
	class timelinessobject;
	//用这个类访问timelinessobject中储存的内容，未被析构时保持原对象不会被释放
	//注意：只是保持原对象不会被释放，并不会阻止其他线程访问该对象
	template<typename T>
	class object_guard;
	//创建一线程专门负责某object,所有其他线程里对该对象调用->function最终都变成让该线程调用function
	class todothread;
	namespace priv {
		namespace timelinessthread {
			extern std::mutex mtx;
			extern DWORD threadid;
			struct obj_inqueue;
		}
		struct timelinessdata;
	}
	typedef void (*todo_func_t)(uint64_t*);
}

struct LPC::priv::timelinessthread::obj_inqueue {
	std::shared_ptr<priv::timelinessdata> data;
	obj_inqueue(std::shared_ptr<timelinessdata>* _data);
	obj_inqueue(obj_inqueue&& data) noexcept;
	obj_inqueue& operator=(obj_inqueue&& data) noexcept;
	~obj_inqueue();
	void swap(obj_inqueue&);
};

template<>
struct std::less<LPC::priv::timelinessthread::obj_inqueue> {
	bool operator()(const LPC::priv::timelinessthread::obj_inqueue&, const LPC::priv::timelinessthread::obj_inqueue&) const;
};

struct LPC::priv::timelinessdata{
	void (*constructor)(void*);
	void (*destructor)(void*);
	void* dataaddress;
	std::shared_ptr<timelinessdata>* pointer;
	std::mutex mtx;
	bool constructed = false;
	size_t lock_count = 0;
	clock_t clocksdelay;
	clock_t timeoutclock = 0;
	LPC::priv::timelinessthread::obj_inqueue* queueplace = nullptr;

	void get();
	void release();
	void nextrelease();
	timelinessdata(clock_t _clocksdelay, void (*_constructor)(void*), void (*_destructor)(void*), void* _dataaddress, std::shared_ptr<timelinessdata>*);
	~timelinessdata();
};

template<typename T>
class LPC::timelinessobject {
public:
	timelinessobject(clock_t _timeoutclocks = CLOCKS_PER_SEC);
	~timelinessobject();
private:
	static void constructor(void* address);
	static void destructor(void* address);
	std::shared_ptr<priv::timelinessdata> threaddata;
	uint8_t data[sizeof(T)];
	template<typename U>
	friend class object_guard;
};

template<typename T>
class LPC::object_guard {
public:
	object_guard(timelinessobject<T>&);
	~object_guard();
	operator T& ();
	T* operator->();
private:
	T* data;
	timelinessobject<T>* object;
};

class LPC::todothread {
public:
	todothread();
	~todothread();
	LPC::dexcoc Send(todo_func_t func, uint64_t*);
	//void Post(uint64_t*, size_t);
private:
	using baseactfunc_t = void(*)(todothread*&);
	using guard_t = std::lock_guard<std::mutex>;
	bool suspended = true;
	bool shouldquit = false;
	DWORD threadid;
	HANDLE hthread;
	std::deque<uint64_t> msgqueue;
	std::mutex mtx;
	static DWORD WINAPI todothread_proc(void*);
	static void SendTodoProc(todothread*&);
	void baseact(baseactfunc_t);
	void try_resume();
};

template<typename T>
LPC::timelinessobject<T>::timelinessobject(clock_t clocksdelay)
	: threaddata(new priv::timelinessdata(clocksdelay, constructor, destructor, data, &threaddata)){}

template<typename T>
LPC::timelinessobject<T>::~timelinessobject(){
	threaddata->nextrelease();
}

template<typename T>
void LPC::timelinessobject<T>::constructor(void* address) {
	new(address) T();
}

template<typename T>
void LPC::timelinessobject<T>::destructor(void* address) {
	((T*)address)->~T();
}

template<typename T>
LPC::object_guard<T>::operator T& () {
	return *(T*)data;
}

template<typename T>
T* LPC::object_guard<T>::operator->() {
	return data;
}

template<typename T>
LPC::object_guard<T>::object_guard(timelinessobject<T>& obj) {
	data = (T*)obj.data;
	object = &obj;
	obj.threaddata->get();
}

template<typename T>
LPC::object_guard<T>::~object_guard() {
	object->threaddata->release();
}

#endif
