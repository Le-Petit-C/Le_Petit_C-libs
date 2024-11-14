#pragma once

#include <windows.h>
#include <LPCwndex.h>
#include "Map.h"

class ScreenRect {
public:
	//���캯��
	ScreenRect(LPC::window& wnd, Map& map, double reserved_area);

	//�������ź������Ҫ������״λ��
	void ref();

	//���������������ͬʱ���Է���ػ�ȡ��ǰ����λ��
	class rectIter {
	public:
		rectIter(ScreenRect&);//����һ����ScreenRect��һ�����鿪ʼ�ĵ�����

		//���Լ�ָ����һ������
		rectIter& operator++();

		//���Լ�����ָ��������
		rectIter& operator+=(size_t);

		//�ȽϺ��������������ж��Լ��Ƿ�ĩβ��
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

	//ʹ�������������ʼ����
	rectIter beginex();

	//��ȡĩβ�����������Ƽ�ÿһ�αȽ϶����ã����Ա�������
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
