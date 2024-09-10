
#include <iostream>
#include <algorithm>
#include <LPCalgorithm.h>
#include <LPCwinmain.h>
#include <LPCdebug.h>

using namespace LPC;

void putall(const heap<int>& h) {
	for (int n = 0; n < h.size(); ++n)
		dprintf("%d ", h[n]);
	dprintf("\n");
}

int main() {
	//MessageBoxA(NULL, "���ǵ�һ��\n���ǵڶ���", "����", MB_OK);
	heap<int> h;
	while (true) {
		int n;
		dprintf("�������������:");
		dscanf_s("%d", &n);
		switch (n) {
		case 0:
			break;
		case 1:
			dprintf("��������:");
			dscanf_s("%d", &n);
			h.push(n);
			putall(h);
			continue;
		case 2:
			dprintf("������%d\n", h.pop_get());
			putall(h);
			continue;
		case 3:
			dprintf("����:");
			dscanf_s("%d", &n);
			dprintf("��%d��Ϊ", h[n]);
			dscanf_s("%d", &h[n]);
			putall(h);
			h.resort_node(n);
			dprintf("�����:\n");
			putall(h);
			continue;
		default:
			dprintf("������Ч�Ĳ���������!\n");
			continue;
		}
		break;
	}
	dprintf("���˳�������ȫ������˳��:\n");
	while (h.size() != 0)
		dprintf("%d ", h.pop_get());
	return 0;
}
