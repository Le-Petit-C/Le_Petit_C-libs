

#include <windows.h>
#include <malloc.h>
#include <conio.h>
#include "LPCdebug.h"

extern int __cdecl main(int argc, char** argv);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
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
    int ret = 0;
    ret = main(argc, argv);
    for (int a = 0; a < argc; ++a)
        free(argv[a]);
    free(argv);
    if (LPC::debugOutputAllocated) {
        printf("\n主线程执行完毕，按任意键退出主线程...\n");
        char c = _getch();
    }
    return ret;
}
