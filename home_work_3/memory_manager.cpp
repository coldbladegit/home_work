#include <malloc.h>
#include <stdio.h>
#include "memory_manager.h"
#include "err_no.h"

#define ADD_MEMORY_SIZE_PER         1024
#define MEMORY_BLOCK_MAX_COUNT      1000

typedef struct _MEM_BLOCK{
    int size;
    char *pStartAddr;
    int used;
}MEM_BLOCK;

static MEM_BLOCK g_memBlocks[MEMORY_BLOCK_MAX_COUNT];

static int MallocMemory(MEM_BLOCK *pMem, int size)
{
    size = ((size >> 10) + ((size & 0x3ff) == 0 ? 0 : 1)) << 10;
    pMem->pStartAddr = (char *)malloc(size);
    if (NULL == pMem->pStartAddr)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pMem->size = size;
    pMem->used = 0;

    return ERR_SUCCESS;
}

int InitMemory()
{
    int ret = ERR_SUCCESS;

    ret = MallocMemory(&g_memBlocks[0], ADD_MEMORY_SIZE_PER);
    if (ERR_SUCCESS != ret)
    {
        ret = ret;
    }
    for (int i = 1; i < MEMORY_BLOCK_MAX_COUNT; ++i)
    {
        g_memBlocks[i].size = 0;
        g_memBlocks[i].used = 0;
        g_memBlocks[i].pStartAddr = NULL;
    }

    return ret;
}

void DestroyMemory()
{
    for (int i = 0; i < MEMORY_BLOCK_MAX_COUNT; ++i)
    {
        g_memBlocks[i].size = 0;
        g_memBlocks[i].used = 0;
        if (NULL != g_memBlocks[i].pStartAddr)
        {
            free(g_memBlocks[i].pStartAddr);
        }
    }
}

void *GetMemory(int size)
{
    void *pMem = NULL;

    for (int i = 0; i < MEMORY_BLOCK_MAX_COUNT; ++i)
    {
        if (g_memBlocks[i].size - g_memBlocks[i].used >= size)
        {
            pMem = g_memBlocks[i].pStartAddr + g_memBlocks[i].used;
            g_memBlocks[i].used += size;
            break;
        }
        else if (0 == g_memBlocks[i].size)
        {
            if (ERR_SUCCESS == MallocMemory(&g_memBlocks[i], size))
            {
                pMem = g_memBlocks[i].pStartAddr;
                g_memBlocks[i].used = size;
            }
            break;
        }
    }
    return pMem;
}