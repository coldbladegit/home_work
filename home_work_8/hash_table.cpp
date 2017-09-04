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
    return addr / (capacity - 1) % (capacity);
}

void* CreateHashTable(int capacity)
{
    HASH_TABLE *pHashTb = (HASH_TABLE *)malloc(sizeof(HASH_TABLE));

    if (NULL == pHashTb)
    {
        return NULL;
    }
	pHashTb->pElems = (BLUCKET_ELEM *)malloc(capacity * sizeof(BLUCKET_ELEM));
	if (NULL == pHashTb->pElems)
	{
		free(pHashTb);
		return NULL;
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
    return pHashTb;
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
    {
        pElem = (BLUCKET_ELEM *)malloc(sizeof(BLUCKET_ELEM));
        if (NULL == pElem)
        {
            return ERR_MALLOC_MEM_FAILD;
        }
        pElem->p = p;
        pElem->pFile = pFile;
        pElem->line = line;
		//发生碰撞时,直接插入到该元素所对应的链表的头部
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
	do 
	{
		if (pElem->p == p)
		{
			return true;
		}
		pElem = pElem->pNext;
	} while (NULL != pElem);

    return false;
}

bool IsHashTableEmpty(void *pHashTb)
{
	HASH_TABLE *pTb = NULL;

	if (NULL == pHashTb)
	{
		return false;
	}
	pTb = (HASH_TABLE *)pHashTb;
	return pTb->used == 0;
}

static void PrintElem(BLUCKET_ELEM *pElem)
{
	if (NULL == pElem->p)
	{
		return;
	}
	do 
	{
		printf("mem_addr:%x, file:%s, line:%d\n", pElem->p, pElem->pFile, pElem->line);
		pElem = pElem->pNext;
	} while (NULL != pElem);
}

int	PrintElems(void *pHashTb)
{
	HASH_TABLE *pTb = NULL;
	BLUCKET_ELEM **ppNext = NULL;

	if (NULL == pHashTb)
	{
		return ERR_INVALID_PARAM;
	}
	pTb = (HASH_TABLE *)pHashTb;

	for (int index = 0; index < pTb->capacity; ++index)
	{
		PrintElem(&pTb->pElems[index]);
	}

	return ERR_SUCCESS;
}

int DeleteFromHashTable(void *pHashTb, void *p)
{
    HASH_TABLE *pTb = NULL;
    BLUCKET_ELEM *pDel = NULL;
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
    {
        if (NULL == *ppNext)
        {//数组元素对应的链表为NULL
            pTb->pElems[index].pFile = NULL;
            pTb->pElems[index].line = 0;
            pTb->used--;
            free(pTb->pElems[index].p);
            pTb->pElems[index].p = NULL;
        }
        else
        {//将数组元素对应链表首个节点的值copy到数组元素,并删除该节点
            free(pTb->pElems[index].p);
            pDel = *ppNext;
            pTb->pElems[index].p = pDel->p;
            pTb->pElems[index].pFile = pDel->pFile;
            pTb->pElems[index].line = pDel->line;
            pTb->pElems[index].pNext = pDel->pNext;
            free(pDel);
        }
        return ERR_SUCCESS;
    }

	while(NULL != *ppNext && (*ppNext)->p != p)
	{
		ppNext = &((*ppNext)->pNext);
	}

	if (NULL == *ppNext)
	{
		return ERR_NOT_FIND;
	}
	else
	{
		pDel = *ppNext;
		*ppNext = pDel->pNext;
		free(pDel->p);
		free(pDel);
		return ERR_SUCCESS;
	}
}

int GetUsed(void *pHashTb, int *used)
{
    if (NULL == pHashTb)
    {
        return ERR_INVALID_PARAM;
    }

    *used = ((HASH_TABLE *)pHashTb)->used;
    return ERR_SUCCESS;
}