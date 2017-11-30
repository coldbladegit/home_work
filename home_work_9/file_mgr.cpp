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

static inline int GetSubDirectory(TCHAR *pDir, TCHAR *pFileName, CB_STACK *pStack)
{
    size_t strLen= 0;
    TCHAR *pSubDir = NULL;

    strLen = _tcslen(pDir) + _tcslen(pFileName) + 2;//Ŀ¼���������
    pSubDir = (TCHAR *) malloc(sizeof(TCHAR) * (strLen));
    if (NULL == pSubDir)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    _stprintf(pSubDir, TEXT("%s%s\\"), pDir, pFileName);

    return pStack->push(pStack, pSubDir);
}

static inline int DeleteCurDirectory(CB_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    TCHAR *pTopElem = NULL;

    pTopElem = (TCHAR *)pStack->pop(pStack);
    if (NULL != pTopElem)
    {
        ret = RemoveDirectory(pTopElem) ? ERR_SUCCESS : GetLastError();
        free(pTopElem);
    }
    return ret;
}

static int DoDelete(CB_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    TCHAR *pCurDir = (TCHAR *)pStack->top(pStack);//ֻ�Ƿ���ջ��Ԫ��,������ջ
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
    {//���˵�������ļ���"."��".."ֱ����������һ��������Ч���ļ����ļ���
        if (!FindNextFile(hFind, &fileData))
        {
            ret = GetLastError();
            break;
        }
    }
    //��ʼ��������
    while(ERR_SUCCESS == ret)
    {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {//�ļ���   
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
    {//��ǰĿ¼�ѱ�����
        if (pCurDir == pStack->top(pStack))
        {//��ǰĿ¼Ϊ���ļ��л򲻰������ļ���(û���κ�ѹջ����)
            DeleteCurDirectory(pStack);
        }
        ret = ERR_SUCCESS;
    }

    return ret;
}

static void FreeStackReserved(CB_STACK *pStack)
{
    void *pTop = NULL;

    pTop = pStack->pop(pStack);
    while(NULL != pTop)
    {
        free(pTop);
        pTop = pStack->pop(pStack);
    }
}

int DeleteDirectory(TCHAR *pDir)
{
    int ret = ERR_SUCCESS;
    CB_STACK *pStack = NULL;//�����ɾ�����ļ���
    TCHAR *pCloneDir = NULL;//��ǰ���ڱ�����Ŀ¼

    ret = cb_new_stack(&pStack);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    ret = CloneDirectory(pDir, &pCloneDir);
    if (ERR_SUCCESS != ret)
    {
        cb_delete_stack(pStack);
        return ret;
    }
    pStack->push(pStack, pCloneDir);
    
    do 
    {
        ret = DoDelete(pStack);
    } while (!pStack->is_empty(pStack) && ERR_SUCCESS == ret);

    FreeStackReserved(pStack);
    cb_delete_stack(pStack);

    return ret;
}

static inline int PushHandleAndOffset(CB_STACK *pStack, HANDLE hFind, int offset)
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

    return pStack->push(pStack, pData);
}

static inline bool PopHandleAndOffset(CB_STACK *pStack, HANDLE *hFind, int *offset)
{
    HANDLE_FLAG *pTop = NULL;

    pTop = (HANDLE_FLAG *) pStack->pop(pStack);
    while(NULL != pTop)
    {
        *hFind = pTop->hFind;
        *offset = pTop->offset;
        return true;
    }
    return false;
}

static void FreeStackReservedEx(CB_STACK *pStack)
{
    HANDLE_FLAG *pTop = NULL;

    pTop = (HANDLE_FLAG *) pStack->pop(pStack);
    while(NULL != pTop)
    {
        FindClose(pTop->hFind);
        free(pTop);
        pTop = (HANDLE_FLAG *)pStack->pop(pStack);
    }
}

int DeleteDirectoryEx(TCHAR *pDir)
{
    int ret = ERR_SUCCESS;
    int offset = 0;
    TCHAR *pBuf = NULL;//һƬ�󻺳�
    CB_STACK *pStack = NULL;
    WIN32_FIND_DATA fileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    if (NULL == pDir)
    {
        return ERR_INVALID_PARAM;
    }

    ret = cb_new_stack(&pStack);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }

    pBuf = (TCHAR *) malloc(sizeof(TCHAR) * MAX_DIR_PATH);
    if (NULL == pBuf)
    {
        cb_delete_stack(pStack);
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
            {//���˵�������ļ���"."��".."
                if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {//�ļ�����ʼ�������ӽڵ�
                    ret = PushHandleAndOffset(pStack, hFind, offset);
                    _stprintf(pBuf + offset, TEXT("%s\\\\*"), fileData.cFileName);
                    offset += _tcslen(fileData.cFileName) + 1;//׷����һ���ļ��ָ���
                    break;
                }
                else
                {//���ļ���ɾ������
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
        {//��ǰ�ǿ��ļ��л򲻰������ļ���
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
    cb_delete_stack(pStack);
    free(pBuf);

    return ret;
}
