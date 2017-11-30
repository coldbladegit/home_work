#include <windows.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <wchar.h>
#include "sock_mgr.h"
#include "stu_mgr_service.h"
#include "err_no.h"
#include "student.h"
#include "cb_lock.h"
#include "cb_vector.h"
#include "session.h"
#include "cb_util.h"

#define MAX_CONN_CNT        5
#define MAX_STU_CNT         20
#define DEF_STU_CNT         10

static bool g_isStop = false;
static SOCK_MGR *g_pSockMgr = NULL;
static SESSION_HANDLE g_hSessions[MAX_CONN_CNT] = {NULL};
static LOCK_HANDLE g_hLock = NULL;
static CB_VECTOR *g_pVecRecvData = NULL;
static CB_VECTOR *g_pVecStu = NULL;

static bool is_same_student(void *p, int stuNo)
{
    STUDENT *pStu = NULL;

    if (NULL == p)
    {
        return false;
    }
    pStu = (STUDENT *) p;

    return pStu->stuNo == stuNo;
}

static int deal_recv_data(void *data)
{
    int ret = ERR_SUCCESS;
    RECV_DATA *pRecvData = NULL;
    RECV_DATA *pData = NULL;

    G_ASSERT(NULL != data);
    
    pData = (RECV_DATA *) data;
    pRecvData = (RECV_DATA *) malloc(sizeof(RECV_DATA) + pData->dataLen);
    if (NULL == pRecvData)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pRecvData->cmd = pData->cmd;
    pRecvData->errNo = pData->errNo;
    pRecvData->dataLen = pData->dataLen;
    pRecvData->hSession = pData->hSession;
    if (pData->dataLen == 0)
    {
        pRecvData->pBuf = NULL;
    }
    else
    {
        pRecvData->pBuf = (char *)pRecvData + sizeof(RECV_DATA);//等价于(char *)(pRecvData + 1)
        memcpy_s(pRecvData->pBuf, pRecvData->dataLen, pData->pBuf, pData->dataLen);
    }

    cb_lock(g_hLock);
    ret = g_pVecRecvData->add_element(g_pVecRecvData, pRecvData);
    cb_unlock(g_hLock);
    return ret;
}

static void free_element(void *pElem)
{
    if (NULL != pElem)
    {
        free(pElem);
    }
}

//stuNo = -1 表示查询所有
static void inner_query_stu(int cmd, int stuNo, SESSION_HANDLE hSession)
{
    int ret = ERR_SUCCESS;
    STUDENT_HEADER header;
    char *pBuf = NULL;
    STUDENT *pStu = NULL;
    int dataLen  = 0;

    if (-1 == stuNo)
    {//查询所有学生信息
        STUDENT_LIST_NET_HANDLE hStuListNet;
        ret = vec_to_net_stu_list(g_pVecStu, &hStuListNet, &dataLen);
        if (ERR_SUCCESS == ret)
        {  
           pBuf = (char *)hStuListNet;
        }
    }
    else
    {//查询单个学生信息
        pStu = (STUDENT *) g_pVecStu->search_element(g_pVecStu, stuNo, is_same_student);
        if (NULL == pStu)
        {
            ret = ERR_NOT_FIND;
        }
        else
        {
            STUDENT_NET_HANDLE hStuNet;
            ret = to_net_stu(pStu, &hStuNet, &dataLen);
            if (ERR_SUCCESS == ret)
            {
                pBuf = (char *) hStuNet;
            }
        }
    }
    header.cmd = htons(cmd);
    header.errNo = htons(ret);
    header.dataLen = htons(dataLen);
    //先发送header
    ret = send_data(hSession, (char *) &header, sizeof(STUDENT_HEADER));
    if (ERR_SUCCESS == ret && NULL != pBuf)
    {//发送真实数据
        ret = send_data(hSession, pBuf, dataLen);
    }
    if (ERR_SUCCESS != ret)
    {
        printf("service send_data failed, [errNo = %d]\n", ret);
    }
    if (NULL != pBuf)
    {//TODO:这片空间其实可以重用的
        free(pBuf);
    }
}

static void inner_deal_recv_data(RECV_DATA *pRecvData)
{
    STRUCT_INT *pStruct = NULL;

    switch(pRecvData->cmd)
    {
    case CMD_QUERY_BY_STUNO:
        pStruct = (STRUCT_INT *) pRecvData->pBuf;
        inner_query_stu(pRecvData->cmd, ntohs(pStruct->iValue), pRecvData->hSession);
        break;
    case CMD_QUERY_ALL:
        inner_query_stu(pRecvData->cmd, -1, pRecvData->hSession);
        break;
    default:
        break;
    }
}

static DWORD WINAPI deal_request(void *p)
{//所有session的接收线程接收到的数据统一在这里处理
    void *pData = NULL;
    int ret = ERR_SUCCESS;

    while(!g_isStop)
    {
        cb_lock(g_hLock);
        ret = g_pVecRecvData->remove_element(g_pVecRecvData, &pData);
        cb_unlock(g_hLock);
        if(ERR_SUCCESS == ret)
        {//TODO:优化方案可以采用线程池
            inner_deal_recv_data((RECV_DATA *)pData);
            free(pData);
        }
        else
        {
            Sleep(50);
        }
    }

    return ERR_SUCCESS;
}

