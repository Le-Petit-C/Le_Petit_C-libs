
#include <LPCdebug.h>
#include <LPCwinmain.h>
#include <malloc.h>
#include <mutex>

using namespace LPC;
int n = 0;
constexpr int k = 0x7ffffff;
std::mutex mtx;
std::thread* threads[8];

void mtxtestthread() {
	while (1) {
		{
			std::lock_guard<std::mutex> mtxg(mtx);
			if (n >= k) {
				break;
			}
			++n;
		}
	}
}

void test(int nthread) {
	mtx.lock();
	n = 0;
	std::thread** threads;
	threads = (std::thread**)_malloca(sizeof(*threads) * nthread);
	if (threads == nullptr) {
		mtx.unlock();
		return;
	}
	for (int a = 0; a < nthread; ++a)
		threads[a] = new std::thread(mtxtestthread);
	clock_t t = clock();
	mtx.unlock();
	for (int a = 0; a < nthread; ++a)
		threads[a]->join();
	_freea(threads);
	t = clock() - t;
	dprintf("Num of threads : %d\n", nthread);
	//dprintf("Num of lock-unlock pairs : %d\n", k);
	//dprintf("Time costed : %lf\n", (double)t / CLOCKS_PER_SEC);
	dprintf("%lf lock-unlock pairs per second on average.\n", k / ((double)t / CLOCKS_PER_SEC));
	dprintf("\n");
}

int main() {
	dprintf("");
	for (int a = 1; a < 16; ++a) {
		test(a);
	}
	char c;
	dprintf("Press enter to exit.");
	dscanf_s("%c", &c);
	return 0;
}
