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

    strLen = _tcslen(pDir) + _tcslen(pFileName) + 2;//目录符与结束符

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
    {//过滤掉特殊的文件夹"."和".."直到遍历到第一个真正有效的文件或文件夹
        ret = FindNextFile(hFind, &fileData);
        if (0 == ret)
        {//返回值为0表示失败
            FindClose(hFind);
            ret = GetLastError();
            return ERROR_NO_MORE_FILES == ret ? ERR_SUCCESS : ret;
        }
    }
    //开始真正遍历
    do 
    {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {//文件夹   
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
            memset(fileName, '\0', sizeof(TCHAR));//设置结束符
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
    void *pStack = NULL;//缓存待删除的文件夹的全名
    void *pListStack = NULL;//遍历文件的辅助空间

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
