
#include "pch.h"
#include <conio.h>

extern "C" int __cdecl main(int argc, char **argv);

int LPCinitapiA(){
    int argc;
    wchar_t** argvw;
    argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    char** argv = (char**)malloc(sizeof(char*) * argc);
    if (argv == 0)
        return main(0, 0);
    for (int a = 0; a < argc; ++a) {
        size_t len;
        errno_t err;
        err = wcstombs_s(&len, 0, 0, argvw[a], _TRUNCATE);
        argv[a] = (char*)malloc(len + 1);
        err = wcstombs_s(&len, argv[a], len + 1, argvw[a], _TRUNCATE);
    }
    LocalFree(argvw);
    int ret = main(argc, argv);
    for (int a = 0; a < argc; ++a)
        free(argv[a]);
    free(argv);
    if (LPC::debugOutputAllocated) {
        printf("\n主线程执行完毕，返回值为%d(%x)，按任意键退出主线程...\n", ret, ret);
        char c = _getch();
    }
    return ret;
}
