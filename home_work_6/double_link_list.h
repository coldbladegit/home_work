#ifndef _DOUBLE_LINK_LIST_H
#define _DOUBLE_LINK_LIST_H
#include "struct.h"

int Insert_s(int id, const char *sName, int grade);

STUDENT *Remove_s(int id, STUDENT *pStu);

STUDENT *Search_s(int id);

void SetGrade_s(int id, int grade);

void Sort_s();

void PrintIn_s();
#endif//_DOUBLE_LINK_LIST_H