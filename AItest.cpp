
#include <LPCgamewnd.h>

using namespace LPC;

#include "Objects.h"

static void setdata() {
	mapwidth = SCRW / stretch;
	mapheight = SCRH / stretch;
	foodlimit = mapwidth * mapheight / foodregeneration * maxfooddensity;
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
	LPC::Clear(0);
	waitingfordraw = true;
	drawmtx.lock();
	waitingfordraw = false;
	foods.paint();
	prey.paint();
	predators.paint();
	drawmtx.unlock();
	LPC::ref();
}

static void actthread() {
	double t = clock();
	float dt = 1.0f / aimedcalculatespeed;
	groupActThread::init(threadcount);
	shouldexit = false;
	while (!shouldexit) {
		float kdt = dt * ktimespeed;
		t += CLOCKS_PER_SEC * dt;
		//do Sleep(0);
		time_t currentt = clock();
		if (t - currentt > 0) Sleep((unsigned long long)(t - currentt) * 1000 / CLOCKS_PER_SEC);
		else if (currentt - t > CLOCKS_PER_SEC) t = currentt - CLOCKS_PER_SEC;
		while (waitingfordraw) std::this_thread::yield();
		drawmtx.lock();
		float k = dt * mapwidth * mapheight / foodregeneration * foodregenerate_densityspeed;
		while (foods.size() <= foodlimit && (k += log(randf())) > 0)
			foods.emplace_back(new food);
		prey.timer(kdt);
		predators.timer(kdt);
		prey.aftertimer(kdt);
		predators.aftertimer(kdt);
		prey.testdead();
		predators.testdead();
		foods.testdead();
		drawmtx.unlock();
	}
	groupActThread::exit();
	shouldexit = false;
}

LPC::timer<> _timer(1, timerproc, true);

#define MYAIK 9
#define OLDAIK 18

static bool fgetbool(FILE* fp) {
	char buf[256] = {};
	fscanf_s(fp, "%s", buf, (uint32_t)sizeof(buf));
	return !((strcmp(buf, "false") == 0) || (strcmp(buf, "0") == 0));
}

bool enable_AI1 = true;
bool enable_AI2 = true;
bool enable_AI3 = true;
bool enable_programmedAI = false;

void init() {

	//¶ÁÈ¡ÅäÖÃÎÄ¼þ
	char buf[256] = {};
	constexpr uint32_t bufsize = sizeof(buf);
	FILE* fp;
	fopen_s(&fp, "config.txt", "r");
	if (fp) {
		while (fscanf_s(fp, "%[^ =]", buf, bufsize) > 0) {
			char c;
			do c = fgetc(fp); while (c != '=' && c != EOF);
			if (c == EOF) break;
			if (strcmp(buf, "stretch") == 0)
				fscanf_s(fp, "%f", &stretch);
			else if (strcmp(buf, "enable_AI1") == 0)
				enable_AI1 = fgetbool(fp);
			else if (strcmp(buf, "enable_AI2") == 0)
				enable_AI2 = fgetbool(fp);
			else if (strcmp(buf, "enable_AI3") == 0)
				enable_AI3 = fgetbool(fp);
			else if (strcmp(buf, "enable_programmedAI") == 0)
				enable_programmedAI = fgetbool(fp);
			else if (strcmp(buf, "foodregeneration") == 0)
				fscanf_s(fp, "%f", &foodregeneration);
			else if (strcmp(buf, "maxfooddensity") == 0)
				fscanf_s(fp, "%f", &maxfooddensity);
			else if (strcmp(buf, "foodregenerate_densityspeed") == 0)
				fscanf_s(fp, "%f", &foodregenerate_densityspeed);
			else if (strcmp(buf, "initfooddensity") == 0)
				fscanf_s(fp, "%f", &initfooddensity);
			else if (strcmp(buf, "aimedcalculatespeed") == 0)
				fscanf_s(fp, "%f", &aimedcalculatespeed);
			else if (strcmp(buf, "ktimespeed") == 0)
				fscanf_s(fp, "%f", &ktimespeed);
			else if (strcmp(buf, "threadcount") == 0)
				fscanf_s(fp, "%d", &threadcount);
			else if (strcmp(buf, "initrandomk") == 0)
				fscanf_s(fp, "%f", &initrandomk);
			else if (strcmp(buf, "initpreycount") == 0)
				fscanf_s(fp, "%d", &initpreycount);
			else if (strcmp(buf, "initpredatorcount") == 0)
				fscanf_s(fp, "%d", &initpredatorcount);
			else if (strcmp(buf, "predatormaxspeed") == 0)
				fscanf_s(fp, "%f", &predatormaxspeed);
			else if (strcmp(buf, "preymaxspeed") == 0)
				fscanf_s(fp, "%f", &preymaxspeed);
			else if (strcmp(buf, "initmutaterate") == 0)
				fscanf_s(fp, "%f", &initmutaterate);
			else if (strcmp(buf, "initmutaterange") == 0)
				fscanf_s(fp, "%f", &initmutaterange);
			else if (strcmp(buf, "mutatemutaterange") == 0)
				fscanf_s(fp, "%f", &mutatemutaterange);
			do c = fgetc(fp); while (c != '\n' && c != EOF);
		}
		fclose(fp);
	}

	setdata();
	srand((unsigned int)time(0));
	for (int a = 0; a < initpreycount; ++a) {
		if (enable_AI1)
			prey.emplace_back(new AIanimal1(9, false));
		if (enable_AI2)
			prey.emplace_back(new AIanimal2(18, false));
		if (enable_AI3)
			prey.emplace_back(new AIanimal3(78, false));
		if (enable_programmedAI)
			prey.emplace_back(new programmedanimal(false));
	}
	for (int a = 0; a < initpredatorcount; ++a) {
		if (enable_AI1)
			predators.emplace_back(new AIanimal1(9, true));
		if (enable_AI2)
			predators.emplace_back(new AIanimal2(18, true));
		if (enable_AI3)
			predators.emplace_back(new AIanimal3(78, true));
		if (enable_programmedAI)
			predators.emplace_back(new programmedanimal(true));
	}
	for (int a = 0; a < mapwidth * mapheight / foodregeneration * initfooddensity; ++a)
		foods.emplace_back(new food);
	std::thread(actthread).detach();
	while (shouldexit) Sleep(0);
}
