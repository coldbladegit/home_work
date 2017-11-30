#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include <tchar.h>
#include <assert.h>
#include "school_mgr.h"
#include "cb_tree.h"
#include "err_no.h"
#include "cb_lock.h"
#include "cfg_mgr.h"

/************************************************************************/
/* 配置树节点：1、根节点为SCHOOL,GRADE为其子节点,STUDENT为GRADE子节点
               2、左子节点表示父子关系,右子节点表示兄弟关系
/************************************************************************/
static CB_TREE *g_cfgTree = NULL;
static CONFIG_MGR *g_cfgMgr = NULL;
static LOCK_HANDLE g_treeLock = NULL;
static LOCK_HANDLE g_idLock = NULL;
static int g_stuCnt = 0;
static int g_gradeCnt = 0;
static int g_stuId = 10000;
static int g_gradeId = 1;

static bool is_equal(void *pStruct, void *pHeader)
{
    STRUCT_HEADER *pH1 = NULL;
    STRUCT_HEADER *pH2 = NULL;

    assert (NULL != pStruct && NULL != pHeader);

    pH1 = (STRUCT_HEADER *) pStruct;
    pH2 = (STRUCT_HEADER *) pHeader;
    if (pH1->struct_type == pH2->struct_type)
    {
        return pH1->id == pH2->id;
    }

    return false;
}

static int get_student_count()
{
    int cnt;

    cb_lock(g_treeLock);
    cnt = g_stuCnt;
    cb_unlock(g_treeLock);

    return cnt;
}

static int get_grade_count ()
{
    int cnt;

    cb_lock(g_treeLock);
    cnt = g_gradeCnt;
    cb_unlock(g_treeLock);

    return cnt;
}

static int init_school(TCHAR *name)
{
    int ret = ERR_SUCCESS;
    SCHOOL *pSchool = NULL;

    assert(NULL != name);

    if (NULL == g_cfgTree->pRoot->pData)
    {
        cb_lock(g_treeLock);
        if (NULL == g_cfgTree->pRoot->pData)
        {
            pSchool = (SCHOOL *) malloc(sizeof(SCHOOL));
            if (NULL == pSchool)
            {
                ret = ERR_MALLOC_MEM_FAILD;
            }
            else
            {
                pSchool->header.id = 0;
                pSchool->header.struct_type = STRUCT_SCHOOL;
                _stprintf_s(pSchool->name, MAX_NAME_LEN, TEXT("%s"), name);

                g_cfgTree->pRoot->pData = pSchool;
            }
        }
        cb_unlock(g_treeLock);
    }

    return ret;
}

static int destroy_school()
{
    //TODO:
    return ERR_SUCCESS;
}

static int get_school(void **pNode)
{
    *pNode = g_cfgTree->pRoot;
    return ERR_SUCCESS;
}

static int add_grade(int gradeId, TCHAR *pMajTeacher)
{
    int ret = ERR_SUCCESS;
    GRADE *pGrade = NULL;
    CB_TREE_NODE *pNewNode = NULL;
    CB_TREE_NODE **ppNode = NULL;
    
    assert(NULL != pMajTeacher);

    pGrade = (GRADE *) malloc(sizeof(GRADE));
    if (NULL == pGrade)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pGrade->header.id = gradeId;
    pGrade->header.struct_type = STRUCT_GRADE;
    _stprintf_s(pGrade->majorTeacher, MAX_NAME_LEN, TEXT("%s"), pMajTeacher);

    ret = g_cfgTree->new_node(&pNewNode, pGrade);
    if (ERR_SUCCESS != ret)
    {
        free(pGrade);
        return ret;
    }

    cb_lock(g_treeLock);
    ppNode = &g_cfgTree->pRoot->pLChild;
    while (NULL != *ppNode)
    {
        ppNode = &((*ppNode)->pRChild);
    }
    *ppNode = pNewNode;
    g_gradeCnt++;
    cb_unlock(g_treeLock);

    return ERR_SUCCESS;
}

static int remove_grade(int gradeId)
{
    int ret = ERR_NOT_FIND;
    CB_TREE_NODE *pNode = NULL;
    CB_TREE_NODE **ppNode = NULL;
    STRUCT_HEADER header;

    header.id = gradeId;
    header.struct_type = STRUCT_GRADE;

    cb_lock(g_treeLock);
    ppNode = &g_cfgTree->pRoot->pLChild;
    while (NULL != *ppNode)
    {
        if (is_equal((*ppNode)->pData, &header))
        {
            ret = ERR_SUCCESS;
            pNode = *ppNode;
            *ppNode = NULL;
            g_gradeCnt--;
            break;
        }
        ppNode = &((*ppNode)->pRChild);
    }
    cb_unlock(g_treeLock);

    if (ERR_SUCCESS == ret)
    {
        g_cfgTree->delete_node(pNode);
    }

    return ret;
}

