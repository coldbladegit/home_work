#ifndef _LINK_LIST_H
#define _LINK_LIST_H
#include "struct.h"

int Insert(int id, const char *sName, int grade);

STUDENT *Remove(int id, STUDENT *pStu);

STUDENT *Search(int id);

void SetGrade(int id, int grade);

void Sort();

void PrintIn();
#endif//_LINK_LIST_H