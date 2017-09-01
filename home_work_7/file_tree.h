#ifndef _FILE_TREE_H
#define _FILE_TREE_H

int ListDirectoryFiles(char *dirPath, void **ppFileTree);

int SortByModifyTime(void *pFileTree);

int PrintFileTree(void *pFileTree);

int FreeFileTree(void *pFileTree);

#endif//_FILE_TREE_H