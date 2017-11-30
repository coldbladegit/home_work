#include <WinSock2.h>
#include <memory.h>
#include <stdio.h>
#include "err_no.h"
#include "cb_util.h"
#include "student.h"
#include "cb_vector.h"

static int sizeof_stu_net(STUDENT *pStu, int *stuSize)
{
    *stuSize = sizeof(STUDENT) + (pStu->nameLen + pStu->homeLen) * sizeof(wchar_t);
    return *stuSize - sizeof(wchar_t *) * 2;
}

static int sizeof_stu(STUDENT_NET *pStuNet, int *stuNetSize)
{
    *stuNetSize = sizeof(STUDENT_NET);
    *stuNetSize += ntohs(pStuNet->nameLen) * sizeof(wchar_t);
    *stuNetSize += ntohs(pStuNet->homeLen) * sizeof(wchar_t);

    return *stuNetSize + sizeof(wchar_t *) * 2;
}

static int sizeof_stus(STUDENT_LIST_NET *pStuListNet, int stuCnt)
{
    char *pDataBuf = NULL;
    STUDENT_NET *pStuNet = NULL;
    int size = 0, stuNetSize;

    pDataBuf = (char *) (pStuListNet + 1);
    for (int index = 0; index < stuCnt; ++index)
    {
        pStuNet = (STUDENT_NET *) pDataBuf;
        size += sizeof_stu(pStuNet, &stuNetSize);
        pDataBuf += stuNetSize;
    }

    return size;
}

static void hton_wchar_array(wchar_t *pSrc, wchar_t *pDest, int len)
{
    for (int index = 0; index < len; ++index)
    {
        pDest[index] = htons(pSrc[index]);
    }
}

static void ntoh_wchar_array(wchar_t *pSrc, wchar_t *pDest, int len)
{
    for (int index = 0; index < len; ++index)
    {
        pDest[index] = ntohs(pSrc[index]);
    }
}

static void hton_stu(STUDENT *pStu, STUDENT_NET *pStuNet)
{
    wchar_t *pDataBuf = NULL;

    pStuNet->stuNo = htons(pStu->stuNo);
    pStuNet->arge = htons(pStu->arge);
    pStuNet->nameLen = htons(pStu->nameLen);
    pStuNet->homeLen = htons(pStu->homeLen);
    memcpy(pStuNet->borthday, pStu->borthday, BORTHDAY_LEN);

    pDataBuf = (wchar_t *) (pStuNet + 1);
    hton_wchar_array(pStu->name, pDataBuf, pStu->nameLen);
    hton_wchar_array(pStu->home, pDataBuf + pStu->nameLen, pStu->homeLen);
}

static void ntoh_stu(STUDENT *pStu, STUDENT_NET *pStuNet)
{
    wchar_t *pDataBuf = NULL;

    pStu->stuNo = ntohs(pStuNet->stuNo);
    pStu->arge = ntohs(pStuNet->arge);
    pStu->nameLen = ntohs(pStuNet->nameLen);
    pStu->homeLen = ntohs(pStuNet->homeLen);
    memcpy(pStu->borthday, pStuNet->borthday, BORTHDAY_LEN);
    pStu->name = (wchar_t *)(pStu + 1);
    pStu->home = pStu->name + pStu->nameLen;

    pDataBuf = (wchar_t *) (pStuNet + 1);
    ntoh_wchar_array(pDataBuf, pStu->name, pStu->nameLen);
    ntoh_wchar_array(pDataBuf + pStu->nameLen, pStu->home, pStu->homeLen);
}

int to_host_stu(STUDENT_NET_HANDLE hStuNet, STUDENT_HANDLE *pHStu)
{
    int size = 0, stuNetSize = 0;
    STUDENT *pStu = NULL;
    STUDENT_NET *pStuNet = NULL;

    G_ASSERT(NULL != hStuNet);

    pStuNet = (STUDENT_NET *) hStuNet;
    size = sizeof_stu(pStuNet, &stuNetSize);
    pStu = (STUDENT *) malloc(size);
    if (NULL == pStu)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    ntoh_stu(pStu, pStuNet);
    *pHStu = pStu;

    return ERR_SUCCESS;
}

