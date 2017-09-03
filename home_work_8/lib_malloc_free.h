#ifndef _LIB_MALLOC_FREE_H
#define _LIB_MALLOC_FREE_H

int lib_init(int capacity);

void lib_destroy();

int lib_malloc(void **ppMem, size_t size, char *pFile, int line);

int lib_free(void *p);

int lib_print_memory();

#endif//_LIB_MALLOC_FREE_H
