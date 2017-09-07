#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#include "sort.h"
#include "err_no.h"

#define  STATCK_BASE_SIZE  32

typedef struct _RANGE {
    int start;
    int end;
}RANGE;

typedef struct _RANGE_STACK {
    RANGE *pRanges;
    int size;
    int capacity;
}RANGE_STACK;

static RANGE_STACK* InitStack(RANGE_STACK *pRangeStack)
{
    int rangeSize = sizeof(RANGE) * STATCK_BASE_SIZE;

    pRangeStack->capacity = STATCK_BASE_SIZE;
    pRangeStack->size = 0;
    pRangeStack->pRanges = (RANGE *)malloc(rangeSize);
    if (NULL == pRangeStack->pRanges)
    {
        return NULL;
    }
    memset(pRangeStack->pRanges, 0, rangeSize);
    return pRangeStack;
}

static void DestroyStack(RANGE_STACK *pRangeStack) 
{
    if (NULL != pRangeStack->pRanges)
    {
        free(pRangeStack->pRanges);   
    }
}

static int IncreaseStack(RANGE_STACK *pRangeStack)
{
    int oldSize = pRangeStack->capacity * sizeof(RANGE);
    int incSize = STATCK_BASE_SIZE * sizeof(RANGE);

    RANGE *pNewArray = (RANGE *)malloc(oldSize + incSize);
    if (NULL == pNewArray)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    memcpy(pNewArray, pRangeStack->pRanges, oldSize);
    memset((char *)pNewArray + oldSize, 0, incSize);
    free(pRangeStack->pRanges);//指向新的缓冲之前,必须释放就的缓冲
    pRangeStack->capacity += STATCK_BASE_SIZE;
    pRangeStack->pRanges = pNewArray;
    return ERR_SUCCESS;
}

static int Push(RANGE_STACK *pRangeStack, int start, int end)
{
    int ret = ERR_SUCCESS;

    if (pRangeStack->capacity <= pRangeStack->size)
    {
        ret = IncreaseStack(pRangeStack);
        if (ERR_SUCCESS != ret)
        {
            return ret;
        }
    }
    pRangeStack->pRanges[pRangeStack->size].start = start;
    pRangeStack->pRanges[pRangeStack->size].end = end;
    pRangeStack->size++;
    return ret;
}

static RANGE* Pop(RANGE_STACK *pRangeStack, RANGE* pRANGE)
{
    if (pRangeStack->size <= 0)
    {
        return NULL;
    }
    pRangeStack->size--;
    pRANGE->start = pRangeStack->pRanges[pRangeStack->size].start;
    pRANGE->end = pRangeStack->pRanges[pRangeStack->size].end;
    return pRANGE;
}

static inline int Compare(int value, int nextValue, ORDER order)
{
    switch(order)
    {
    case ASCENDING:
        return value - nextValue;
    case DSCENDING:
        return nextValue - value;
    default:
        return 0;
    }
}

static inline void Swap2Value(int *value1, int *value2)
{
    int tmp = *value1;
    *value1 = *value2;
    *value2 = tmp;
}

void BubbleSort(int iArry[], int len, ORDER order)
{
    if (len <= 1)
    {
        return;
    }
    for (int i = 1; i < len; ++i)
    {
        for (int j = 0; j < len - i; ++j)
        {
            if (Compare(iArry[j], iArry[j + 1], order) > 0)
            {
                Swap2Value(&iArry[j], &iArry[j + 1]);
            }
        }
    }
}

void SelectSort(int iArry[], int len, ORDER order)
{
    if (len <= 1)
    {
        return;
    }
    for (int i = 0; i < len; ++i)
    {
        for (int j = i + 1; j < len; ++j)
        {
            if (Compare(iArry[i], iArry[j], order) > 0)
            {
                Swap2Value(&iArry[i], &iArry[j]);
            }
        }
    }
}

void InsertSort(int iArry[], int len, ORDER order)
{
    if (len <= 1)
    {
        return;
    }
    int j, key;
    for (int i = 1; i < len; ++i)
    {
        key = iArry[i];
        j = i - 1;
        while (j >= 0 && Compare(iArry[j], key, order) > 0)
        {//边比较，边移动
            iArry[j + 1] = iArry[j];
            j--;
        }
        if (j + 1 != i)
        {
            iArry[j + 1] = key;
        }
    }
}

static int partition(int iArry[], int start, int end, ORDER order)
{
    int pivot = iArry[end];
    int left = start, right = end - 1;
    while (left < right)
    {
        while (left < right && Compare(pivot, iArry[left], order) > 0)
        {
            left++;
        }
        while (left < right && Compare(pivot, iArry[right], order) <= 0)
        {
            right--;
        }
        if (left < right)
        {
            Swap2Value(&iArry[left], &iArry[right]);
        }
    }
    if (Compare(pivot, iArry[left], order) <= 0) 
    {
        Swap2Value(&iArry[left], &iArry[end]);
    }
    else
    {
        left++;
    }
    return left;
}

static void QuickSortRecusive(int iArry[], int start, int end, ORDER order)
{
    int key = partition(iArry, start, end, order);
    if (key > start + 1)
    {
        QuickSortRecusive(iArry, start, key - 1, order);
    }
    if (key < end - 1)
    {
        QuickSortRecusive(iArry, key + 1, end, order);
    }
}

static void QuickSortNonRecusive(int iArry[], int len, ORDER order)
{
    int ret = ERR_SUCCESS, key;
    RANGE_STACK stack;
    RANGE_STACK *pStack = NULL;
    RANGE range;
    RANGE *pRange = NULL;

    pStack = InitStack(&stack);
    if (NULL == pStack)
    {
        return;
    }
    do 
    {
        ret = Push(pStack, 0, len - 1);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        while(NULL != (pRange = Pop(pStack, &range)))
        {
            key = partition(iArry, pRange->start, pRange->end, order);
            if (key > pRange->start + 1)
            {
                ret = Push(pStack, pRange->start, key - 1);
            }
            if (ERR_SUCCESS == ret && key < pRange->end - 1)
            {
                ret = Push(pStack, key + 1, pRange->end);
            }
            if (ERR_SUCCESS != ret)
            {
                break;
            }
        }
    } while (0);
    DestroyStack(pStack);
}

void QuickSort(int iArry[], int len, ORDER order)
{
    //QuickSortRecusive(iArry, 0, len - 1, order);
    QuickSortNonRecusive(iArry, len, order);
}