#ifndef _SESSION_H
#define _SESSION_H

typedef void * SESSION_HANDLE;
typedef int (__cdecl *DEAL_RECV_FUNC) (void *);

int new_session(SESSION_HANDLE *pHSession, void *pSockMgr, DEAL_RECV_FUNC fun);

int delete_session(SESSION_HANDLE hSession);

int start_session(SESSION_HANDLE hSession, void *hSock);

int stop_session(SESSION_HANDLE hSession);

int send_data(SESSION_HANDLE hSession, char *pBuf, int dataLen);

bool is_idle(SESSION_HANDLE hSession);

bool is_stopped(SESSION_HANDLE hSession);

#endif//_SESSION_H