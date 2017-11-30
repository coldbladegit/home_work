#ifndef _CB_UTIL_H
#define _CB_UTIL_H

typedef void * STUDENT_HANDLE;
typedef void * STUDENT_NET_HANDLE;
typedef void * STUDENT_LIST_HANDLE;
typedef void * STUDENT_LIST_NET_HANDLE;

int to_host_stu(STUDENT_NET_HANDLE hStuNet, STUDENT_HANDLE *pHStu);

int to_net_stu(STUDENT_HANDLE hStu, STUDENT_NET_HANDLE *pHStuNet, int *dataLen);

int to_host_stu_list(STUDENT_LIST_NET_HANDLE hStuListNet, STUDENT_LIST_HANDLE *pHStuList);

int vec_to_net_stu_list(void *p, STUDENT_LIST_NET_HANDLE *pHStuList, int *dataLen);

void print_stu(STUDENT_HANDLE hStu);

void print_stu_list(STUDENT_LIST_HANDLE hStuList);

#endif//_CB_UTIL_H