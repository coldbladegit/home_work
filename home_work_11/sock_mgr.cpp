#include <Winsock2.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include "sock_mgr.h"
#include "err_no.h"

#define GET_SOCK_ERRNO()  WSAGetLastError()
#define SOCK_ADDR_SIZE    sizeof(sockaddr_in)

typedef struct _SOCKET_INFO {
    SOCKET hSocket;
    int recvTimeout;
    int sendTimeout;
    sockaddr_in localAddr;
    sockaddr_in remoteAddr;
} SOCKET_INFO;

static int init_sock()
{
    WSAData wsaData;

    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        return GET_SOCK_ERRNO();
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return ERR_INIT_SOCKET_FAILED;
    }
    
    return ERR_SUCCESS;
}

static int new_tcp_sock(SOCK_HANDLE *pHSock, const char *ip, int port)
{
    SOCKET_INFO *pSockInfo = NULL;
    
    pSockInfo = (SOCKET_INFO *) malloc(sizeof(SOCKET_INFO));
    if (NULL == pSockInfo)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    memset(pSockInfo, 0, sizeof(SOCKET_INFO));

    pSockInfo->hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == pSockInfo->hSocket)
    {
        free(pSockInfo);
        return GET_SOCK_ERRNO();
    }
    if (NULL != ip)
    {
        pSockInfo->localAddr.sin_family = AF_INET;
        pSockInfo->localAddr.sin_addr.s_addr = inet_addr(ip);
        pSockInfo->localAddr.sin_port = htons(port);
    }

    *pHSock = pSockInfo;
 
    return ERR_SUCCESS;
}

static void delete_tcp_sock(SOCK_HANDLE hSock)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);
    
    pSockInfo = (SOCKET_INFO *) hSock;
    if (INVALID_SOCKET != pSockInfo->hSocket)
    {
        closesocket(pSockInfo->hSocket);
    }
    free(pSockInfo);
}

static int do_bind(SOCK_HANDLE hSock)
{
    SOCKET_INFO *pSockInfo = NULL;
    
    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    if (SOCKET_ERROR == bind(pSockInfo->hSocket, (SOCKADDR*) &pSockInfo->localAddr, SOCK_ADDR_SIZE))
    {
        return GET_SOCK_ERRNO();
    }

    return ERR_SUCCESS;
}

static int do_listen(SOCK_HANDLE hSock, int maxCnt)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    if (SOCKET_ERROR == listen(pSockInfo->hSocket, maxCnt))
    {
        return GET_SOCK_ERRNO();
    }

    return ERR_SUCCESS;
}

static int do_accept(SOCK_HANDLE *pHSock, SOCK_HANDLE hSock)
{
    SOCKET_INFO *pNewSockInfo = NULL;
    SOCKET_INFO *pSockInfo = NULL;
    int addrLen = SOCK_ADDR_SIZE;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;

    pNewSockInfo = (SOCKET_INFO *) malloc(sizeof(SOCKET_INFO));
    if (NULL == pNewSockInfo)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    memcpy(pNewSockInfo, pSockInfo, sizeof(SOCKET_INFO));
    
    pNewSockInfo->hSocket = accept(pSockInfo->hSocket, (SOCKADDR*)&pNewSockInfo->remoteAddr, &addrLen);
    if (INVALID_SOCKET == pNewSockInfo->hSocket)
    {
        free(pNewSockInfo);
        return GET_SOCK_ERRNO();
    }
    *pHSock = pNewSockInfo;

    return ERR_SUCCESS;
}

static int do_connect(SOCK_HANDLE hSock, const char *ip, int port)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);
    
    pSockInfo = (SOCKET_INFO *)hSock;
    pSockInfo->remoteAddr.sin_family = AF_INET;
    pSockInfo->remoteAddr.sin_addr.s_addr = inet_addr(ip);
    pSockInfo->remoteAddr.sin_port = htons(port);
    if (SOCKET_ERROR == connect(pSockInfo->hSocket, (SOCKADDR*)&pSockInfo->remoteAddr, SOCK_ADDR_SIZE))
    {
        return GET_SOCK_ERRNO();
    }

    return ERR_SUCCESS;
}

static int do_recv(SOCK_HANDLE hSock, char *pBuf, int len) 
{
    int readLen = 0;
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    do 
    {
        readLen = recv(pSockInfo->hSocket, pBuf, len, 0);
        if (SOCKET_ERROR == readLen)
        {
            return GET_SOCK_ERRNO();
        }
        pBuf += readLen;
        len -= readLen;
    } while (len > 0);

    return ERR_SUCCESS;
}

static int do_send(SOCK_HANDLE hSock, char *pBuf, int dataLen)
{
    int sendLen = 0;
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    do 
    {
        sendLen = send(pSockInfo->hSocket, pBuf, dataLen, 0);
        if (SOCKET_ERROR == sendLen)
        {
            return GET_SOCK_ERRNO();
        }
        dataLen -= sendLen;
        pBuf += sendLen;
    } while (dataLen > 0);

    return ERR_SUCCESS;
}

static void set_recv_send_timeout(SOCK_HANDLE hSock, int recvTimeout, int sendTimeout)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    pSockInfo->recvTimeout = recvTimeout;
    pSockInfo->sendTimeout = sendTimeout;
}

static int get_local_addr(SOCK_HANDLE hSock, int *ip, int *port)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    *ip = pSockInfo->localAddr.sin_addr.s_addr;
    *port = ntohs(pSockInfo->localAddr.sin_port);

    return ERR_SUCCESS;
}

static int get_remote_addr(SOCK_HANDLE hSock, int *ip, int *port)
{
    SOCKET_INFO *pSockInfo = NULL;

    G_ASSERT(NULL != hSock);

    pSockInfo = (SOCKET_INFO *)hSock;
    *ip = pSockInfo->remoteAddr.sin_addr.s_addr;
    *port = ntohs(pSockInfo->remoteAddr.sin_port);

    return ERR_SUCCESS;
}

int init_sock_mgr(SOCK_MGR *pMgr)
{
    int ret = ERR_SUCCESS;
    G_ASSERT(NULL != pMgr);

    ret = init_sock();
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    pMgr->new_tcp_sock = new_tcp_sock;
    pMgr->delete_tcp_sock = delete_tcp_sock;
    pMgr->do_bind = do_bind;
    pMgr->do_listen = do_listen;
    pMgr->do_accept = do_accept;
    pMgr->do_connect = do_connect;
    pMgr->do_recv = do_recv;
    pMgr->do_send = do_send;
    pMgr->set_recv_send_timeout = set_recv_send_timeout;
    pMgr->get_local_addr = get_local_addr;
    pMgr->get_remote_addr = get_remote_addr;

    return ERR_SUCCESS;
}

void release_sock_mgr()
{
    WSACleanup();
}