#include <stdio.h>
#include "link_list.h"
#include "double_link_list.h"

int main(int argc, char **argv)
{
    STUDENT stu;
    STUDENT *pStu;

    printf("******��ʼ¼��******\n");
    Insert(111222333, "����", 0);
    Insert(101369273, "����", 0);
    Insert(270645954, "����", 0);
    Insert(998775624, "�Դ�", 0);
    Insert(359864569, "Ǯ��", 0);
    Insert(224684586, "����", 0);
    PrintIn();
    
    pStu = Remove(270645954, &stu);
    if (NULL != pStu)
    {
        printf("%s ��Ϣ�����б����Ƴ�!\n", pStu->sName);
    }

    printf("******¼��ɼ�******\n");
    SetGrade(111222333, 80);
    SetGrade(101369273, 85);
    SetGrade(998775624, 92);
    SetGrade(359864569, 88);
    SetGrade(224684586, 79);
    PrintIn();
    printf("******���ɼ��Ӹߵ���******\n");
    Sort();
    PrintIn();

    return 0;
}