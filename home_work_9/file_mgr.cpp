#include <tchar.h>
#include <Windows.h>
#include <malloc.h>
#include "file_mgr.h"
#include "cb_stack.h"
#include "err_no.h"

static inline int GetDirectory(TCHAR *pDir, TCHAR *pFileName, TCHAR **ppNewDir)
{
    size_t strLen= 0;
    TCHAR *pNewDir = NULL;

    strLen = _tcslen(pDir) + _tcslen(pFileName) + 2;//Ŀ¼���������

    pNewDir = (TCHAR *) malloc(sizeof(TCHAR) * (strLen));
    if (NULL == pNewDir)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    _stprintf(pNewDir, TEXT("%s%s\\"), pDir, pFileName);
    *ppNewDir = pNewDir;
    return ERR_SUCCESS;
}

static int ListDirectory(TCHAR *pParentDir, void *pStack, void *pListStack)
{
    int ret = ERR_SUCCESS;
    TCHAR *pDir = NULL;
    TCHAR path[MAX_PATH] = {0};
    TCHAR fileName[MAX_PATH];
    WIN32_FIND_DATA fileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    _tcscat(path, pParentDir);
    _tcscat(path, TEXT("\\*"));

    hFind = FindFirstFile(path, &fileData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return GetLastError();
    }

    while (_tcscmp(fileData.cFileName, TEXT(".")) == 0
        || _tcscmp(fileData.cFileName, TEXT("..")) == 0)
    {//���˵�������ļ���"."��".."ֱ����������һ��������Ч���ļ����ļ���
        ret = FindNextFile(hFind, &fileData);
        if (0 == ret)
        {//����ֵΪ0��ʾʧ��
            FindClose(hFind);
            ret = GetLastError();
            return ERROR_NO_MORE_FILES == ret ? ERR_SUCCESS : ret;
        }
    }
    //��ʼ��������
    do 
    {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {//�ļ���   
            ret = GetDirectory(pParentDir, fileData.cFileName, &pDir);
            if (ERR_SUCCESS != ret)
            {
                break;
            }
            cb_stack_push(pListStack, pDir);
            cb_stack_push(pStack, pDir);
        }
        else
        {
            memset(fileName, '\0', sizeof(TCHAR));//���ý�����
            _tcscat(fileName, pParentDir);
            _tcscat(fileName, fileData.cFileName);
            if (!DeleteFile(fileName))
            {
                ret = GetLastError();
                break;
            }
        }
        if (0 == FindNextFile(hFind, &fileData))
        {
            ret = GetLastError();
            if (ERROR_NO_MORE_FILES == ret)
            {
                ret = ERR_SUCCESS;
            }
            break;
        }
    } while (1);

    FindClose(hFind);
    return ret;
}

static void DeleteDirectoryRecursive(void *pStack, int *ret)
{
    TCHAR *pFilePath = NULL;

    pFilePath = (TCHAR *) cb_stack_pop(pStack);
    do 
    {
        if (ERR_SUCCESS == *ret)
        {
            if (!RemoveDirectory(pFilePath))
            {
                *ret = GetLastError();
            }
        }
        _tprintf(TEXT("RemoveDirectory %s\n"), pFilePath);
        free(pFilePath);
        pFilePath = (TCHAR *) cb_stack_pop(pStack);
    } while (NULL != pFilePath);
}

int DeleteDirectoryRecursive(TCHAR *pDir)
{
    int ret = ERR_SUCCESS;
    void *pStack = NULL;//�����ɾ�����ļ��е�ȫ��
    void *pListStack = NULL;//�����ļ��ĸ����ռ�

    ret = cb_create_stack(&pStack);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    ret = cb_create_stack(&pListStack);
    if (ERR_SUCCESS != ret)
    {
        cb_destroy_stack(pStack);
        return ret;
    }

    do 
    {
        ret = ListDirectory(pDir, pStack, pListStack);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        pDir = (TCHAR *) cb_stack_pop(pListStack);
    } while (NULL != pDir);
    
    DeleteDirectoryRecursive(pStack, &ret);

    cb_destroy_stack(pStack);
    cb_destroy_stack(pListStack);
    return ret;
}
