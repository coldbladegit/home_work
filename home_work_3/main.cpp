#include <stdio.h>
#include <string.h>
#include "err_no.h"
#include "calculate.h"
#include "repstr.h"
#include "memory_manager.h"

typedef struct _STRUCT_TEST {
    int a;
    char str[500];
}STRUCT_TEST;

void TestFunc();

void TestMallocMemory();

int main(int argc, char **argv)
{
    int ret = ERR_SUCCESS;

    TestFunc();
    if (argc > 4)
    {
        REP_WORD_PARAM param;
        param.pSrcFile = argv[1];
        param.pSrcWord = argv[2];
        param.pDestWord = argv[3];
        param.pDestFile = argv[4];
       
        ret = ReplaceWord(&param);
        if (ERR_OPEN_FILE_FAILD == ret)
        {
            printf("failed to open file %s, %s\n", argv[1], argv[4]);
        }
        else if (ERR_MALLOC_MEM_FAILD == ret)
        {
            printf("out of memory!!!\n");
        }
        else if (ERR_WORD_TOO_LONG == ret)
        {
            printf("word in %s is too long ", argv[1]);
        }
        else
        {
            printf("successfull to replace the %s with %s\n", argv[2], argv[3]);
        }
    }
    else
    {
        printf("please input <原文件>  <原串> <目标串> <目标文件>!!!\n");
    }
    TestMallocMemory();
    return 0;
}

void PrintfExp(int a, int b, char op)
{
    int result;

    if (ERR_INVALID_OP == Calculate(a, b, &result, op))
    {
        printf("invalid operational character: %c\n", op);
    }
    else
    {
        printf("a %c b = %d\n", op, result);
    }
}

void TestFunc()
{
    int a = 10, b = 2;  
    PrintfExp(a, b, '+');
    PrintfExp(a, b, '-');
    PrintfExp(a, b, '*');
    PrintfExp(a, b, '/');
    PrintfExp(a, b, '^');
    PrintfExp(a, b, 'p');
}

void TestMallocMemory()
{
    if (ERR_SUCCESS != InitMemory())
    {
        printf("initialize memory manager failed!!!\n");
        return;
    }

    int *pInt = (int *)GetMemory(sizeof(int));
    *pInt = 5;

    double *pDouble = (double *)GetMemory(sizeof(double));
    *pDouble = 10000;

    char *pChar = (char *)GetMemory(sizeof(char));
    *pChar = 'S';

    char *pStr = (char *)GetMemory(20);
    strcpy(pStr,"hello world");

    char *pLongStr = (char *)GetMemory(800);
    strcpy(pLongStr,"aaaaaaaaaaaaaa");

    STRUCT_TEST *pStruct = (STRUCT_TEST *)GetMemory(sizeof(STRUCT_TEST));
    pStruct->a =8;
    strcpy(pStruct->str,"hello c");

    printf(" int = |%d|,double = |%d| \n char = |%d| \n Str = |%d| \n Struct_a = |%d|,Struct_str = |%d|\n",
        pInt, pDouble, pChar, pStr, &pStruct->a, pStruct->str);
    printf(" longStr = |%d|\n",pLongStr);

    DestroyMemory();
}