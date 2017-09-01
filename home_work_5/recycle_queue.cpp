#include <stdio.h>
#include <malloc.h>
#include "recycle_queue.h"
#include "err_no.h"

static int Add(RECYCLE_QUEUE *pRecyQue, char *str)
{
    if (pRecyQue->size == pRecyQue->capacity)
    {
        return ERR_FAILED;
    }
    pRecyQue->strArry[pRecyQue->addIndex] = str;
    pRecyQue->addIndex = (pRecyQue->addIndex + 1) % pRecyQue->capacity;
    pRecyQue->size++;
    return ERR_SUCCESS;
}

static char *Remove(RECYCLE_QUEUE *pRecyQue)
{
    if (0 == pRecyQue->size)
    {
        return NULL;
    }
    char *str = pRecyQue->strArry[pRecyQue->rmIndex];
    pRecyQue->strArry[pRecyQue->rmIndex] = NULL;
    pRecyQue->rmIndex = (pRecyQue->rmIndex + 1) % pRecyQue->capacity;
    pRecyQue->size--;
    return str;
}

static bool IsEmpty(RECYCLE_QUEUE *pRecyQue)
{
    return 0 == pRecyQue->size;
}

static bool IsFull(RECYCLE_QUEUE *pRecyQue)
{
    return pRecyQue->capacity == pRecyQue->size;
}

int CreateRecyleQueue(RECYCLE_QUEUE **ppRecyQue, int capacity)
{
    RECYCLE_QUEUE *pRecyQue = (RECYCLE_QUEUE *)malloc(sizeof(RECYCLE_QUEUE));
    if (NULL == pRecyQue)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pRecyQue->strArry = (char **)malloc(sizeof(char *) * capacity);
    if (NULL == pRecyQue->strArry)
    {
        free(pRecyQue);
        return ERR_MALLOC_MEM_FAILD;
    }
    pRecyQue->addIndex = 0;
    pRecyQue->rmIndex = 0;
    pRecyQue->capacity = capacity;
    pRecyQue->size = 0;
    pRecyQue->Add = Add;
    pRecyQue->Remove = Remove;
    pRecyQue->IsEmpty = IsEmpty;
    pRecyQue->IsFull = IsFull;
    *ppRecyQue = pRecyQue;

    return ERR_SUCCESS;
}

void DestroyRecyleQueue(RECYCLE_QUEUE *pRecyQue)
{
    if (NULL != pRecyQue)
    {
        free(pRecyQue->strArry);
        free(pRecyQue);
    }
}