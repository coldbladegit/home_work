#include <stdio.h>
#include "zeller.h"
#include "err_no.h"

static bool IsLeapYear(int year)
{
    if (year % 100 == 0)
    {
        return year % 400 == 0;
    }
    return year % 4 == 0;
}

static int GetDayMaxValue(int year, int month)
{
    switch(month)
    {
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return IsLeapYear(year) ? 29 : 28;
        default:
            return 31;
    }
}

int ZellerCalculate(int year, int month, int day, int *week)
{
    int c, y;

    if (year <= 0 || month <= 0 || month > 12)
    {
        return ERR_INVALID_PARAM;
    }
    if (day <= 0 || day > GetDayMaxValue(year, month))
    {
        return ERR_INVALID_PARAM;
    }
    
    if (month <= 2)
    {
        c = (year - 1) / 100;
        y = (year - 1) % 100;
        month += 12;
    }
    else
    {
        c = year / 100;
        y = year % 100;
    }

    *week = (c / 4 - 2 * c + y + y / 4 + 13 * (month + 1) / 5 + day - 1) % 7;
    if (*week <= 0)
    {
        *week += 7;
    }

    return ERR_SUCCESS;
}