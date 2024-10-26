
#include <iostream>

class c1;
class c2;

int puta();
int putb();
int setb();

static int b;

class c2 {
	static int n;
};

class c1 {
public:
	static int a;
};

int c2::n = putb();
int c1::a = setb();


int puta() {
	std::cout << c1::a << std::endl;
	return 0;
}
int putb() {
	std::cout << b << std::endl;
	return 0;
}
int setb() {
	b = 2;
	return 0;
}

int main() {

}
