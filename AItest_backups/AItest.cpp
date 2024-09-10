
#include <LPCgamewnd.h>
#include "Objects.h"

using namespace LPC;

size_t foodlimit;

static void setdata() {
	foodregeneration = 32.0f;
	mapwidth = SCRW / stretch;
	mapheight = SCRH / stretch;
	foodlimit = mapwidth * mapheight / 4.0f / foodregeneration;
}

volatile bool shouldexit = true;

int event(unsigned int type, intptr_t p1, intptr_t p2) {
	switch (type) {
	case WND_MESSAGE:
		switch (LPC::GetMSGCode()) {
		case WM_DESTROY:
			shouldexit = true;
			while (shouldexit) Sleep(0);
			break;
		}
		break;
	case WD_SIZE:
		setdata();
		break;
	}
	return 0;
}

std::mutex drawmtx;
volatile bool waitingfordraw = false;

static void timerproc() {
	//static float t = 0.0f;
	//float dt = (float)clock() / CLOCKS_PER_SEC;
	//if (t == 0.0f) t = dt;
	//dt -= t;
	//t += dt;
	float dt = 0.016;
	float k = dt * 200 / foodregeneration;
	LPC::Clear(0);
	waitingfordraw = true;
	drawmtx.lock();
	waitingfordraw = false;
	while(foods.size() <= foodlimit && (k += log(randf())) > 0)
		foods.emplace_back(new food);
	foods.paint();
	animals.paint();
	attacks.paint();
	defences.paint();
	drawmtx.unlock();
	LPC::ref();
}

static void actthread() {
	time_t t = clock();
	float dt = 0.02;
	groupActThread::init(20);
	shouldexit = false;
	while (!shouldexit) {
		t += (time_t)(dt * CLOCKS_PER_SEC);
		//do Sleep(0);
		time_t currentt = clock();
		if (t - currentt > 0) Sleep((unsigned long long)(t - currentt) * 1000 / CLOCKS_PER_SEC);
		else if (currentt - t > CLOCKS_PER_SEC) t = currentt - CLOCKS_PER_SEC;
		while (waitingfordraw);
		drawmtx.lock();
		attacks.timer(dt * 4.0f);
		defences.timer(dt * 4.0f);
		animals.timer(dt * 4.0f);
		animals.aftertimer(dt * 4.0f);
		foods.testdead();
		animals.testdead();
		attacks.testdead();
		defences.testdead();
		drawmtx.unlock();
	}
	groupActThread::exit();
	shouldexit = false;
}

LPC::timer<> _timer(1, timerproc, true);

#define MYAIK 9
#define OLDAIK 18

void init() {
	setdata();
	srand((unsigned int)time(0));
	if (rand() & 1) {
		//animals.emplace_back(new programmedanimal);
		for (int a = 0; a < 128; ++a) {
			animals.emplace_back(new AIanimal1<MYAIK>);
			animals.emplace_back(new AIanimal2<OLDAIK>);
		}
	}
	else if (rand() & 1) {
		for (int a = 0; a < 256; ++a) {
			animals.emplace_back(new AIanimal1<MYAIK>);
		}
	}
	else{
		for (int a = 0; a < 256; ++a) {
			animals.emplace_back(new AIanimal2<OLDAIK>);
		}
	}
	for (int a = 0; a < foodlimit / 4; ++a)
		foods.emplace_back(new food);
	std::thread(actthread).detach();
	while (shouldexit) Sleep(0);
}
