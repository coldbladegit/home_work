#include <stdio.h>

#include "file_util.h"
#include "err_no.h"

int main(int argc, char **argv)
{
    int ret = 0;
    if (argc > 1)
    {
        ret = PrintFile(argv[1]);
        if (ERR_SUCCESS != ret)
        {
            printf("failed to parse file [errNo = %d]\n", ret);
        }
        else
        {
            printf("successfull to parse file!!!\n");
        }
    }
    else
    {
        printf("please input file path!!!");
    }
    return ret;
}