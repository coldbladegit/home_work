#include <stdio.h>
#include <string.h>
#include "err_no.h"
#include "vir_stack.h"
#include "sort_search.h"
#include "cb_stack.h"

#define STACK_MAX_SIZE  100

void TestStack();
void TestSortAndSearch();

int main(int argc, char **argv)
{
    TestStack();
    //TestSortAndSearch();
    return 0;
}

char *strtoparam(char *pSrc, const char *step)
{
    static char *pStr = NULL;
    static char *pSrcStr = NULL;
    const char *control = NULL;

    if (NULL != pSrc)
    {//�Ӻ���ǰ����
        pStr = pSrc + strlen(pSrc);
        pSrcStr = pSrc;
    }
    if (pStr == pSrcStr)
    {//�Ѿ������������˻��״δ���ľ��ǿմ�
        return NULL;
    }
    do 
    {
        control = step;
        do 
        {
            if (*pStr == *control)
            {
                *pStr = '\0';
                if ('\0' != *(pStr + 1))
                {
                    return pStr + 1;
                }
                break;
            }
            control++;
        } while ('\0' != *control);
        pStr--;
    } while(pStr != pSrcStr);
    
    return pStr;
}

void TestStack()
{
    int ret = ERR_SUCCESS;
    char *pElem = NULL;
    const char *step = ",)";
    char funcStr[] = "func(aa,bb)";
    char *pFunc = funcStr;

    ret = Initialize(STACK_MAX_SIZE);
    if (ERR_SUCCESS != ret)
    {
        printf("failed to create stack!\n");
        return;
    }

    pElem = strchr(pFunc, '(');//�Ƚ�����������
    if (NULL != pElem) 
    {
        *pElem = '\0';
        Push(pFunc);
        pFunc += strlen(pFunc) + 1;
        pElem = strtoparam(pFunc, step);
        while (NULL != pElem)
        {
            Push(pElem);
            pElem = strtoparam(NULL, step);
        }
    }
    
    while (NULL != (pElem = (char *)Pop()))
    {
        printf("%s ", pElem);
    }
    printf("\n");
}

void TestSortAndSearch()
{
    int index, len = 11;
    char *destStr = "wz678f";
    char *strArray[11];//���޸�11Ϊ12����

    strArray[0] = "abcty";
    strArray[1] = "z52xf";
    strArray[2] = "d5bc";
    strArray[3] = "cxa3z";
    strArray[4] = "wz678f";
    strArray[5] = "aaa98";
    strArray[6] = "cdeuih";
    strArray[7] = "oiwuer";
    strArray[8] = "98a78";
    strArray[9] = "nn89n";
    strArray[10] = "mam";
    printf("******before sort******\n");
    for (int i = 0; i < len; ++i)
    {
        printf("%s ", strArray[i]);
    }
    QuickSort(strArray, len);
    printf("******after sort******\n");
    for (int i = 0; i < len; ++i)
    {
        printf("%s ", strArray[i]);
    }
    printf("\n");
    index = BinarySearch(strArray, len, destStr);
    printf("%s: index = %d\n", destStr, index);
}