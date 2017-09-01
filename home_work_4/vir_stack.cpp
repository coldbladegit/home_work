#include <malloc.h>
#include <stdio.h>
#include "vir_stack.h"
#include "err_no.h"

typedef struct _VIR_STACK {
    int     size;
    int     capacity;
    void    **elems;
}VIR_STACK;

static VIR_STACK g_stack;

int Initialize(int capacity)
{
    if (capacity <= 0)
    {
        return ERR_INVALID_PARAM;
    }
    g_stack.elems = (void **)malloc(sizeof(void *) * capacity);
    if (NULL == g_stack.elems)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    g_stack.size = 0;
    g_stack.capacity = capacity;

    return ERR_SUCCESS;
}

int Push(void *elem)
{
    if (g_stack.size >= g_stack.capacity)
    {
        return ERR_STACK_FULL;
    }
    g_stack.elems[g_stack.size] = elem;
    g_stack.size++;

    return ERR_SUCCESS;
}

void *Pop()
{
    if (g_stack.size <= 0)
    {
        return NULL;
    }
    g_stack.size--;
    return g_stack.elems[g_stack.size];
}