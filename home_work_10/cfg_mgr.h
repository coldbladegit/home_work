#ifndef _CFG_MGR_H
#define _CFG_MGR_H

int ReadConfig();

int WriteConfig();

int SaveSchool(void *pData);

int SaveGrade(void *pData);

int InsertStudent(void *pData);

void *SearchStudent(int stuNo);

void *RemoveStudent(int stuNo);

int UpdateStudentGrade(int stuNo, int grade);

int UpdateStudentScore(int stuNo, int score);

#endif//_CFG_MGR_H