
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

//SendTodo,ִ����ɻ���ԭ�߳�
void LPC::todothread::SendTodoProc(todothread*& me)  _Requires_lock_held_(me->mtx) {
	//���ݶ��е�һ���洢����ָ��
	todo_func_t func = (todo_func_t)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//���ݶ��еڶ����洢�Զ������
	uint64_t* data = (uint64_t*)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//���ݶ��е������洢ԭ�߳̾��
	HANDLE hthread = (HANDLE)(uintptr_t)me->msgqueue.front();
	me->msgqueue.pop_front();
	//������ִ�к�������
	me->mtx.unlock();
	func(data);
	DWORD resumereturn;
	//ִ���껽��ԭ�߳�
	do resumereturn = ResumeThread(hthread); while (resumereturn == 0);
	if (resumereturn < 0) {
		//����ʧ�ܣ��׳��쳣
		setexception("SendTodoProc:ResumeThread failed unexpectedly.");
		throw;
	}
	//�����������˻�ѭ���н�����һ������
	me->mtx.lock();
}

//�ڲ�ִ��ʱ�õģ�����ʱִ��
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
* �����￪ʼ�ǡ�ʱЧ�Զ��󡱵�ʵ�ִ���
**********************************************************************************************/

namespace LPC {
	namespace priv {
		namespace timelinessthread {
			struct threaddata;
			threaddata* thread;
			//��������������������������
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
	DWORD threadid;//�����̵߳��߳�id
	HANDLE hthread;//�����̵߳��߳̾��
	size_t objcount = 0;//ʱЧ�Զ������
	LPC::heap<obj_inqueue, std::vector<obj_inqueue>, std::less<obj_inqueue>> releasequeue;//�ȴ��ͷŵĶ����
	threaddata() {//�����߳�
		hthread = CreateThread(nullptr, 0, threadproc, 0, 0, &threadid);
	}
	~threaddata() {//�ͷ��߳�
		CloseHandle(hthread);
	}
};

LPC::priv::timelinessdata::timelinessdata(clock_t _clocksdelay, void (*_constructor)(void*), void (*_destructor)(void*), void* _dataaddress, std::shared_ptr<timelinessdata>* ptr)
	: clocksdelay(_clocksdelay), constructor(_constructor), destructor(_destructor), dataaddress(_dataaddress), pointer(ptr) {
	staticmtx.lock();
	//��������̲߳������򴴽������߳�
	if (thread == nullptr)
		thread = new threaddata();
	//���������һ
	++thread->objcount;
	staticmtx.unlock();
}

LPC::priv::timelinessdata::~timelinessdata(){
	//dprintf("timelinessdata destructor called.\n");
	//����ʱҪô���Ѿ��ͷ��˶�û�����Ŷӣ�Ҫô���ڶ�������ŶӲ��౻�������Ͳ����ٹ�queueplace��
	mtx.lock();
	//���������һ
	--thread->objcount;
	if (constructed) {
		//�ͷ���Դ
		destructor(dataaddress);
		constructed = false;
	}
	mtx.unlock();
}

static DWORD WINAPI LPC::priv::timelinessthread::threadproc(void*) {
	constexpr const int t = 3;
	int count = t;//���߳��˳�ʱ��
	while(true) {
		Sleep(1000);
		//dprintf("123\n");
		staticmtx.lock();
		if (thread->objcount == 0) {
			//û�ж������̹߳�������ʱ��1
			//������ʱ�������˳��߳�
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
			if (thread->releasequeue.size() == 0) {//������û�����ˣ���ȻҪ�˳�
				staticmtx.unlock();
				break;
			}
			std::shared_ptr<LPC::priv::timelinessdata> data(thread->releasequeue.top().data);
			// Ҫ�ǳ�С��������
			// �涨Ҫͬʱ�����кͶ���ʱ������������
			// �������󲻵������У�ֻ���ͷŶ��У��������ͷ��˶���
			data->mtx.lock();
			if (data->lock_count != 0) {//�����������ʹ���У��Ǿ����߳�����
				thread->releasequeue.pop();
				data->mtx.unlock();
				continue;
			}
			if (clock() - data->timeoutclock < 0) {//��ʱʱ���������δ��ʱ��˵��û�г�ʱ���ˣ��˳�ѭ��
				staticmtx.unlock();
				data->mtx.unlock();
				break;
			}
			//û������ʹ�ã�û�С�δ��ʱ������ֻ���ǳ�ʱδ��ʹ����
			//��ʱδ��ʹ�ã������ö���
			thread->releasequeue.pop();
			staticmtx.unlock();
			data->destructor(data->dataaddress);
			data->constructed = false;
			data->mtx.unlock();
			staticmtx.lock();
		}
		count = t;//���õ���ʱ
	};
}

//�����Ŷӵȴ����ͷţ���Ҫʱ�̸������λ��
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

void LPC::priv::timelinessdata::get() {//����������1��ȷ���������
	mtx.lock();
	if (!constructed) {
		constructor(dataaddress);
		constructed = true;
	}
	++lock_count;
	mtx.unlock();
}

void LPC::priv::timelinessdata::release() {//����������1��ȷ������ȴ��ͷŵĶ�����
	mtx.lock();
	timeoutclock = clock() + clocksdelay;//���ü�ʱ
	--lock_count;
	if (lock_count == 0 && queueplace == nullptr) {
		//�����û��������Ͱ��Լ��ŵ�����ȥ
		mtx.unlock();
		staticmtx.lock();
		thread->releasequeue.emplace(pointer);
		//dprintf("emplaced\n");
		staticmtx.unlock();
	}
	else mtx.unlock();
}

void LPC::priv::timelinessdata::nextrelease() {//������timelinessobject�����ˣ����������ͷ�
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
* ��ʱЧ�Զ��󡱵�ʵ�ִ��뵽�������
**********************************************************************************************/
