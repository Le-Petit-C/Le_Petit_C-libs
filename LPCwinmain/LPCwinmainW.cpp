
#include "pch.h"
#include <conio.h>

extern "C" int __cdecl main(int argc, wchar_t **argv);

int LPCinitapiW(){
    int argc;
    wchar_t** argv;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int ret = main(argc, argv);
    LocalFree(argv);
    if (LPC::debugOutputAllocated) {
        wprintf(L"\n���߳�ִ����ϣ�����ֵΪ%d(%x)����������˳����߳�...\n", ret, ret);
        char c = _getch();
    }
    return ret;
}
