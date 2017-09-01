#ifndef _SORT_H
#define _SORT_H

typedef enum ORDER
{
	ASCENDING, DSCENDING
};

typedef enum SORT_ARITHMETIC
{
	BUBBLE, SELECT, INSERT, QUICK
};

void BubbleSort(int iArry[], int len, ORDER order);

void SelectSort(int iArry[], int len, ORDER order);

void InsertSort(int iArry[], int len, ORDER order);

void QuickSort(int iArry[], int len, ORDER order);

#endif //_SORT_H