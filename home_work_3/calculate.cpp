#include <stdio.h>

#include "calculate.h"
#include "err_no.h"

typedef int (*FUN_CALC)(int, int);

static int Add(int a, int b)
{
    return a + b;      
}

static int Sub(int a, int b)
{
    return a - b;
}

static int Mul(int a, int b)
{
    return a * b;
}

static int Div(int a, int b)
{
    return a / b;
}

static int Pow(int a, int b)
{
    int result = 1;
    for (int i = 0; i < b; ++i)
    {
        result = result * a;
    }
    return result;
}

static FUN_CALC GetCalculateOp(char op)
{
    switch(op)
    {
    case '+':
        return Add;
    case '-':
        return Sub;
    case '*':
        return Mul;
    case '/':
        return Div;
    case '^':
        return Pow;
    default:
        return NULL;
    }
}

int Calculate(int a, int b, int *result, char op)
{
    FUN_CALC pFunc = GetCalculateOp(op);
    if (NULL == pFunc)
    {
        return ERR_INVALID_OP;
    }
    *result = pFunc(a, b);
    return ERR_SUCCESS;
}