static STUDENT *BuildStudent(int stuNo, int arge, wchar_t *name, wchar_t *home, char *borthday)
{
    int ret = ERR_SUCCESS;
    STUDENT *pStu = NULL;
    int nameLen ,homeLen ,borthdayLen;

    G_ASSERT(NULL != name && NULL != home && NULL != borthday);

    borthdayLen = strlen(borthday) + 1;
    G_ASSERT(borthdayLen <= BORTHDAY_LEN);

    nameLen = wcslen(name) + 1;
    homeLen = wcslen(home) + 1;
    

    pStu = (STUDENT *) malloc(sizeof(STUDENT) + (nameLen + homeLen) * sizeof(wchar_t));
    if (NULL == pStu)
    {
        return NULL;
    }
    pStu->stuNo = stuNo;
    pStu->arge = arge;
    pStu->nameLen = nameLen;
    pStu->homeLen = homeLen;
    pStu->name = (wchar_t *)(pStu + 1);
    wmemcpy(pStu->name, name, nameLen);
    pStu->home = pStu->name + nameLen;
    wmemcpy(pStu->home, home, homeLen);
    sprintf_s(pStu->borthday, BORTHDAY_LEN, borthday);

    return pStu;
}

static int init_student_vec()
{
    int ret = ERR_SUCCESS;
    int stuNo, arge;
    wchar_t name[20];
    wchar_t *home = L"四川省成都市高新西区";
    char borthday[BORTHDAY_LEN];
    STUDENT *pStu = NULL;

    for (int index = 0; index < DEF_STU_CNT; ++index)
    {
        stuNo = 1000 + index;
        arge = 10 + index % 4;
        swprintf_s(name, 20, L"%s_%d", L"cold_blade", index + 1);
        sprintf_s(borthday, BORTHDAY_LEN, "%d-12-12", 2017 - arge);
        pStu = BuildStudent(stuNo, arge, name, home, borthday);
        if (NULL == pStu)
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }
        ret = g_pVecStu->add_element(g_pVecStu, pStu);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
    }

    return ret;
}

int init_stu_service()
{
    int ret = ERR_SUCCESS;

    do 
    {
        g_pSockMgr = (SOCK_MGR *) malloc(sizeof(SOCK_MGR));
        if (NULL == g_pSockMgr)
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }
        ret = new_cb_vector(&g_pVecRecvData);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        ret = new_cb_vector(&g_pVecStu);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        if (ERR_SUCCESS != init_student_vec())
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }
    } while (0);
    if (ERR_SUCCESS == ret)
    {
        g_hLock = cb_createLock();
    }
    else
    {
        release_stu_service();
    }

    return ret;
}

void release_stu_service()
{
    if (NULL != g_pVecRecvData)
    {
        g_pVecRecvData->remove_all(g_pVecRecvData, free_element);
        delete_cb_vector(g_pVecRecvData);
    }
    if (NULL != g_pVecStu)
    {
        g_pVecStu->remove_all(g_pVecStu, free_element);
        delete_cb_vector(g_pVecStu);
    }
    for (int index = 0; index < MAX_CONN_CNT; ++index)
    {
        if (NULL != g_hSessions[index])
        {
            delete_session(g_hSessions[index]);
        }
    }
    if (NULL != g_hLock)
    {
        cb_destroyLock(g_hLock);
    }
    if (NULL != g_pSockMgr)
    {
        release_sock_mgr();
        free(g_pSockMgr);
    }
}

static int get_idle_session(SESSION_HANDLE *pHSession)
{
    int minIndex = -1;

    for (int index = 0; index < MAX_CONN_CNT; ++index)
    {
        if (NULL != g_hSessions[index] && is_idle(g_hSessions[index]))
        {
            *pHSession = g_hSessions[index];
            return ERR_SUCCESS;
        }
        if (-1 == minIndex && NULL == g_hSessions[index])
        {
            minIndex = index;
        }
    }
    if (-1 == minIndex)
    {
        return ERR_NOT_FIND;
    }

    return new_session(pHSession, g_pSockMgr, deal_recv_data);
}

int start_stu_service()
{
    int ret = ERR_SUCCESS;
    HANDLE dealThread = NULL;
    DWORD threadId;
    SOCK_HANDLE hSSock = NULL;
    SOCK_HANDLE hCSock = NULL;
    SESSION_HANDLE hSession = NULL;

    ret = init_sock_mgr(g_pSockMgr);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    ret = g_pSockMgr->new_tcp_sock(&hSSock, SOCK_LISTEN_ADDR, SOCK_LISTEN_PORT);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    do 
    {
        ret = g_pSockMgr->do_bind(hSSock);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        ret = g_pSockMgr->do_listen(hSSock, MAX_CONN_CNT);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        dealThread = CreateThread(NULL, 0, deal_request, NULL, 0, &threadId);
        if (NULL == dealThread)
        {
            ret = GetLastError();
            break;
        }
        
        while (!g_isStop)
        {
            ret = g_pSockMgr->do_accept(&hCSock, hSSock);
            if (ERR_SUCCESS != ret)
            {
                continue;
            }
            ret = get_idle_session(&hSession);
            if (ERR_SUCCESS != ret)
            {//内存已经不足
                break;
            }
            ret = start_session(hSession, hCSock);
            if (ERR_SUCCESS != ret)
            {
                printf("start session failed, [errNo = %d]\n", ret);
            }
        }
    } while (0);
    if (NULL != dealThread)
    {
        CloseHandle(dealThread);
    }
    g_pSockMgr->delete_tcp_sock(hSSock);
    return ret;
}

int stop_stu_service()
{
    g_isStop = true;
    for (int index = 0; index < MAX_CONN_CNT; ++index)
    {
        if (NULL != g_hSessions[index])
        {
            stop_session(g_hSessions[index]);
        }
    }
    return ERR_SUCCESS;
}