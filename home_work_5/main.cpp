#include <stdio.h>
#include "err_no.h"
#include "recycle_queue.h"

#define STACK_MAX_SIZE  100

void TestRecycleQueue();

int main(int argc, char **argv)
{
    TestRecycleQueue();
    return 0;
}

void TestRecycleQueue()
{
    RECYCLE_QUEUE *pRecyQue;
    int ret;
    
    ret = CreateRecyleQueue(&pRecyQue, 4);
    if (ERR_SUCCESS != ret)
    {
        printf("create recycle queue failed!\n");
        return;
    }
    char *strArray[11];

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

    for (int i = 0; i < 11; ++i)
    {
        ret = pRecyQue->Add(pRecyQue, strArray[i]);
        if (ERR_SUCCESS != ret)
        {
            printf("\nqueue is full: %s\n", pRecyQue->IsFull(pRecyQue) ? "TRUE" : "FALSE");
            break;
        }
        printf("add str =%s; ", strArray[i]);
    }

    do 
    {
        printf("remove str =%s; ", pRecyQue->Remove(pRecyQue));
    } while (!pRecyQue->IsEmpty(pRecyQue));
    printf("\nqueue is empty: %s\n", pRecyQue->IsEmpty(pRecyQue) ? "TRUE" : "FALSE");

    DestroyRecyleQueue(pRecyQue);
}