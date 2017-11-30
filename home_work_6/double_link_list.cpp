#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "double_link_list.h"
#include "err_no.h"
#include "struct.h"

typedef struct _DOUBLE_LINK_NODE {
    STUDENT     *pStu;
    _DOUBLE_LINK_NODE  *pPrev;
    _DOUBLE_LINK_NODE  *pNext;
}DOUBLE_LINK_NODE;

static DOUBLE_LINK_NODE *g_pHeader = NULL;
static DOUBLE_LINK_NODE *g_pTail = NULL;

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

    if (NULL == g_pHeader)
    {
        g_pHeader = pNewNode;
        g_pTail = pNewNode;
        pNewNode->pNext = NULL;
        pNewNode->pPrev = NULL;

    }
    else
    {
        pNode = g_pTail;
        while (NULL != pNode->pPrev && pNode->pStu->nId >= pNewNode->pStu->nId)
        {
            pNode = pNode->pPrev;
        }
        pNewNode->pNext = pNode->pNext;
        pNode->pNext = pNewNode;
        pNewNode->pPrev = pNode;

        if (g_pTail == pNode)
        {
            g_pTail = pNewNode;
        }
    }    

    return ERR_SUCCESS;
}

STUDENT *Remove_s(int id, STUDENT *pStu)
{
    DOUBLE_LINK_NODE *pNode = NULL;

    pNode = g_pHeader;
    while(NULL != pNode)
    {
        if (pNode->pStu->nId == id)
        {
            pStu->nId = pNode->pStu->nId;
            pStu->nGrade = pNode->pStu->nGrade;
            strcpy(pStu->sName, pNode->pStu->sName);

            if (g_pHeader == pNode || g_pTail == pNode)
            {
                if (g_pHeader == pNode)
                {
                    g_pHeader = pNode->pNext;
                    if (NULL != g_pHeader)
                    {
                        g_pHeader->pPrev = NULL;
                    }
                }
                if (g_pTail == pNode)
                {
                    g_pTail = pNode->pPrev;
                    if (NULL != g_pTail)
                    {
                        g_pTail->pNext = NULL;
                    }
                }
            }
            else
            {
                pNode->pNext->pPrev = pNode->pPrev;
                pNode->pPrev->pNext = pNode->pNext;
            }
            
            free(pNode->pStu);
            free(pNode);
            return pStu;
        }
        pNode = pNode->pNext;
    }

    return NULL;
}

STUDENT *Search_s(int id)
{
    DOUBLE_LINK_NODE *pNode = NULL;

    pNode = g_pHeader;
    while(NULL != pNode)
    {
        if (pNode->pStu->nId == id)
        {
            return pNode->pStu;
        }
        pNode = pNode->pNext;
    }

    return NULL;
}

void Sort_s()
{
    DOUBLE_LINK_NODE *p = NULL;
    DOUBLE_LINK_NODE *q = NULL;
    DOUBLE_LINK_NODE *pNode = NULL;
    bool isNeedChange = false;
    
    for (p = g_pHeader; NULL != p; p = p->pNext)
    {
        isNeedChange = false;
        for (q = p->pPrev; NULL != q; q = q->pPrev)
        {//插入排序
            if (p->pStu->nGrade > q->pStu->nGrade)
            {
                isNeedChange = true;
                break;
            }
        }
        if (isNeedChange)
        {
            q = q->pNext;//这个节点才是要交换的节点

            pNode = p->pNext;
            p->pNext = q->pNext;
            q->pNext = pNode;

            pNode = p->pPrev;
            p->pPrev = q->pPrev;
            q->pPrev = pNode;
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
    pNode = g_pHeader;
    while(NULL != pNode)
    {
        printf("%d\t%s\t%d\n", pNode->pStu->nId, pNode->pStu->sName, pNode->pStu->nGrade);
        pNode = pNode->pNext;
    }
}