#include <tchar.h>
#include <Windows.h>
#include <malloc.h>
#include "file_mgr.h"
#include "cb_stack.h"
#include "err_no.h"

#define  MAX_DIR_PATH   1024

typedef struct _HANDLE_FLAG {
    HANDLE hFind;
    int    offset;
}HANDLE_FLAG;

static int CloneDirectory(TCHAR *pDir, TCHAR **ppNewDir)
{
    TCHAR *pNewDir = NULL;

    pNewDir = (TCHAR *) malloc(sizeof(TCHAR) * (_tcslen(pDir) + 1));
    if (NULL == pNewDir)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    _stprintf(pNewDir, TEXT("%s"), pDir);
    *ppNewDir = pNewDir;
    return ERR_SUCCESS;
}

static inline int GetSubDirectory(TCHAR *pDir, TCHAR *pFileName, void *pStack)
{
    size_t strLen= 0;
    TCHAR *pSubDir = NULL;

    strLen = _tcslen(pDir) + _tcslen(pFileName) + 2;//目录符与结束符
    pSubDir = (TCHAR *) malloc(sizeof(TCHAR) * (strLen));
    if (NULL == pSubDir)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    _stprintf(pSubDir, TEXT("%s%s\\"), pDir, pFileName);

    return cb_stack_push(pStack, pSubDir);
}

static inline int DeleteCurDirectory(void *pStack)
{
    int ret = ERR_SUCCESS;
    TCHAR *pTopElem = NULL;

    pTopElem = (TCHAR *)cb_stack_pop(pStack);
    if (NULL != pTopElem)
    {
        ret = RemoveDirectory(pTopElem) ? ERR_SUCCESS : GetLastError();
        free(pTopElem);
    }
    return ret;
}

static int DoDelete(void *pStack)
{
    int ret = ERR_SUCCESS;
    TCHAR *pCurDir = (TCHAR *)cb_stack_top(pStack);//只是访问栈顶元素,并不出栈
    TCHAR tmpBuf[MAX_PATH];
    WIN32_FIND_DATA fileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    _stprintf(tmpBuf, TEXT("%s\\*"), pCurDir);
    hFind = FindFirstFile(tmpBuf, &fileData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return GetLastError();
    }

    while (_tcscmp(fileData.cFileName, TEXT(".")) == 0
        || _tcscmp(fileData.cFileName, TEXT("..")) == 0)
    {//过滤掉特殊的文件夹"."和".."直到遍历到第一个真正有效的文件或文件夹
        if (!FindNextFile(hFind, &fileData))
        {
            ret = GetLastError();
            break;
        }
    }
    //开始真正遍历
    while(ERR_SUCCESS == ret)
    {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {//文件夹   
            ret = GetSubDirectory(pCurDir, fileData.cFileName, pStack);
        }
        else
        {
            _stprintf(tmpBuf, TEXT("%s%s"), pCurDir, fileData.cFileName);
            if (!DeleteFile(tmpBuf))
            {
                ret = GetLastError();
                break;
            }
        }
        if (!FindNextFile(hFind, &fileData))
        {
            ret = GetLastError();
        }
    }
    FindClose(hFind);

    if (ERROR_NO_MORE_FILES == ret)
    {//当前目录已遍历完
        if (pCurDir == cb_stack_top(pStack))
        {//当前目录为空文件夹或不包含子文件夹(没有任何压栈动作)
            DeleteCurDirectory(pStack);
        }
        ret = ERR_SUCCESS;
    }

    return ret;
}

static void FreeStackReserved(void *pStack)
{
    void *pTop = NULL;

    pTop = cb_stack_pop(pStack);
    while(NULL != pTop)
    {
        free(pTop);
        pTop = cb_stack_pop(pStack);
    }
}

