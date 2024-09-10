
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
	//MessageBoxA(NULL, "这是第一行\n这是第二行", "标题", MB_OK);
	heap<int> h;
	while (true) {
		int n;
		dprintf("请输入操作代码:");
		dscanf_s("%d", &n);
		switch (n) {
		case 0:
			break;
		case 1:
			dprintf("输入数据:");
			dscanf_s("%d", &n);
			h.push(n);
			putall(h);
			continue;
		case 2:
			dprintf("弹出了%d\n", h.pop_get());
			putall(h);
			continue;
		case 3:
			dprintf("更改:");
			dscanf_s("%d", &n);
			dprintf("将%d改为", h[n]);
			dscanf_s("%d", &h[n]);
			putall(h);
			h.resort_node(n);
			dprintf("整理后:\n");
			putall(h);
			continue;
		default:
			dprintf("不是有效的操作数代码!\n");
			continue;
		}
		break;
	}
	dprintf("已退出，最终全部弹出顺序:\n");
	while (h.size() != 0)
		dprintf("%d ", h.pop_get());
	return 0;
}
