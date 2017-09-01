#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "link_list.h"
#include "err_no.h"

typedef struct _LINK_NODE {
    STUDENT     *pStu;
    _LINK_NODE  *pNext;
}LINK_NODE;

static LINK_NODE g_header = {NULL, NULL};

int Insert(int id, const char *sName, int grade)
{
    LINK_NODE *pNode = NULL;
    STUDENT *pStu = NULL;
    LINK_NODE *pNewNode = NULL;

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

    pNewNode = (LINK_NODE *)malloc(sizeof(LINK_NODE));
    if (NULL == pNewNode)
    {
        free(pStu);
        return ERR_MALLOC_MEM_FAILD;
    }
    pNewNode->pStu = pStu;
    pNewNode->pNext = NULL;

    pNode = &g_header;
    while (NULL != pNode->pNext)
    {//遍历到表尾
        pNode = pNode->pNext;
    }
    pNode->pNext = pNewNode;

    return ERR_SUCCESS;
}

STUDENT *Remove(int id, STUDENT *pStu)
{
    LINK_NODE **ppNode = &g_header.pNext;

    while(NULL != *ppNode)
    {
        if((*ppNode)->pStu->nId == id)
        {
            LINK_NODE *pNode = *ppNode;
            *ppNode = pNode->pNext;
            pStu->nId = pNode->pStu->nId;
            pStu->nGrade = pNode->pStu->nGrade;
            strcpy(pStu->sName, pNode->pStu->sName);

            ppNode = &pNode->pNext;

            free(pNode->pStu);
            free(pNode);
            return pStu;
        }
        ppNode = &(*ppNode)->pNext;
    }

}

STUDENT *Search(int id)
{
    LINK_NODE *pNode = NULL;

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

void Sort()
{
    LINK_NODE *p = NULL;
    LINK_NODE *q = NULL;
    STUDENT   *pStu = NULL;

    for (p = g_header.pNext; NULL != p; p = p->pNext)
    {//选择排序
        for (q = p->pNext; NULL != q; q = q->pNext)
        {
            if (p->pStu->nGrade < q->pStu->nGrade)
            {
                pStu = p->pStu;
                p->pStu = q->pStu;
                q->pStu = pStu;
            }
        }
    }
}

void SetGrade(int id, int grade)
{
    STUDENT *pStu = NULL;

    pStu = Search(id);
    if (NULL != pStu)
    {
        pStu->nGrade = grade;
    }
}

void PrintIn()
{
    LINK_NODE *pNode = NULL;

    printf("学号\t\t姓名\t成绩\t\n");
    pNode = &g_header;
    while(NULL != (pNode = pNode->pNext))
    {
        printf("%d\t%s\t%d\n", pNode->pStu->nId, pNode->pStu->sName, pNode->pStu->nGrade);
    }
}