static int update_grade(int gradeId, TCHAR *pMajTeacher)
{
    int ret = ERR_SUCCESS;
    CB_TREE_NODE *pNode = NULL;
    GRADE *pGrade = NULL;
    STRUCT_HEADER header;

    assert(NULL != pMajTeacher);

    header.id = gradeId;
    header.struct_type = STRUCT_GRADE;
    cb_lock(g_treeLock);
    ret = g_cfgTree->search_node(g_cfgTree, &pNode, &header, is_equal);
    if (ERR_SUCCESS == ret)
    {
        pGrade = (GRADE *) pNode->pData;
        _stprintf_s(pGrade->majorTeacher, MAX_NAME_LEN, TEXT("%s"), pMajTeacher);
    }
    cb_unlock(g_treeLock);

    return ret;
}

static void insert_student_to_grade(CB_TREE_NODE *pGrade, CB_TREE_NODE *pStu)
{
    CB_TREE_NODE **ppNode = NULL;

    assert(NULL != pGrade);
    
    cb_lock(g_treeLock);
    ppNode = &pGrade->pLChild;
    while (NULL != *ppNode)
    {
        ppNode = &((*ppNode)->pRChild);
    }
    *ppNode = pStu;
    g_stuCnt++;
    cb_unlock(g_treeLock);
}

static CB_TREE_NODE *query_student_from_grade(CB_TREE_NODE *pGrade, int stuId)
{
    CB_TREE_NODE *pNode = NULL;
    STRUCT_HEADER header;

    assert(NULL != pGrade);

    header.id = stuId;
    header.struct_type = STRUCT_STUDENT;

    cb_lock(g_treeLock);
    pNode = pGrade->pLChild;
    while (NULL != pNode)
    {
        if (is_equal(pNode->pData, &header))
        {
            break;
        }
        pNode = pNode->pRChild;
    }
    cb_unlock(g_treeLock);

    return pNode;
}

static CB_TREE_NODE *remove_student_from_grade(CB_TREE_NODE *pGrade, int stuId)
{
    CB_TREE_NODE **ppNode = NULL;
    CB_TREE_NODE *pNode = NULL;
    STRUCT_HEADER header;

    assert(NULL != pGrade);

    header.id = stuId;
    header.struct_type = STRUCT_STUDENT;

    cb_lock(g_treeLock);
    ppNode = &pGrade->pLChild;
    while (NULL != *ppNode)
    {
        if (is_equal((*ppNode)->pData, &header))
        {
            pNode = *ppNode;
            *ppNode = NULL;
            g_stuCnt--;
            break;
        }
        ppNode = &((*ppNode)->pRChild);
    }
    cb_unlock(g_treeLock);

    return pNode;
}

static int add_student(int stuId, int gradeId, int score, TCHAR *name)
{
    int ret = ERR_SUCCESS;
    CB_TREE_NODE *pNewNode = NULL;
    CB_TREE_NODE *pNode = NULL;
    STUDENT *pStu = NULL;
    STRUCT_HEADER header;

    assert(NULL != name);

    header.id = gradeId;
    header.struct_type = STRUCT_GRADE;
    ret = g_cfgTree->search_node(g_cfgTree, &pNode, &header, is_equal);
    if (ERR_SUCCESS != ret)
    {//没有找到对应的班级
        return ret;
    }

    pStu = (STUDENT *) malloc(sizeof(STUDENT));
    if (NULL == pStu)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pStu->header.id = stuId;
    pStu->header.struct_type = STRUCT_STUDENT;
    pStu->gradeId = gradeId;
    _stprintf_s(pStu->name, MAX_NAME_LEN, TEXT("%s"), name);
    pStu->totalScore = score;

    ret = g_cfgTree->new_node(&pNewNode, pStu);
    if (ERR_SUCCESS == ret)
    {
        insert_student_to_grade(pNode, pNewNode);
    }
    else
    {
        free(pStu);
    }

    return ERR_SUCCESS;
}

