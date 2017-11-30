#ifndef _STUDENT_H
#define _STUDENT_H

#define MAX_NAME_LEN        64
#define MAX_HOME_LEN        128
#define BORTHDAY_LEN        12 //yyyy-MM-dd
#define SOCK_LISTEN_PORT    8888
#define SOCK_LISTEN_ADDR    "10.12.12.25"
/************************************************************************
                            student op cmd
************************************************************************/
#define CMD_QUERY_BY_STUNO   0x001
#define CMD_QUERY_ALL        0x002

typedef struct _STUDENT_HEADER {
    int cmd;
    int errNo;
    int dataLen;
} STUDENT_HEADER;

typedef struct _STRUCT_INT {
    int iValue;  
} STRUCT_INT;

typedef struct _RECV_DATA {
    int cmd;
    int errNo;
    int dataLen;
    void *hSession;
    char *pBuf;
} RECV_DATA;

typedef struct _STUDENT {
    int stuNo;
    int arge;
    int nameLen;
    int homeLen;
    char    borthday[BORTHDAY_LEN];
    wchar_t *name;
    wchar_t *home;
} STUDENT;

//专门用于发送的网络结构
typedef struct _STUDENT_NET {
    int stuNo;
    int arge;
    int nameLen;
    int homeLen;
    char    borthday[BORTHDAY_LEN];
} STUDENT_NET;

typedef struct _STUDENT_LIST {
    int stuCnt;
    STUDENT **pStus;
} STUDENT_LIST;

//专门用于发送的网络结构
typedef struct _STUDENT_LIST_NET {
    int stuCnt;
} STUDENT_LIST_NET;

#endif//_STUDENT_H