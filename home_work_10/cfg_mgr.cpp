#include <tchar.h>
#include <malloc.h>
#include "cfg_mgr.h"
#include "education.h"
#include "err_no.h"

#define TAG_SCHOOL      TEXT("[SCHOOL]")
#define TAG_GRADE       TEXT("[GRADE]")
#define TAG_STUDENT     TEXT("[STUDENT]")
#define HIERARCHY_TAB   4

typedef struct _CONFIG_TREE_NODE{
    HIERARCHY         hierarchy;
    void              *pData;
    _CONFIG_TREE_NODE *pChild;
    _CONFIG_TREE_NODE *pBorther;
}CONFIG_TREE_NODE;

static int CreateTreeNode(HIERARCHY hierarchy, void *pData, CONFIG_TREE_NODE **ppNode)
{
    CONFIG_TREE_NODE *pNode = NULL;

    pNode = (CONFIG_TREE_NODE *) malloc(sizeof(CONFIG_TREE_NODE));
    if (NULL == pNode)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pNode->hierarchy = hierarchy;
    pNode->pData = pData;
    pNode->pBorther = NULL;
    pNode->pChild = NULL;

    return ERR_SUCCESS;
}

int ReadConfig()
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

int WriteConfig()
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

int SaveSchool(void *pData)
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

int SaveGrade(void *pData)
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

int InsertStudent(void *pData)
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

void *SearchStudent(int stuNo)
{
    STUDENT *pStu = NULL;
    //TODO:
    return pStu;
}

void *RemoveStudent(int stuNo)
{
    STUDENT *pStu = NULL;
    //TODO:
    return pStu;
}

int UpdateStudentGrade(int stuNo, int grade)
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}

int UpdateStudentScore(int stuNo, int score)
{
    int ret = ERR_SUCCESS;
    //TODO:
    return ret;
}