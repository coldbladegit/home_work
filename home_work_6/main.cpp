#include <stdio.h>
#include "link_list.h"
#include "double_link_list.h"

int main(int argc, char **argv)
{
    STUDENT stu;
    STUDENT *pStu;

    printf("******开始录入******\n");
    Insert(111222333, "张三", 0);
    Insert(101369273, "李四", 0);
    Insert(270645954, "王二", 0);
    Insert(998775624, "赵大", 0);
    Insert(359864569, "钱六", 0);
    Insert(224684586, "孙七", 0);
    PrintIn();
    
    pStu = Remove(270645954, &stu);
    if (NULL != pStu)
    {
        printf("%s 信息被从列表中移除!\n", pStu->sName);
    }

    printf("******录入成绩******\n");
    SetGrade(111222333, 80);
    SetGrade(101369273, 85);
    SetGrade(998775624, 92);
    SetGrade(359864569, 88);
    SetGrade(224684586, 79);
    PrintIn();
    printf("******按成绩从高到低******\n");
    Sort();
    PrintIn();

    return 0;
}