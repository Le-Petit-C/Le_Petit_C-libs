#include <iostream>
#include <windows.h>

// 函数指针类型
typedef void (*TimerCallback)();

// 被调用的函数
void MyFunction() {
    std::cout << "MyFunction called from another thread!" << std::endl;
}

// 线程函数
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        std::cout << msg.message << std::endl;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

int main() {
    // 创建线程
    HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create thread!" << std::endl;
        return 1;
    }

    // 等待线程初始化
    Sleep(1000);

    // 定义消息参数
    //UINT_PTR nIDEvent = 1; // 定时器ID
    //UINT nElapse = 1000;   // 毫秒

    // 设置定时器
    //SetTimer(NULL, nIDEvent, nElapse, NULL);

    // 发送消息到线程
    std::cout << "WM_TIMER:" << WM_TIMER << std::endl;
    BOOL ret = PostThreadMessage(GetThreadId(hThread), WM_TIMER, (WPARAM)MyFunction, (LPARAM)MyFunction);
    std::cout << ret << std::endl;

    // 等待线程结束
    WaitForSingleObject(hThread, INFINITE);

    // 关闭线程句柄
    CloseHandle(hThread);

    return 0;
}