#include <stdio.h>

#include "sort.h"

void printArray(int iArr[], int len);

void test_sort(SORT_ARITHMETIC sort);

int main(int argc, char **argv)
{
    printf("......blubble sort......\n");
    test_sort(BUBBLE);
    printf("\n......select sort......\n");
    test_sort(SELECT);
    printf("\n......insert sort......\n");
    test_sort(INSERT);
    printf("\n......quick sort......\n");
    test_sort(QUICK);
    return 0;
}

void test_sort(SORT_ARITHMETIC sort)
{
    int arrLen = 9;
    //int num[] = { 8, 3, 5, 2, 7 };
    int num[] = { 1, 12, 5, 26, 7, 14, 3, 7, 2 };
    printf("......num array before order......\n");
    printArray(num, arrLen);
    switch (sort)
    {
    case BUBBLE:
        BubbleSort(num, arrLen, ASCENDING);
        break;
    case SELECT:
        SelectSort(num, arrLen, ASCENDING);
        break;
    case INSERT:
        InsertSort(num, arrLen, ASCENDING);
        break;
    case QUICK:
        QuickSort(num, arrLen, ASCENDING);
        break;
    default:
        break;
    }
    printf("......num array after order......\n");
    printArray(num, arrLen);
}

void printArray(int iArr[], int len)
{
    if (len <= 0)
    {
        return;
    }
    printf("{");
    for (int i = 0; i < len - 1; ++i)
    {
        printf("%d, ", iArr[i]);
    }
    printf("%d} \n", iArr[len - 1]);
}