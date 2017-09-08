#ifndef _EDUCATION_H
#define _EDUCATION_H

typedef struct _STUDENT{
    int stuNo;
    int grade;
    int totalScore;
    char *name;
}STUDENT;

typedef struct _GRADE{
    int     grade;
    char    *majorTeacher;
    STUDENT **ppStus;
}GRADE;

typedef struct _SCHOOL{
    char    *name;
    GRADE   **pGrades;
}SCHOOL;

typedef enum HIERARCHY{
    HIER_SCHOOL = 0, HIER_GRADE, HIER_STUDENT;
};

#endif//_EDUCATION_H