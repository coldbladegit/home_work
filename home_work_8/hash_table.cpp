#include <malloc.h>
#include <stdio.h>
#include "hash_table.h"
#include "err_no.h"

typedef struct _BLUCKET_ELEM {
    void            *p;//key
    char            *pFile;//value
    int             line;//value
    _BLUCKET_ELEM   *pNext;//next elem
}BLUCKET_ELEM;

typedef struct _HASH_TABLE {
    int             used;
    int             capacity;
    BLUCKET_ELEM    *pElems;
}HASH_TABLE;

static int KeyToIndex(void *p, int capacity)
{
    size_t addr = (size_t)p;
    return addr % (capacity * capacity);
}

int CreateHashTable(void **ppHashTb, int capacity)
{
    HASH_TABLE *pHashTb = (HASH_TABLE *)malloc(sizeof(HASH_TABLE) + capacity * sizeof(BLUCKET_ELEM));

    if (NULL == pHashTb)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pHashTb->capacity = capacity;
    pHashTb->used = 0;
    for (int i = 0; i < capacity; ++i)
    {
        pHashTb->pElems[i].p = NULL;
        pHashTb->pElems[i].pFile = NULL;
        pHashTb->pElems[i].line = 0;
        pHashTb->pElems[i].pNext = NULL;
    }
    *ppHashTb = pHashTb;

    return ERR_SUCCESS;
}

void DestroyHashTable(void *pHashTb)
{
    HASH_TABLE *pTb = NULL;
    BLUCKET_ELEM *pElem = NULL;
    BLUCKET_ELEM *pNext = NULL;

    if (NULL == pHashTb)
    {
        return;
    }
    pTb = (HASH_TABLE *)pHashTb;
    for (int i = 0; i < pTb->capacity; ++i)
    {
        pElem = &pTb->pElems[i];
        if (NULL == pElem->p)
        {
            continue;
        }
        free(pElem->p);

        pNext = pElem->pNext;
        while(NULL != pNext)
        {
            pElem = pNext;
            pNext = pNext->pNext;
            free(pElem->p);
            free(pElem);
        }
    }
    free(pTb);
}

int InsertToHashTable(void *pHashTb, void *p, char *pFile, int line)
{
    HASH_TABLE *pTb = NULL;
    BLUCKET_ELEM *pElem = NULL;
    int index;

    if (NULL == pHashTb || NULL == p)
    {
        return ERR_INVALID_PARAM;
    }
    pTb = (HASH_TABLE *)pHashTb;

    index = KeyToIndex(p, pTb->capacity);
    /**碰撞检测**/
    if (pTb->pElems[index].p == NULL)
    {
        pTb->pElems[index].p = p;
        pTb->pElems[index].pFile = pFile;
        pTb->pElems[index].line = line;
        pTb->used++;
    }
    else
    {//发生碰撞时,直接插入到该元素所对应的链表的头部
        pElem = (BLUCKET_ELEM *)malloc(sizeof(BLUCKET_ELEM));
        if (NULL == pElem)
        {
            return ERR_MALLOC_MEM_FAILD;
        }
        pElem->p = p;
        pElem->pFile = pFile;
        pElem->line = line;

        pElem->pNext = pTb->pElems[index].pNext;
        pTb->pElems[index].pNext = pElem;
    }
    return ERR_SUCCESS;
}

bool IsInHashTable(void *pHashTb, void *p)
{
    HASH_TABLE *pTb = NULL;
    BLUCKET_ELEM *pElem = NULL;
    int index;

    if (NULL == pHashTb || NULL == p)
    {
        return false;
    }
    pTb = (HASH_TABLE *)pHashTb;

    index = KeyToIndex(p, pTb->capacity);
    pElem = &pTb->pElems[index];
    while (pElem->p != p)
    {
        pElem = pElem->pNext;
    }

    return NULL != pElem;
}

int DeleteFromHashTable(void *pHashTb, void *p)
{
    HASH_TABLE *pTb = NULL;
    BLUCKET_ELEM **ppNext = NULL;
    int index;

    if (NULL == pHashTb || NULL == p)
    {
        return ERR_INVALID_PARAM;
    }
    pTb = (HASH_TABLE *)pHashTb;

    index = KeyToIndex(p, pTb->capacity);
    ppNext = &pTb->pElems[index].pNext;
    if (pTb->pElems[index].p == p)
    {//特殊处理表头
        if (NULL == *ppNext)
        {
            pTb->pElems[index].pFile = NULL;
            pTb->pElems[index].line = 0;
            pTb->used--;
            free(pTb->pElems[index].p);
        }
        else
        {
            pTb->pElems[index].p = (*ppNext)->p;
            pTb->pElems[index].pFile = (*ppNext)->pFile;
            pTb->pElems[index].line = (*ppNext)->line;
            pTb->pElems[index].pNext = (*ppNext)->pNext;
            free((*ppNext)->p);
            free(*ppNext);
        }
        return ERR_SUCCESS;
    }

    while(NULL == *ppNext && (*ppNext)->p != p)
    {
        ppNext = &((*ppNext)->pNext);
    }
    if (NULL == *ppNext)
    {
        return ERR_NOT_FIND;
    }
    BLUCKET_ELEM *pDel = *ppNext;
    *ppNext = (*ppNext)->pNext;
    free(pDel->p);
    free(pDel);

    return ERR_SUCCESS;
}