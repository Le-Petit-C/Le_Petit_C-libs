
#ifndef LPCalgorithm_h
#define LPCalgorithm_h

#include <vector>
//#include <algorithm>
#include <functional>
#include <type_traits>

namespace LPC {
	//compare返回true表示第一个参数应该更远离根，返回false表示同级或第二个参数应该更远离根
	template<typename T, typename container = std::vector<T>, typename compare = std::less<T>>
	class heap;
}

template<typename T, typename cont, typename comp>
class LPC::heap : public cont{
public:
	using deftype = T;
	using container_t = cont;
	using compare_t = comp;
	static constexpr const compare_t compare = compare_t();
	//要求：container支持无参数构造函数
	heap();
	//要求：container支持移动构造函数
	heap(container_t&&);
	size_t resort_node_toleaves(size_t index);
	size_t resort_node_toroot(size_t index);
	size_t resort_node(size_t index);
	void make_heap();
	template<typename ...U>
	void emplace(U...);
	void push(T val);
	void pop();
	void pop(size_t);
	inline T& top();
	inline T pop_get();
	inline T pop_get(size_t index);
};

template<typename T, typename cont, typename comp>
LPC::heap<T, cont, comp>::heap(){}

template<typename T, typename cont, typename comp>
LPC::heap<T, cont, comp>::heap(container_t&& c) : cont(std::move(c)){
	make_heap();
}

template<typename T, typename cont, typename comp>
size_t LPC::heap<T, cont, comp>::resort_node_toleaves(size_t index) {
	size_t nextindex;
again:
	nextindex = (index << 1) + 1;
	if (nextindex < this->size()) {
		if (nextindex + 1 < this->size()) {
			if (compare((*this)[nextindex], (*this)[nextindex + 1])) ++nextindex;
			if (compare((*this)[index] , (*this)[nextindex])) {
				std::swap((*this)[nextindex], (*this)[index]);
				index = nextindex;
				goto again;
			}
		}
		else{
			if (compare((*this)[index], (*this)[nextindex])) {
				std::swap((*this)[index], (*this)[nextindex]);
				index = nextindex;
			}
		}
	}
	return index;
}

template<typename T, typename cont, typename comp>
size_t LPC::heap<T, cont, comp>::resort_node_toroot(size_t index) {
	while (index != 0) {
		size_t nextindex = (index - 1) >> 1;
		if (compare((*this)[index], (*this)[nextindex])) break;
		std::swap((*this)[index], (*this)[nextindex]);
		index = nextindex;
	}
	return index;
}

template<typename T, typename cont, typename comp>
size_t LPC::heap<T, cont, comp>::resort_node(size_t index) {
	size_t _index = resort_node_toroot(index);
	if (_index == index)
		return resort_node_toleaves(index);
	else return _index;
}

template<typename T, typename cont, typename comp>
void LPC::heap<T, cont, comp>::make_heap() {
	//std::make_heap(container[0], container[container.size()], compare_t());
	for (size_t a = 1; a < this->size(); ++a)
		resort_node_toroot(a);
}

template<typename T, typename cont, typename comp>
template<typename ...U>
void LPC::heap<T, cont, comp>::emplace(U... args) {
	(*this).emplace_back(args...);
}

template<typename T, typename cont, typename comp>
void LPC::heap<T, cont, comp>::push(T val) {
	(*this).push_back(std::move(val));
	resort_node_toroot(this->size() - 1);
}

template<typename T, typename cont, typename comp>
void LPC::heap<T, cont, comp>::pop() {
	std::swap((*this)[0], (*this)[this->size() - 1]);
	this->pop_back();
	resort_node_toleaves(0);
}

template<typename T, typename cont, typename comp>
void LPC::heap<T, cont, comp>::pop(size_t index) {
	std::swap((*this)[index], (*this)[this->size() - 1]);
	this->pop_back();
	resort_node(index);
}

template<typename T, typename cont, typename comp>
inline T& LPC::heap<T, cont, comp>::top() {
	return (*this)[0];
}

template<typename T, typename cont, typename comp>
inline T LPC::heap<T, cont, comp>::pop_get() {
	T ret = std::move(top());
	pop();
	return ret;
}

template<typename T, typename cont, typename comp>
inline T LPC::heap<T, cont, comp>::pop_get(size_t index) {
	T ret = std::move((*this)[index]);
	pop(index);
	return ret;
}

#endif
