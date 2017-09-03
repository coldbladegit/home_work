#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

void* CreateHashTable(int capacity);

void DestroyHashTable(void *pHashTb);

int InsertToHashTable(void *pHashTb, void *p, char *pFile, int line);

bool IsInHashTable(void *pHashTb, void *p);

bool IsHashTableEmpty(void *pHashTb);

int	PrintElems(void *pHashTb);

int DeleteFromHashTable(void *pHashTb, void *p);

#endif//_HASH_TABLE_H