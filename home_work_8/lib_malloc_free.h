#ifndef _LIB_MALLOC_FREE_H
#define _LIB_MALLOC_FREE_H

#define lib_malloc(ppMem, size)  lib_inner_malloc(ppMem, size, __FILE__, __LINE__)

int lib_init(int capacity);

void lib_destroy();

int lib_inner_malloc(void **ppMem, size_t size, char *pFile, int line);

int lib_free(void *p);

int lib_print_memory();

int lib_get_used(int *);

#endif//_LIB_MALLOC_FREE_H
