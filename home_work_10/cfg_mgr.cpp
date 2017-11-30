#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>
#include <assert.h>
#include "cfg_mgr.h"
#include "err_no.h"
#include "cb_tree.h"
#include "school_mgr.h"

#define TAG_ID                  TEXT("ID = ")
#define TAG_NAME                TEXT("NAME = ")
#define TAG_SCHOOL              TEXT("[SCHOOL]")
#define TAG_GRADE               TEXT("[GRADE]")
#define TAG_GRADE_TEACHER       TEXT("TEACHER = ")
#define TAG_STUDENT             TEXT("[STUDENT]")
#define TAG_STUDENT_SCORE       TEXT("SCORE = ")
#define TAG_STUDENT_GRADE       TEXT("GRADE = ")
#define CONFIG_PATH             TEXT("cfg.txt")
#define BUF_SIZE                1024

static int read_school(TCHAR *pBuf, FILE *pStream, SCHOOL_MGR *pMgr)
{
    int ret = ERR_SUCCESS;
    int offset = 0;
    TCHAR *name = NULL;

    if (_fgetts(pBuf, BUF_SIZE, pStream) != NULL)
    {
        offset = _tcslen(TAG_NAME);
        name = pBuf + offset;
        ret = pMgr->init_school(name);
    }
    else
    {
        ret = ferror(pStream);
    }

    return ERR_SUCCESS;
}

static int read_grade(TCHAR *pBuf, FILE *pStream, SCHOOL_MGR *pMgr)
{
    int ret = ERR_SUCCESS;
    int id = 0;
    int offset = 0;
    TCHAR *teacher = NULL;

    do 
    {
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_ID);
        id = _ttoi(pBuf + offset);
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_NAME);
        teacher = pBuf + offset;
    } while (0);

    if (ERR_SUCCESS == ret)
    {
        ret = pMgr->add_grade(id, teacher);
    }

    return ret;
}

static int read_student(TCHAR *pBuf, FILE *pStream, SCHOOL_MGR *pMgr)
{
    int ret = ERR_SUCCESS;
    int id = 0, grade = 0, score = 0;
    int offset = 0;
    TCHAR *name = NULL;

    do 
    {
        //student id
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_ID);
        id = _ttoi(pBuf + offset);
        //student grade
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_STUDENT_GRADE);
        grade = _ttoi(pBuf + offset);
        //student score
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_STUDENT_SCORE);
        score = _ttoi(pBuf + offset);
        //student name
        if (_fgetts(pBuf, BUF_SIZE, pStream) == NULL)
        {
            ret = ferror(pStream);
            break;
        }
        offset = _tcslen(TAG_NAME);
        name = pBuf + offset;
    } while (0);
    if (ERR_SUCCESS == ret)
    {
        ret = pMgr->add_student(id, grade, score, name);
    }

    return ret;
}

static int read_config(void *p)
{
    int ret = ERR_SUCCESS;
    bool isParsingSchool = false;
    bool isParsingGrade = false;
    bool isParsingStudent = false;
    FILE *pStream = NULL;
    SCHOOL_MGR *pMgr = NULL;
    TCHAR buf[BUF_SIZE];
    
    assert(NULL != p);
    pMgr = (SCHOOL_MGR *) p;

    ret = _tfopen_s(&pStream, CONFIG_PATH, TEXT("a+"));
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    while (!feof(pStream) && ERR_SUCCESS == ret)
    {
        if (_fgetts(buf, BUF_SIZE, pStream) == NULL)
        {
            break;
        }

        if (NULL != _tcsstr(buf, TAG_SCHOOL))
        {
            if (isParsingSchool)
            {
                isParsingSchool = false;
            }
            else
            {
                isParsingSchool = true;
                ret = read_school(buf, pStream, pMgr);
            }
        }
        else if (NULL != _tcsstr(buf, TAG_GRADE))
        {
            if (isParsingGrade)
            {
                isParsingGrade = false;
            }
            else
            {
                isParsingGrade = true;
                ret = read_grade(buf, pStream, pMgr);
            }
        }
        else if (NULL != _tcsstr(buf, TAG_STUDENT))
        {
            if (isParsingStudent)
            {
                isParsingStudent = false;
            }
            else
            {
                isParsingStudent = true;
                ret = read_student(buf, pStream, pMgr);
            }
        }
    }
    fclose(pStream);

    return ERR_SUCCESS;
}

