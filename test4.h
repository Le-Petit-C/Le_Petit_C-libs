
#include <stdio.h>
#include <conio.h>

//#define WCHAR_TEST 1

#ifdef WCHAR_TEST

int main(int argc, wchar_t** argv) {
	for (int a = 0; a < argc; ++a)
		wprintf(L"%s\n", argv[a]);
	char c = _getch();
	return 0;
}

#else

int main(int argc, char** argv) {
	for (int a = 0; a < argc; ++a)
		printf("%s\n", argv[a]);
	char c = _getch();
	return 0;
}

#endif
