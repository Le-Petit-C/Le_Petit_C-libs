
#include <stdint.h>
#include <iostream>
#include <conio.h>
#include <LPCwinmainA.h>
#include <LPCdebug.h>
#include <LPCthread.h>

using namespace LPC;

struct c1 {
	int data = 1;
	c1() {
		std::cout << "c1 constructor" << std::endl;
	}
	void print() {
		std::cout << data << std::endl;
	}
	~c1() {
		std::cout << "c1 destructor" << std::endl;
	}
};

struct c2 {
	c1 c;
	c2() {
		std::cout << "c2 constructor" << std::endl;
	}
	~c2() {
		std::cout << "c2 destructor" << std::endl;
	}
};

void testfunc() {
	timelinessobject<c1> obj;
	while (true) {
		if (_getch() == '0') break;
		object_guard<c1> g(obj);
		g->print();
	}
}

int main(int argc, char** argv) {
	dprintf("");
	testfunc();
	return 0;
}
