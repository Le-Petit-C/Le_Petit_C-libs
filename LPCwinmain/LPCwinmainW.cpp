
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
        wprintf(L"\n主线程执行完毕，返回值为%d(%x)，按任意键退出主线程...\n", ret, ret);
        char c = _getch();
    }
    return ret;
}
