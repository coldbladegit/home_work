#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>
#include "err_no.h"
#include "stu_mgr_service.h"
#include "stu_mgr_client.h"

static int g_flag = 0;

void do_stop(int sig);

int main(int argc, char **argv)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    printf("cpu num: %d\n", si.dwNumberOfProcessors);
    return 0;

    int ret = ERR_SUCCESS;

    setlocale(LC_ALL, "");

    if (argc < 2)
    {
        printf("please input flag<1 or 2>, 1 is for service\n");
    }
    else
    {
        g_flag = atoi(argv[1]);
        signal(SIGINT, do_stop);
        if (1 == g_flag)
        {
            ret = init_stu_service();
            if (ERR_SUCCESS != ret)
            {
                printf("init service failed, [errNo = %d]\n", ret);
            }
            else
            {
                ret = start_stu_service();
                if (ERR_SUCCESS != ret)
                {
                    printf("start service failed, [errNo = %d]\n", ret);
                }
            }
        }
        else
        {
            ret = init_stu_client();
            if (ERR_SUCCESS != ret)
            {
                printf("init client failed, [errNo = %d]\n", ret);
            }
        }
    }
    return 0;
}

void do_stop(int sig)
{
    if (SIGINT == sig)
    {
        if (1 == g_flag)
        {
            stop_stu_service();
            release_stu_service();
        }
        else
        {
            release_stu_client();
        }
    }
}