int to_net_stu(STUDENT_HANDLE hStu, STUDENT_NET_HANDLE *pHStuNet, int *dataLen)
{
    int size = 0, sutSize = 0;
    STUDENT *pStu = NULL;
    STUDENT_NET *pStuNet = NULL;

    G_ASSERT(NULL != hStu);

    pStu = (STUDENT *) hStu;
    size = sizeof_stu_net(pStu, &sutSize);
    pStuNet = (STUDENT_NET *) malloc(size);
    if (NULL == pStuNet)
    {
        return ERR_MALLOC_MEM_FAILD;
    } 
    *dataLen = size;
    hton_stu(pStu, pStuNet);
    *pHStuNet = pStuNet;

    return ERR_SUCCESS;
}

int to_host_stu_list(STUDENT_LIST_NET_HANDLE hStuListNet, STUDENT_LIST_HANDLE *pHStuList)
{
    int stuCnt = 0, stuSize = 0, stuNetSize = 0;
    char *pDataBuf = NULL;
    char *pBuf = NULL;
    STUDENT_NET *pStuNet = NULL;
    STUDENT_LIST *pStuList = NULL;
    STUDENT_LIST_NET *pStuListNet = NULL;

    G_ASSERT(NULL != hStuListNet);

    pStuListNet = (STUDENT_LIST_NET *) hStuListNet;
    stuCnt = ntohs(pStuListNet->stuCnt);
    pStuList = (STUDENT_LIST *) malloc(sizeof(STUDENT_LIST) + stuCnt * sizeof(STUDENT *) + sizeof_stus(pStuListNet, stuCnt));
    if (NULL == pStuList)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    pStuList->stuCnt = stuCnt;
    pStuList->pStus = (STUDENT **) (pStuList + 1);
    pBuf = (char *) (pStuList->pStus + stuCnt);//student内容真正开始的位置
    pDataBuf = (char *)(pStuListNet + 1);
    
    for (int index = 0; index < pStuList->stuCnt; ++index)
    {
        pStuNet = (STUDENT_NET *) pDataBuf;
        pStuList->pStus[index] = (STUDENT *) pBuf;
        stuSize = sizeof_stu(pStuNet, &stuNetSize);
        ntoh_stu(pStuList->pStus[index], pStuNet);
        pDataBuf += stuNetSize;
        pBuf += stuSize;
    }
    *pHStuList = pStuList;

    return ERR_SUCCESS;
}

int vec_to_net_stu_list(void *p, STUDENT_LIST_NET_HANDLE *pHStuListNet, int *dataLen)
{
    int size = 0, stuSize = 0;
    CB_VECTOR *pVec = NULL;
    char *pDataBuf = NULL;
    STUDENT *pStu = NULL;
    STUDENT_LIST_NET *pStuListNet = NULL;

    G_ASSERT(NULL != p);

    pVec = (CB_VECTOR *) p;
    for (int index = 0; index < pVec->size; ++index)
    {
        size += sizeof_stu_net((STUDENT *)pVec->ppDatas[index], &stuSize);
    }
    size += sizeof(STUDENT_LIST_NET);
    pStuListNet = (STUDENT_LIST_NET *) malloc(size);
    if (NULL == pStuListNet)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    *dataLen = size;
    pStuListNet->stuCnt = htons(pVec->size);
    pDataBuf = (char *)(pStuListNet + 1);
    for (int index = 0; index < pVec->size; ++index)
    {
        pStu = (STUDENT *)pVec->ppDatas[index];
        hton_stu(pStu, (STUDENT_NET *)pDataBuf);
        pDataBuf += sizeof_stu_net(pStu, &stuSize);
    }
    *pHStuListNet = pStuListNet;

    return ERR_SUCCESS;
}

void print_stu(STUDENT_HANDLE hStu)
{
    G_ASSERT(NULL != hStu);

    STUDENT *pStu = (STUDENT *) hStu;
    wprintf_s(L"学号\t年龄\t姓名\t\t籍贯\t\t\t生日\n");
    wprintf_s(L"%d\t%d\t%s\t%s\t", pStu->stuNo, pStu->arge, pStu->name, pStu->home);
    printf("%s\n",pStu->borthday);
}

void print_stu_list(STUDENT_LIST_HANDLE hStuList)
{
    STUDENT_LIST *pStuList = NULL;
    STUDENT *pStu = NULL;

    G_ASSERT(NULL != hStuList);

    pStuList = (STUDENT_LIST *) hStuList;
    wprintf_s(L"学号\t年龄\t姓名\t\t籍贯\t\t\t生日\n");
    for (int index = 0; index < pStuList->stuCnt; ++index)
    {
        pStu = (STUDENT *) pStuList->pStus[index];
        wprintf_s(L"%d\t%d\t%s\t%s\t", pStu->stuNo, pStu->arge, pStu->name, pStu->home);
        printf("%s\n",pStu->borthday);
    }
}