static int write_student(CB_TREE_NODE *pStuNode, FILE *pStream)
{
    TCHAR buf[BUF_SIZE];
    STUDENT *pStu = NULL;

    pStu = (STUDENT *) pStuNode->pData;
    
    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_STUDENT);
    _fputts(buf, pStream);

    //student id
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%d\n"), TAG_ID, pStu->header.id);
    _fputts(buf, pStream);
    //student grade
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%d\n"), TAG_STUDENT_GRADE, pStu->gradeId);
    _fputts(buf, pStream);
    //student score
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%d\n"), TAG_STUDENT_SCORE, pStu->totalScore);
    _fputts(buf, pStream);
    //student name
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%s\n"), TAG_NAME, pStu->name);
    _fputts(buf, pStream);

    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_STUDENT);
    _fputts(buf, pStream);

    return ERR_SUCCESS;
}

static int write_grade(CB_TREE_NODE *pGradeNode, FILE *pStream)
{
    TCHAR buf[BUF_SIZE];
    GRADE *pGrade = NULL;
    CB_TREE_NODE *pStuNode = NULL;

    pGrade = (GRADE *) pGradeNode->pData;

    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_GRADE);
    _fputts(buf, pStream);

    //grade id
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%d\n"), TAG_ID, pGrade->header.id);
    _fputts(buf, pStream);
    //grade teacher
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%s\n"), TAG_GRADE_TEACHER, pGrade->majorTeacher);
    _fputts(buf, pStream);

    //students of grade
    pStuNode = pGradeNode->pLChild;
    while(NULL != pStuNode)
    {
        write_student(pStuNode, pStream);
        pStuNode = pStuNode->pRChild;
    }

    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_GRADE);
    _fputts(buf, pStream);

    return ERR_SUCCESS;
}

static int write_school(CB_TREE_NODE *pSchoolNode, FILE *pStream)
{
    TCHAR buf[BUF_SIZE];
    SCHOOL *pSchool = NULL;
    CB_TREE_NODE *pGradeNode = NULL;

    pSchool = (SCHOOL *) pSchoolNode->pData;

    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_SCHOOL);
    _fputts(buf, pStream);

    //school id
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%d\n"), TAG_ID, pSchool->header.id);
    _fputts(buf, pStream);
    //school name
    _stprintf_s(buf, BUF_SIZE, TEXT("%s%s\n"), TAG_NAME, pSchool->name);
    _fputts(buf, pStream);

    //grades of school
    pGradeNode = pSchoolNode->pLChild;
    while(NULL != pGradeNode)
    {
        write_grade(pGradeNode, pStream);
        pGradeNode = pGradeNode->pRChild;
    }

    _stprintf_s(buf, BUF_SIZE, TEXT("%s\n"), TAG_SCHOOL);
    _fputts(buf, pStream);

    return ERR_SUCCESS;
}

static int write_config(void *p)
{
    int ret = ERR_SUCCESS;
    FILE *pStream = NULL;
    void *q = NULL;
    CB_TREE_NODE *pSchool = NULL;
    SCHOOL_MGR *pMgr = NULL;

    assert(NULL != p);
    pMgr = (SCHOOL_MGR *) p;

    ret = _tfopen_s(&pStream, CONFIG_PATH, TEXT("w"));
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    ret = pMgr->get_school(&q);
    if (ERR_SUCCESS == ret)
    {
        pSchool = (CB_TREE_NODE *)q;
        if (NULL != pSchool->pData)
        {
            ret = write_school(pSchool, pStream);
        }
    }
    fclose(pStream);
    
    return ret;
}

int new_config_mgr(CONFIG_MGR **ppMgr)
{
    CONFIG_MGR *pMgr = NULL;

    pMgr = (CONFIG_MGR *) malloc(sizeof(CONFIG_MGR));
    if (NULL == pMgr)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pMgr->read_config = read_config;
    pMgr->write_config = write_config;

    *ppMgr = pMgr;

    return ERR_SUCCESS;
}

void delete_config_mgr(CONFIG_MGR *pMgr)
{
    assert(NULL != pMgr);

    free(pMgr);
}