static int remove_student(int gradeId, int stuId)
{
    int ret = ERR_SUCCESS;
    CB_TREE_NODE *pNode = NULL;
    STRUCT_HEADER header;

    header.id = gradeId;
    header.struct_type = STRUCT_GRADE;

    cb_lock(g_treeLock);
    ret = g_cfgTree->search_node(g_cfgTree, &pNode, &header, is_equal);
    cb_unlock(g_treeLock);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    pNode = remove_student_from_grade(pNode, stuId);
    if (NULL == pNode)
    {
        ret = ERR_NOT_FIND;
    }
    else
    {
        g_cfgTree->delete_node(pNode);
    }

    return ret;
}

static int update_student_score(int gradeId, int stuId, int score)
{
    int ret = ERR_SUCCESS;
    CB_TREE_NODE *pNode = NULL;
    STRUCT_HEADER header;

    header.id = gradeId;
    header.struct_type = STRUCT_GRADE;

    cb_lock(g_treeLock);
    ret = g_cfgTree->search_node(g_cfgTree, &pNode, &header, is_equal);
    cb_unlock(g_treeLock);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    pNode = query_student_from_grade(pNode, stuId);
    if (NULL == pNode)
    {
        ret = ERR_NOT_FIND;
    }
    else
    {
        ((STUDENT *) pNode->pData)->totalScore = score;
    }

    return ret;
}

static int update_student_grade(int oldGradeId, int newGradeId, int stuId)
{
    int ret = ERR_SUCCESS;
    CB_TREE_NODE *pOldGradeNode = NULL;
    CB_TREE_NODE *pNewGradeNode = NULL;
    CB_TREE_NODE *pStuNode = NULL;
    STRUCT_HEADER header;

    header.id = oldGradeId;
    header.struct_type = STRUCT_GRADE;
    //查询学生对应的原班级
    ret = g_cfgTree->search_node(g_cfgTree, &pOldGradeNode, &header, is_equal);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    //查询学生对应的将要更新的班级
    header.id = newGradeId;
    ret = g_cfgTree->search_node(g_cfgTree, &pNewGradeNode, &header, is_equal);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    pStuNode = remove_student_from_grade(pOldGradeNode, stuId);
    if (NULL == pStuNode)
    {
        ret = ERR_NOT_FIND;
    }
    else
    {
        ((STUDENT *) pStuNode->pData)->gradeId = newGradeId;
        insert_student_to_grade(pNewGradeNode, pStuNode);
    }

    return ret;
}

int new_school_mgr(SCHOOL_MGR **ppMgr)
{
    int ret = ERR_SUCCESS;
    SCHOOL_MGR *pMgr = NULL;

    do 
    {
        pMgr = (SCHOOL_MGR *) malloc(sizeof(SCHOOL_MGR));
        if (NULL == pMgr)
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }
        ret = new_config_mgr(&g_cfgMgr);//config manager
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        ret = cb_new_tree(&g_cfgTree, NULL, true);//config tree
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        g_treeLock = cb_createLock();//global tree lock
        g_idLock = cb_createLock();//global lock for grade and student id
        pMgr->init_school = init_school;
        pMgr->destroy_school = destroy_school;
        pMgr->get_school = get_school;
        pMgr->get_grade_count = get_grade_count;
        pMgr->get_student_count = get_student_count;
        pMgr->add_grade = add_grade;
        pMgr->update_grade = update_grade;
        pMgr->remove_grade = remove_grade;
        pMgr->add_student = add_student;
        pMgr->remove_student = remove_student;
        pMgr->update_student_score = update_student_score;
        pMgr->update_student_grade = update_student_grade;

        ret = g_cfgMgr->read_config(pMgr);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        *ppMgr = pMgr;
    } while (0);  
    if (ERR_SUCCESS != ret)
    {
        if (NULL != g_cfgMgr)
        {
            delete_config_mgr(g_cfgMgr);
            g_cfgMgr = NULL;
        }
        if (NULL != pMgr)
        {
            free(pMgr);
        }
    }

    return ret;
}

int delete_school_mgr(SCHOOL_MGR *pMgr)
{
    int ret = ERR_SUCCESS;

    assert(NULL != pMgr);

    do 
    {
        ret = g_cfgMgr->write_config(pMgr);//保存配置
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        ret = cb_delete_tree(g_cfgTree);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        delete_config_mgr(g_cfgMgr);
        cb_destroyLock(g_treeLock);
        cb_destroyLock(g_idLock);
        free(pMgr);
    } while (0);

    return ret;
}

int generate_grade_id()
{
    int id;

    cb_lock(g_idLock);
    id = g_gradeId;
    g_gradeId++;
    cb_unlock(g_idLock);

    return id;
}

int generate_student_id()
{
    int id;

    cb_lock(g_idLock);
    id = g_stuId;
    g_stuId++;
    cb_unlock(g_idLock);

    return id;
}