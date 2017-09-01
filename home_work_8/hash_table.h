#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

int CreateHashTable(void **ppHashTb, int capacity);

void DestroyHashTable(void *pHashTb);

int InsertToHashTable(void *pHashTb, void *p, char *pFile, int line);

bool IsInHashTable(void *pHashTb, void *p);

int DeleteFromHashTable(void *pHashTb, void *p);

#endif//_HASH_TABLE_H