
//��δ��ɵ��ļ�

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
	//��ʱδʹ���Զ��ͷŵĶ����ٴ�ʹ��ʱ���û�б��ͷŻ����ó�ʱ��ʱ������Ѿ����ͷŻ����¹��첢���ó�ʱ��ʱ��
	//����֤��ʱ�����ͷţ����ܵ���ʱֵ+1��ʱ�ͷţ�Ҳ������������������ʱ���������ͷš�
	//�ͷ�ʱ����������һ���ض��̵߳���
	//��Ҫע����ǣ�������ͷ�ֻ�ǵ��ù��캯���������������������ͷ����Ӧ���ڴ�ռ䡣
	//�Լ�����������ֻ���ڹ����߳��б����ã��벻Ҫ��������������߳������Ķ���
	template<typename T>
	class timelinessobject;
	//����������timelinessobject�д�������ݣ�δ������ʱ����ԭ���󲻻ᱻ�ͷ�
	//ע�⣺ֻ�Ǳ���ԭ���󲻻ᱻ�ͷţ���������ֹ�����̷߳��ʸö���
	template<typename T>
	class object_guard;
	//����һ�߳�ר�Ÿ���ĳobject,���������߳���Ըö������->function���ն�����ø��̵߳���function
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