int DeleteDirectory(TCHAR *pDir)
{
    int ret = ERR_SUCCESS;
    void *pStack = NULL;//缓存待删除的文件夹
    TCHAR *pCloneDir = NULL;//当前正在遍历的目录

    ret = cb_create_stack(&pStack);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    ret = CloneDirectory(pDir, &pCloneDir);
    if (ERR_SUCCESS != ret)
    {
        cb_destroy_stack(pStack);
        return ret;
    }
    cb_stack_push(pStack, pCloneDir);
    
    do 
    {
        ret = DoDelete(pStack);
    } while (!cb_is_stack_empty(pStack) && ERR_SUCCESS == ret);

    FreeStackReserved(pStack);
    cb_destroy_stack(pStack);
    return ret;
}

static inline int PushHandleAndOffset(void *pStack, HANDLE hFind, int offset)
{
    int ret = ERR_SUCCESS;
    HANDLE_FLAG *pData = NULL;

    pData = (HANDLE_FLAG *) malloc(sizeof(HANDLE_FLAG));
    if (NULL == pData)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pData->hFind = hFind;
    pData->offset = offset;

    return cb_stack_push(pStack, pData);
}

static inline bool PopHandleAndOffset(void *pStack, HANDLE *hFind, int *offset)
{
    HANDLE_FLAG *pTop = NULL;

    pTop = (HANDLE_FLAG *) cb_stack_pop(pStack);
    while(NULL != pTop)
    {
        *hFind = pTop->hFind;
        *offset = pTop->offset;
        return true;
    }
    return false;
}

static void FreeStackReservedEx(void *pStack)
{
    void *pTop = NULL;

    pTop = cb_stack_pop(pStack);
    while(NULL != pTop)
    {
        free(pTop);
        pTop = cb_stack_pop(pStack);
    }
}

int DeleteDirectoryEx(TCHAR *pDir)
{
    int ret = ERR_SUCCESS;
    int offset = 0;
    TCHAR *pBuf = NULL;//一片大缓冲
    void *pStack = NULL;
    WIN32_FIND_DATA fileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    if (NULL == pDir)
    {
        return ERR_INVALID_PARAM;
    }

    ret = cb_create_stack(&pStack);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    pBuf = (TCHAR *) malloc(sizeof(TCHAR) * MAX_DIR_PATH);
    if (NULL == pBuf)
    {
        cb_destroy_stack(pStack);
        return ERR_MALLOC_MEM_FAILD;
    }

    _stprintf(pBuf, TEXT("%s\\*"), pDir);
    offset = _tcslen(pDir);

    while (ERR_SUCCESS == ret)
    {
        if (INVALID_HANDLE_VALUE == hFind)
        {
            hFind = FindFirstFile(pBuf, &fileData);
            if (INVALID_HANDLE_VALUE == hFind)
            {
                ret = GetLastError();
                break;
            }
        }
        else
        {
            if (!FindNextFile(hFind, &fileData))
            {
                ret = GetLastError();
            }
        }
        
        while (ERR_SUCCESS == ret)
        {
            if (_tcscmp(fileData.cFileName, TEXT(".")) != 0
                && _tcscmp(fileData.cFileName, TEXT("..")) != 0)
            {//过滤掉特殊的文件夹"."和".."
                if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {//文件夹则开始遍历其子节点
                    ret = PushHandleAndOffset(pStack, hFind, offset);
                    _stprintf(pBuf + offset, TEXT("%s\\\\*"), fileData.cFileName);
                    offset += _tcslen(fileData.cFileName) + 1;//追加了一个文件分隔符
                    break;
                }
                else
                {//对文件做删除处理
                    _stprintf(pBuf + offset, TEXT("%s"), fileData.cFileName);
                    if (!DeleteFile(pBuf))
                    {
                        ret = GetLastError();
                        break;
                    }
                }
            }
            if (!FindNextFile(hFind, &fileData))
            {
                ret = GetLastError();
            }
        }
        
        if (ERROR_NO_MORE_FILES == ret)
        {//当前是空文件夹或不包含子文件夹
            FindClose(hFind);
            _stprintf(pBuf + offset, TEXT(""));
            ret = RemoveDirectory(pBuf) ? ERR_SUCCESS : GetLastError();
            if (!PopHandleAndOffset(pStack, &hFind, &offset))
            {
                break;
            }
        }
        else
        {
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    FreeStackReservedEx(pStack);
    cb_destroy_stack(pStack);
    free(pBuf);
    return ret;
}
