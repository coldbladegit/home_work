#include <stdio.h>
#include <memory.h>
#include <WinSock2.h>
#include <malloc.h>
#include "sock_mgr.h"
#include "err_no.h"
#include "student.h"
#include "session.h"
#include "cb_util.h"
#include "stu_mgr_client.h"

static bool g_isStop = false;
static bool g_isRecved = false;

static int deal_recv_data(void *pData)
{
    RECV_DATA *pRecvData = NULL;
    int errNo = 0, ret = ERR_SUCCESS;

    pRecvData = (RECV_DATA *) pData;
    errNo = pRecvData->errNo;
    printf("cmd = %d\n", pRecvData->cmd);
    switch(pRecvData->cmd)
    {
    case CMD_QUERY_BY_STUNO:
        if (ERR_SUCCESS != errNo)
        {
            printf("query student failed, [errNo = %d]\n", errNo);
        }
        else
        {
            printf("client recv dataLen = %d\n", pRecvData->dataLen);
            STUDENT_HANDLE hStu;
            ret = to_host_stu(pRecvData->pBuf, &hStu);
            if (ERR_SUCCESS == ret)
            {
                print_stu(hStu);
                free(hStu);
            }
        }
        break;
    case CMD_QUERY_ALL:
        if (ERR_SUCCESS != errNo)
        {
            printf("list student failed, [errNo = %d]\n", errNo);
        }
        else
        {
            STUDENT_LIST_HANDLE hStuList;
            ret = to_host_stu_list(pRecvData->pBuf, &hStuList);
            if (ERR_SUCCESS == ret)
            {
                print_stu_list(hStuList);
                free(hStuList);
            }
        }
        break;
    default:
        break;
    }
    if (ERR_SUCCESS != ret)
    {
        printf("parse data failed, [errNo = %d]\n", ret);
    }
    g_isRecved = true;

    return ERR_SUCCESS;
}

static int inner_query(STRUCT_INT *pStuNo, SESSION_HANDLE hSession)
{
    int ret = ERR_SUCCESS;
    STUDENT_HEADER header;
    int dataLen = sizeof(STRUCT_INT);

    header.cmd = htons(CMD_QUERY_BY_STUNO);
    header.dataLen = htons(dataLen);
    ret = send_data(hSession, (char *)&header, sizeof(STUDENT_HEADER));
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    pStuNo->iValue = htons(pStuNo->iValue);
    return send_data(hSession, (char *)pStuNo, dataLen);
}

static int inner_query_all(SESSION_HANDLE hSession)
{
    STUDENT_HEADER header;

    header.cmd = htons(CMD_QUERY_ALL);
    header.dataLen = 0;
    return send_data(hSession, (char *)&header, sizeof(STUDENT_HEADER));
}

static void query_student(SESSION_HANDLE hSession)
{
    int ret = ERR_SUCCESS;
    STRUCT_INT stuNo;

    while(!g_isStop)
    {
        g_isRecved = false;
        printf("please input the student no(-1 is for query all):");
        scanf_s("%d", &stuNo.iValue, 1);
        if (-1 == stuNo.iValue)
        {
            ret = inner_query_all(hSession);
        }
        else
        {
            ret = inner_query(&stuNo, hSession);
        }
        if (ERR_SUCCESS != ret)
        {
            printf("disconnected....\n");
            break;
        }
        while (!g_isRecved)
        {
            Sleep(50);
        }
    }
}

int init_stu_client()
{
    int ret = ERR_SUCCESS;
    SOCK_MGR *pSockMgr = NULL;
    SOCK_HANDLE hSock = NULL;
    SESSION_HANDLE hSession = NULL;
    char ip[16] = {0};

    pSockMgr = (SOCK_MGR *) malloc(sizeof(SOCK_MGR));
    if (NULL == pSockMgr)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    ret = init_sock_mgr(pSockMgr);
    if (ERR_SUCCESS != ret)
    {
        free(pSockMgr);
        return ret;
    }
    ret = pSockMgr->new_tcp_sock(&hSock, NULL, -1);
    if (ERR_SUCCESS != ret)
    {
        free(pSockMgr);
        return ret;
    }
    do 
    {
        printf("please input service ip:");
        scanf_s("%s", &ip, 15);
        ret = pSockMgr->do_connect(hSock, ip, SOCK_LISTEN_PORT);
        if (ERR_SUCCESS != ret)
        {
            printf("connect %s failed, [errNo = %d]", ret);
            continue;
        }
        if (NULL == hSession)
        {
            ret = new_session(&hSession, pSockMgr, deal_recv_data);
            if (ERR_SUCCESS != ret)
            {
                break;
            }
        }
        start_session(hSession, hSock);
        query_student(hSession);
    } while (!g_isStop);
    if (NULL != hSock)
    {
        pSockMgr->delete_tcp_sock(hSock);
    }
    if (NULL != hSession)
    {
        delete_session(hSession);
    }
    release_sock_mgr();
    free(pSockMgr);
    
    return ret;
}

void release_stu_client()
{
    g_isStop = true;
}