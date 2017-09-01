#ifndef _STRUCT_H
#define _STRUCT_H

#define MAX_NAME_LEN    16

typedef struct _STUDENT {
    int     nId;
    int     nGrade;
    char    sName[MAX_NAME_LEN];
}STUDENT;

#endif//_STRUCT_H