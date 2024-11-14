#pragma once

#include <windows.h>
#include <LPCwndex.h>
#include "Map.h"

class ScreenRect {
public:
	//构造函数
	ScreenRect(LPC::window& wnd, Map& map, double reserved_area);

	//窗口缩放后可能需要更新形状位置
	void ref();

	//特殊迭代器，迭代同时可以方便地获取当前方块位置
	class rectIter {
	public:
		rectIter(ScreenRect&);//构造一个从ScreenRect第一个方块开始的迭代器

		//将自己指向下一个内容
		rectIter& operator++();

		//将自己往后指几个内容
		rectIter& operator+=(size_t);

		//比较函数，可以用来判断自己是否到末尾了
		inline bool operator!=(const Map::iterator& x) const { return &**this != &*x; }
		inline bool operator==(const Map::iterator& x) const { return &**this == &*x; }
		inline bool operator!=(const rectIter& x) const { return &**this != &*x; }
		inline bool operator==(const rectIter& x) const { return &**this == &*x; }

		struct rectIterOutput {
		public:
			size_t x, y;
			int left, top, right, bottom;
			inline operator int&() { return *data; }
			inline operator const int& () const { return *data; }
		private:
			friend class rectIter;
			int* data;
		};

		inline int& operator*() { return *out.data; }
		inline const int& operator*() const { return *out.data; }
		inline rectIterOutput* operator->() { return &out; }
		inline const rectIterOutput* operator->() const { return &out; }

	private:
		rectIterOutput out;
		int resx, resy;
		ScreenRect* rect;
	};

	//使用特殊迭代器开始遍历
	rectIter beginex();

	//获取末尾迭代器，不推荐每一次比较都调用，可以保存下来
	rectIter endex();

	inline Map::iterator end() { return map->end(); }

	inline const RECT& getRect() const { return rect; }

private:
	RECT rect;
	int stepx, stepy;
	int resx, resy;
	LPC::window* wnd;
	Map* map;
	double reserved_area;
};
