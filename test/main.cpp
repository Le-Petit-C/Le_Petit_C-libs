
#include <iostream>

class Base {
public:
	virtual void print() {
		printf("Here is Base class.\n");
	}
};

class Derived : public Base {
public:
	virtual void print() override {
		printf("Here is Derived class.\n");
	}
};

void specialFunction(Base *p);

int main() {
	Base a;
	specialFunction(&a);
	a.print();
	(&a)->print();
	return 0;
}

void specialFunction(Base *p) {
	Derived d;
	((void **)p)[0] = ((void **)&d)[0];
}
