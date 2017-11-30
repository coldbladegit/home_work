#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "err_no.h"
#include "cb_vector.h"

#define VEC_INCREMENT_CNT		16
#define DEFAULT_CAPACITY        8

static int add_element_to(CB_VECTOR *pVec, void *pElem, int index)
{
    void **ppDatas = NULL;

    G_ASSERT(NULL != pVec);

    if (index < 0 || index > pVec->size)
    {
        return ERR_INVALID_INDEX;
    }

    if (pVec->size >= pVec->capacity)
    {
        int newCapacity = pVec->capacity + VEC_INCREMENT_CNT;
        ppDatas = (void **) malloc(sizeof(void *) * newCapacity);
        if (NULL == ppDatas)
        {
            return ERR_MALLOC_MEM_FAILD;
        }
        for (int index = 0; index < pVec->capacity; ++index)
        {
            ppDatas[index] = pVec->ppDatas[index];
        }
        free(pVec->ppDatas);
        pVec->ppDatas = ppDatas;
        pVec->capacity = newCapacity;
    }
    if (index != pVec->size)
    {
        for (int i = pVec->size - 1; i >= index; --i)
        {
            pVec->ppDatas[i + 1] = pVec->ppDatas[i];
        }
    }
    pVec->ppDatas[index] = pElem;
    pVec->size++;

    return ERR_SUCCESS;
}

static int add_element (CB_VECTOR *pVec, void *pElem)
{
    G_ASSERT(NULL != pVec);

    return add_element_to(pVec, pElem, pVec->size);
}

static int remove_element_from(CB_VECTOR *pVec, int index, void **ppElem)
{
    G_ASSERT(NULL != pVec);

    if (index < 0 || index > pVec->size)
    {
        return ERR_INVALID_INDEX;
    }
    else if (pVec->size == 0)
    {
        return ERR_NOT_FIND;
    }
    *ppElem = pVec->ppDatas[index];
    for (int i = index; index < pVec->size - 1; ++index)
    {
        pVec[i] = pVec[i + 1];
    }
    pVec->size--;

    return ERR_SUCCESS;
}

static int remove_element(CB_VECTOR *pVec, void **ppElem)
{
    return remove_element_from(pVec, 0, ppElem);
}

static void remove_all(CB_VECTOR *pVec, FREE_ELEM_FUNC func)
{
    G_ASSERT(NULL != pVec);

    for (int index = 0; index < pVec->size; ++index)
    {
        func(pVec->ppDatas[index]);
    }
}

static void *search_element(CB_VECTOR *pVec, int stuNo, IS_EQUAL func)
{
    G_ASSERT(NULL != pVec);

    for (int index = 0; index < pVec->size; ++index)
    {
        if (func(pVec->ppDatas[index], stuNo))
        {
            return pVec->ppDatas[index];
        }
    }

    return NULL;
}

int new_cb_vector(CB_VECTOR **pp)
{
    CB_VECTOR *pVec = NULL;

    pVec = (CB_VECTOR *) malloc(sizeof(CB_VECTOR));
    if (NULL == pVec)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pVec->ppDatas = (void **) malloc(sizeof(void *) * DEFAULT_CAPACITY);
    if (NULL == pVec->ppDatas)
    {
        free(pVec);
        return ERR_MALLOC_MEM_FAILD;
    }
    pVec->size = 0;
    pVec->capacity = DEFAULT_CAPACITY;
    pVec->add_element = add_element;
    pVec->add_element_to = add_element_to;
    pVec->remove_element = remove_element;
    pVec->remove_element_from = remove_element_from;
    pVec->remove_all = remove_all;
    pVec->search_element = search_element;

    *pp = pVec;

    return ERR_SUCCESS;
}

void delete_cb_vector(CB_VECTOR *p)
{
    G_ASSERT(NULL != p);
    free(p);
}