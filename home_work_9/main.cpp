#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <locale.h>
#include "file_mgr.h"
#include "err_no.h"

int _tmain(int argc, TCHAR *argv[])
{
    int ret = ERR_SUCCESS;

    if (argc < 2)
    {
        _tprintf(TEXT("please input the directory.\n"));
        return 0;
    }
    _tprintf(TEXT("the directory is %s\n"), argv[1]);
    setlocale(LC_ALL, "");

    ret = DeleteDirectoryEx(argv[1]);
    if (ERR_SUCCESS != ret)
    {
        _tprintf(TEXT("failed to delete the directory. [ret = %d]\n"), ret);
    }
    return 0;
}