#ifndef _SOCK_MGR_H
#define _SOCK_MGR_H

typedef void * SOCK_HANDLE;

typedef struct _SOCK_MGR {
    int (*new_tcp_sock) (SOCK_HANDLE *, const char *, int);
    void (*delete_tcp_sock) (SOCK_HANDLE);
    int (*do_bind) (SOCK_HANDLE);
    int (*do_listen) (SOCK_HANDLE, int);
    int (*do_accept) (SOCK_HANDLE *, SOCK_HANDLE);
    int (*do_connect) (SOCK_HANDLE, const char *, int);
    int (*do_recv) (SOCK_HANDLE, char *, int);
    int (*do_send) (SOCK_HANDLE, char *, int);
    void (*set_recv_send_timeout) (SOCK_HANDLE, int, int);
    int (*get_local_addr) (SOCK_HANDLE, int *, int *);
    int (*get_remote_addr) (SOCK_HANDLE, int *, int *);
} SOCK_MGR;

int init_sock_mgr(SOCK_MGR *pMgr);

void release_sock_mgr();

#endif//_SOCK_MGR_H