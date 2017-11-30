#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include "school_mgr.h"
#include "err_no.h"

int _tmain (int argc, TCHAR **argv)
{
    int ret = ERR_SUCCESS;
    SCHOOL_MGR *pMgr = NULL;

    setlocale(LC_ALL, "");

    ret = new_school_mgr(&pMgr);
    if (ERR_SUCCESS != ret)
    {
        _tprintf(TEXT("create school manager failed, err = %d\n"), ret);
        return 0;
    }
    pMgr->init_school(TEXT("SWPU"));
    pMgr->add_grade(generate_grade_id(), TEXT("Li Mei"));
    pMgr->add_grade(generate_grade_id(), TEXT("����"));
    pMgr->add_grade(generate_grade_id(), TEXT("����"));
    pMgr->add_student(generate_student_id(), 1, 0, TEXT("����"));
    pMgr->add_student(generate_student_id(), 2, 0, TEXT("����"));
    pMgr->add_student(generate_student_id(), 3, 0, TEXT("����"));
    
    ret = delete_school_mgr(pMgr);
    if (ERR_SUCCESS != ret)
    {
        _tprintf(TEXT("delete school manager failed, err = %d\n"), ret);
    }

    return 0;
}