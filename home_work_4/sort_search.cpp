#include <string.h>
#include "sort_search.h"

static inline void SwapStr(char *strArry[], int srcIndex, int destIndex)
{
    char *str = strArry[srcIndex];
    strArry[srcIndex] = strArry[destIndex];
    strArry[destIndex] = str;
}

void BubbleSort(char *strArr[], int len)
{
    for (int i = 1; i < len; ++i)
    {
        for (int j = 0; j < len - i; ++j)
        {
            if (strcmp(strArr[j], strArr[j + 1]) > 0)
            {
                SwapStr(strArr, j, j + 1);
            }
        }
    }
}

void InsertSort(char *strArr[], int len)
{
    for (int i = 0; i < len; ++i)
    {
        for (int j = i + 1; j > 0; --j)
        {
            if (strcmp(strArr[j], strArr[j - 1]) < 0)
            {
                SwapStr(strArr, j, j - 1);
            }
            else
            {
                break;
            }
        }
    }
}

static int partition(char *strArr[], int start, int end)
{
    char *pivot = strArr[end];
    int left = start, right = end - 1;
    while (left < right)
    {
        while (left < right && strcmp(strArr[left], pivot) < 0)
        {
            left++;
        }
        while (left < right && strcmp(strArr[right], pivot) > 0)
        {
            right--;
        }
        if (left < right)
        {
            SwapStr(strArr, left, right);
        }
    }
    if (strcmp(strArr[left], pivot) >= 0) 
    {
        SwapStr(strArr, left, end);
    }
    else
    {
        left++;
    }
    return left;
}

static void QuickSortRecusive(char *strArr[], int start, int end)
{
    int key = partition(strArr, start, end);
    if (key > start + 1)
    {
        QuickSortRecusive(strArr, start, key - 1);
    }
    if (key < end - 1)
    {
        QuickSortRecusive(strArr, key + 1, end);
    }
}

void QuickSort(char *strArr[], int len)
{
    QuickSortRecusive(strArr, 0, len - 1);
}

int Search(char *strArr[], int len, char *str)
{
    int index = -1;
    for (int i = 0; i < len; ++i)
    {
        if (strcmp(strArr[i], str) == 0)
        {
            index = i;
            break;
        }
    }
    return index;
}

int BinarySearch(char *strArr[], int len, char *str)
{
    int mid;
    int start = 0, end = len - 1;
    mid = (start + end) >> 1;
    do 
    {
        if (strcmp(strArr[mid], str) > 0)
        {
            end = mid - 1;
        }
        else if (strcmp(strArr[mid], str) < 0)
        {
            start = mid + 1;
        }
        else
        {
            return mid;
        }
        mid = (start + end) >> 1;
    } while (mid >= start && mid <= end);

    return -1;
}