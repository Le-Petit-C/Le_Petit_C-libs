
#include <windows.h>
#include <conio.h>
#include "LPCdebug.h"

extern int __cdecl main(int argc, wchar_t** argv);

int LPCinitapiW(){
    int argc;
    wchar_t** argv;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int ret = main(argc, argv);
    LocalFree(argv);
    if (LPC::debugOutputAllocated) {
        printf("\n���߳�ִ����ϣ���������˳����߳�...\n");
        char c = _getch();
    }
    return ret;
}
