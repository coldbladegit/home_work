#include <stdio.h>
#include "err_no.h"
#include "vir_stack.h"
#include "sort_search.h"

#define STACK_MAX_SIZE  100

void TestStack();
void TestSortAndSearch();

int main(int argc, char **argv)
{
    TestStack();
    TestSortAndSearch();
    return 0;
}

void TestStack()
{
    int ret;
    char *pElem;

    ret = Initialize(STACK_MAX_SIZE);
    if (ERR_SUCCESS != ret)
    {
        printf("failed to create stack!\n");
        return;
    }
    Push("func");
    Push("bb");
    Push("aa");
    
    while (NULL != (pElem = (char *)Pop()))
    {
        printf("%s ", pElem);
    }
    printf("\n");
}

void TestSortAndSearch()
{
    int index, len = 11;
    char *destStr = "wz678f";
    char *strArray[11];//¿ÉÐÞ¸Ä11Îª12²âÊÔ

    strArray[0] = "abcty";
    strArray[1] = "z52xf";
    strArray[2] = "d5bc";
    strArray[3] = "cxa3z";
    strArray[4] = "wz678f";
    strArray[5] = "aaa98";
    strArray[6] = "cdeuih";
    strArray[7] = "oiwuer";
    strArray[8] = "98a78";
    strArray[9] = "nn89n";
    strArray[10] = "mam";
    printf("******before sort******\n");
    for (int i = 0; i < len; ++i)
    {
        printf("%s ", strArray[i]);
    }
    QuickSort(strArray, len);
    printf("******after sort******\n");
    for (int i = 0; i < len; ++i)
    {
        printf("%s ", strArray[i]);
    }
    printf("\n");
    index = BinarySearch(strArray, len, destStr);
    printf("%s: index = %d\n", destStr, index);
}