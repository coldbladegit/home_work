#include <string.h>
#include <stdio.h>
#include "swap_str.h"

int Swap(char **ppSrc, char **ppDest)
{
    char *pTmp = NULL;
    int ret = 0, i = 0;

    do 
    {
        ret = *ppSrc[i] - *ppDest[i];
        if (0 != ret || '\0' == *ppSrc[i])
        {
            break;
        }
        ++i;
    } while (1);

    pTmp = *ppSrc;
    *ppSrc = *ppDest;
    *ppDest = pTmp;

    return ret;
}