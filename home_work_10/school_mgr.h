#ifndef _SCHOOL_MGR_H
#define _SCHOOL_MGR_H

#define SCHOOL_MAX_GRADES   6
#define MAX_NAME_LEN        260

typedef enum STRUCT_TYPE{
    STRUCT_SCHOOL = 0, STRUCT_GRADE, STRUCT_STUDENT
};

typedef struct _STRUCT_HEADER {
    STRUCT_TYPE struct_type;
    int         id;  
} STRUCT_HEADER;

typedef struct _STUDENT{
    STRUCT_HEADER header;
    int gradeId;
    int totalScore;
    TCHAR name[MAX_NAME_LEN];
}STUDENT;

typedef struct _GRADE{
    STRUCT_HEADER header;
    TCHAR majorTeacher[MAX_NAME_LEN];
}GRADE;

typedef struct _SCHOOL{
    STRUCT_HEADER header;
    TCHAR   name[MAX_NAME_LEN];
}SCHOOL;

typedef struct _SCHOOL_MGR {
    int (*init_school) (TCHAR *);
    int (*destroy_school) ();
    int (*get_school) (void **);
    int (*get_student_count) ();
    int (*get_grade_count) ();
    int (*add_grade) (int, TCHAR *);
    int (*remove_grade) (int);
    int (*update_grade) (int, TCHAR *);
    int (*add_student) (int, int, int, TCHAR *);
    int (*remove_student) (int, int);
    int (*update_student_score) (int, int, int);
    int (*update_student_grade) (int, int, int);
} SCHOOL_MGR; 

int new_school_mgr(SCHOOL_MGR **ppMgr);

int delete_school_mgr(SCHOOL_MGR *pMgr);

int generate_grade_id();

int generate_student_id();

#endif//_SCHOOL_MGR_H