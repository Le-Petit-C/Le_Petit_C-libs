
#include <LPCwinmain.h>
#include <LPCdebug.h>
#include <LPCcomplex.h>

int main() {
	LPC::dprintf("");
	LPC::complex<int> c(8, 1);
	LPC::complex<int> d(2, 1);
	std::cout << c / d << std::endl;
	return 0;
}
