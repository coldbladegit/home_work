#include <stdio.h>

#include "file_util.h"
#include "zeller.h"
#include "err_no.h"

void PrintWeek();

int main(int argc, char **argv)
{
    PrintWeek();
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

void PrintWeek()
{
    int year = 2010;
    int month = 2;
    int day = 28;
    int week = 0;
    char *pWeekDay = NULL;
    
    ZellerCalculate(year, month, day, &week);
    switch (week)
    {
    case 1:
        pWeekDay = "Monday";
    	break;
    case 2:
        pWeekDay = "Tuesday";
        break;
    case 3:
        pWeekDay = "Wednesday";
        break;
    case 4:
        pWeekDay = "Thursday";
        break;
    case 5:
        pWeekDay = "Friday";
        break;
    case 6:
        pWeekDay = "Saturday";
        break;
    default:
        pWeekDay = "Sunday";
        break;
    }

    printf("%d/%d/%d is %s\n", year, month, day, pWeekDay);
}