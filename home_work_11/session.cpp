#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include "sock_mgr.h"
#include "student.h"
#include "session.h"
#include "err_no.h"
#include "cb_lock.h"

typedef enum SESSION_STATE {
    NOT_START = 0, IDLE, RUNNING, SHUTDOWN, STOPPED
};

typedef struct _SESSION {
    SOCK_HANDLE hSock;
    SOCK_MGR *pSockMgr;
    HANDLE hRecvThread;
    SESSION_STATE state;
    LOCK_HANDLE stateLock;
    DEAL_RECV_FUNC deal_recv_data;
} SESSION;

static void update_session_state(SESSION *pSession, SESSION_STATE state)
{
    cb_lock(pSession->stateLock);
    pSession->state = state;
    cb_unlock(pSession->stateLock);
}

static SESSION_STATE get_session_state(SESSION *pSession)
{
    SESSION_STATE state;

    cb_lock(pSession->stateLock);
    state = pSession->state;
    cb_unlock(pSession->stateLock);

    return state;
}

static DWORD WINAPI recv_data(void *pParam)
{
    SESSION *pSession = (SESSION *)pParam;
    int ret = ERR_SUCCESS;
    int headerLen = sizeof(STUDENT_HEADER);
    char dataBuf[1024];
    char *pBuf = NULL;
    RECV_DATA recvData;
    STUDENT_HEADER *pHeader = NULL;
    
    recvData.hSession = pSession;
    while (SHUTDOWN != get_session_state(pSession))
    {
        do 
        {//先读头部
            recvData.pBuf = NULL;
            ret = pSession->pSockMgr->do_recv(pSession->hSock, dataBuf, headerLen);
            if (ERR_SUCCESS != ret)
            {
                break;
            }
            pHeader = (STUDENT_HEADER *) dataBuf;
            recvData.cmd = ntohs(pHeader->cmd);
            recvData.errNo = ntohs(pHeader->errNo);
            recvData.dataLen = ntohs(pHeader->dataLen);
            if (recvData.dataLen <= 0)
            {
                break;
            }
            else if (recvData.dataLen > 1024)
            {
                recvData.pBuf = (char *) malloc(recvData.dataLen);
                if (NULL == recvData.pBuf)
                {
                    ret = ERR_MALLOC_MEM_FAILD;
                    break;
                }
                pBuf = recvData.pBuf;
            }
            else
            {
                recvData.pBuf = dataBuf;
            }
            ret = pSession->pSockMgr->do_recv(pSession->hSock, recvData.pBuf, recvData.dataLen);
        } while (0);
        if (ERR_SUCCESS != ret)
        {//线程重用,这里将状态置为空闲并等待下一个连接
            if (ERR_MALLOC_MEM_FAILD != ret)
            {
                printf("socket error, [errNo = %d]\n", ret);
            }
            if (NULL != pBuf)
            {
                free(pBuf);
            }
            update_session_state(pSession, IDLE);
            while (IDLE == get_session_state(pSession))
            {
                Sleep(200);
            }
        }
        else
        {//交给外部去处理
            pSession->deal_recv_data(&recvData);
            if (NULL != pBuf)
            {
                free(pBuf);
            }
        }
    }
    update_session_state(pSession, STOPPED);
    return ERR_SUCCESS;
}

int new_session(SESSION_HANDLE *pHSession, void *pSockMgr, DEAL_RECV_FUNC fun)
{
    SESSION *pSession = NULL;

    G_ASSERT(NULL != pSockMgr);

    pSession = (SESSION *) malloc(sizeof(SESSION));
    if (NULL == pSession)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pSession->hSock = NULL;
    pSession->pSockMgr = (SOCK_MGR *) pSockMgr;
    pSession->deal_recv_data = fun;
    pSession->hRecvThread = NULL;
    pSession->state = NOT_START;
    pSession->stateLock = cb_createLock();
    *pHSession = pSession;

    return ERR_SUCCESS;
}

int delete_session(SESSION_HANDLE hSession)
{
    SESSION *pSession = NULL;

    G_ASSERT(NULL != hSession);

    pSession = (SESSION *) hSession;
    if (!is_stopped(hSession))
    {
        return ERR_SESSION_IS_WORKING;
    }
    cb_destroyLock(pSession->stateLock);
    free(hSession);

    return ERR_SUCCESS;
}

int start_session(SESSION_HANDLE hSession, void *hSock)
{
    SESSION *pSession = NULL;
    DWORD threadId = 0;

    G_ASSERT(NULL != hSession && NULL != hSock);

    pSession = (SESSION *) hSession;
    if (NULL != pSession->hSock)
    {
        pSession->pSockMgr->delete_tcp_sock(pSession->hSock);
    }
    pSession->hSock = (SOCK_HANDLE) hSock;
    if (NULL == pSession->hRecvThread)
    {
        pSession->hRecvThread = CreateThread(NULL, 0, recv_data, pSession, 0, &threadId);
        if (NULL == pSession->hRecvThread)
        {
            return GetLastError();
        }
    }
    update_session_state(pSession, RUNNING);

    return ERR_SUCCESS;
}

int stop_session(SESSION_HANDLE hSession)
{
    SESSION *pSession = NULL;

    G_ASSERT(NULL != hSession);

    pSession = (SESSION *) hSession;
    if (NOT_START == get_session_state(pSession))
    {
        return ERR_SESSION_NOT_START;
    }
    update_session_state(pSession, SHUTDOWN);
    pSession->pSockMgr->delete_tcp_sock(pSession->hSock);
    pSession->hSock = NULL;
    while(!is_stopped(hSession))
    {
        Sleep(500);
    }
    CloseHandle(pSession->hRecvThread);

    return ERR_SUCCESS;
}

int send_data(SESSION_HANDLE hSession, char *pBuf, int dataLen)
{
    SESSION *pSession = NULL;

    G_ASSERT(NULL != hSession);

    pSession = (SESSION *) hSession;
    if (NOT_START == get_session_state(pSession))
    {
        return ERR_SESSION_NOT_START;
    }
    return pSession->pSockMgr->do_send(pSession->hSock, pBuf, dataLen);
}

bool is_idle(SESSION_HANDLE hSession)
{
    SESSION *pSession = NULL;
    bool isIdle = false;

    G_ASSERT(NULL != hSession);

    pSession = (SESSION *) hSession;
    cb_lock(pSession->stateLock);
    isIdle = IDLE == pSession->state;
    cb_unlock(pSession->stateLock);

    return isIdle;
}

bool is_stopped(SESSION_HANDLE hSession)
{
    SESSION *pSession = NULL;
    bool isStopped = false;

    G_ASSERT(NULL != hSession);

    pSession = (SESSION *) hSession;
    cb_lock(pSession->stateLock);
    isStopped = STOPPED == pSession->state;
    cb_unlock(pSession->stateLock);

    return isStopped;
}