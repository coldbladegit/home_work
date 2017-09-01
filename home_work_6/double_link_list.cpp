#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "double_link_list.h"
#include "err_no.h"

typedef struct _DOUBLE_LINK_NODE {
    STUDENT     *pStu;
    _DOUBLE_LINK_NODE  *pPrev;
    _DOUBLE_LINK_NODE  *pNext;
}DOUBLE_LINK_NODE;

static DOUBLE_LINK_NODE g_header = {NULL, NULL, NULL};

int Insert_s(int id, const char *sName, int grade)
{
    DOUBLE_LINK_NODE *pNode = NULL;
    STUDENT *pStu = NULL;
    DOUBLE_LINK_NODE *pNewNode = NULL;

    if (strlen(sName) > MAX_NAME_LEN)
    {
        return ERR_STR_TOO_LONG;
    }

    pStu = (STUDENT *)malloc(sizeof(STUDENT));
    if (NULL == pStu)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pStu->nId = id;
    pStu->nGrade = grade;
    strcpy(pStu->sName, sName);

    pNewNode = (DOUBLE_LINK_NODE *)malloc(sizeof(DOUBLE_LINK_NODE));
    if (NULL == pNewNode)
    {
        free(pStu);
        return ERR_MALLOC_MEM_FAILD;
    }
    pNewNode->pStu = pStu;
    pNewNode->pNext = NULL;

    pNode = &g_header;
    while (NULL != pNode->pNext)
    {//遍历到链表末尾
        pNode = pNode->pNext;
    }
    pNode->pNext = pNewNode;
    pNewNode->pPrev = pNode;

    return ERR_SUCCESS;
}

STUDENT *Remove_s(int id, STUDENT *pStu)
{
    DOUBLE_LINK_NODE *pNode = NULL;

    pNode = &g_header;
    while(NULL != (pNode = pNode->pNext))
    {
        if (pNode->pStu->nId == id)
        {
            pStu->nId = pNode->pStu->nId;
            pStu->nGrade = pNode->pStu->nGrade;
            strcpy(pStu->sName, pNode->pStu->sName);

            pNode->pPrev->pNext = pNode->pNext;
            free(pNode->pStu);
            free(pNode);
            return pStu;
        }
    }
    return NULL;
}

STUDENT *Search_s(int id)
{
    DOUBLE_LINK_NODE *pNode = NULL;

    pNode = &g_header;
    while(NULL != (pNode = pNode->pNext))
    {
        if (pNode->pStu->nId == id)
        {
            return pNode->pStu;
        }
    }
    return NULL;
}

void Sort_s()
{
    DOUBLE_LINK_NODE *p = NULL;
    DOUBLE_LINK_NODE *q = NULL;
    STUDENT *pStu;
    
    p = g_header.pNext;
    if (NULL == p)
    {
        return;
    }
    for (p = p->pNext; NULL != p; p = p->pNext)
    {
        pStu = p->pStu;
        for (q = p->pPrev; NULL != q->pPrev; q = q->pPrev)
        {//插入排序
            if (pStu->nGrade < q->pStu->nGrade)
            {//边比较，边移动
                q->pNext->pStu = q->pStu;
            }
        }
        if (q->pNext != p)
        {
            q->pNext->pStu = pStu;
        }
    }
}

void SetGrade_s(int id, int grade)
{
    STUDENT *pStu = NULL;

    pStu = Search_s(id);
    if (NULL != pStu)
    {
        pStu->nGrade = grade;
    }
}

void PrintIn_s()
{
    DOUBLE_LINK_NODE *pNode = NULL;

    printf("学号\t\t姓名\t成绩\t\n");
    pNode = &g_header;
    while(NULL != (pNode = pNode->pNext))
    {
        printf("%d\t%s\t%d\n", pNode->pStu->nId, pNode->pStu->sName, pNode->pStu->nGrade);
    }
}