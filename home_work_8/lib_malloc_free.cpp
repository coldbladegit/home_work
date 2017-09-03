#include <malloc.h>
#include <stdio.h>
#include "hash_table.h"
#include "err_no.h"

static struct HASH_TABLE* g_hashTb = NULL;

int lib_init(int capacity)
{
	g_hashTb = (HASH_TABLE *)CreateHashTable(capacity);
	return NULL == g_hashTb ? ERR_MALLOC_MEM_FAILD : ERR_SUCCESS;
}

int lib_malloc(void **ppMem, size_t size, char *pFile, int line)
{
	void *pNewMem = NULL;

	if (NULL == g_hashTb)
	{
		return ERR_NOT_INITIALIZED;
	}
	
	pNewMem = malloc(size);
	if (NULL == pNewMem)
	{
		return ERR_MALLOC_MEM_FAILD;
	}
	*ppMem = pNewMem;
	InsertToHashTable(g_hashTb, pNewMem, pFile, line);
	return ERR_SUCCESS;
}

int lib_free(void *p)
{
	if (NULL == g_hashTb)
	{
		return ERR_NOT_INITIALIZED;
	}
	return DeleteFromHashTable(g_hashTb, p);
}

int lib_print_memory()
{
	if (NULL == g_hashTb)
	{
		return ERR_NOT_INITIALIZED;
	}
	if (IsHashTableEmpty(g_hashTb))
	{
		return ERR_NOT_FIND;
	}

	return PrintElems(g_hashTb);
}

void lib_destroy()
{
	DestroyHashTable(g_hashTb);
}