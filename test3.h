#include <iostream>
#include <windows.h>

// ����ָ������
typedef void (*TimerCallback)();

// �����õĺ���
void MyFunction() {
    std::cout << "MyFunction called from another thread!" << std::endl;
}

// �̺߳���
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    // ��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        std::cout << msg.message << std::endl;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

int main() {
    // �����߳�
    HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create thread!" << std::endl;
        return 1;
    }

    // �ȴ��̳߳�ʼ��
    Sleep(1000);

    // ������Ϣ����
    //UINT_PTR nIDEvent = 1; // ��ʱ��ID
    //UINT nElapse = 1000;   // ����

    // ���ö�ʱ��
    //SetTimer(NULL, nIDEvent, nElapse, NULL);

    // ������Ϣ���߳�
    std::cout << "WM_TIMER:" << WM_TIMER << std::endl;
    BOOL ret = PostThreadMessage(GetThreadId(hThread), WM_TIMER, (WPARAM)MyFunction, (LPARAM)MyFunction);
    std::cout << ret << std::endl;

    // �ȴ��߳̽���
    WaitForSingleObject(hThread, INFINITE);

    // �ر��߳̾��
    CloseHandle(hThread);

    return 0;
}