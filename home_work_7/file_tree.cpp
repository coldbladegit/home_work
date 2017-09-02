#include <Windows.h>
#include <tchar.h>
#include <malloc.h>
#include <wchar.h>
#include <strsafe.h>

#include "file_tree.h"
#include "err_no.h"

#define  ADD_STACK_SIZE_PER     100

typedef struct _BINARY_TREE_NODE {
    int                    hierarchy;//层级
    _BINARY_TREE_NODE      *pBrother;
    _BINARY_TREE_NODE      *pChild;
    WIN32_FIND_DATA        fileData;
    TCHAR                  dirPath[MAX_PATH];
}BINARY_TREE_NODE;

typedef struct _TREE_NODE_STACK{
    int size;
    int capacity;
    BINARY_TREE_NODE    **ppNodes;
}TREE_NODE_STACK;

static int CompareTreeNode(BINARY_TREE_NODE *pNode1, BINARY_TREE_NODE *pNode2)
{//先比较高位,再比较低位
    int highDiff = pNode1->fileData.ftLastWriteTime.dwHighDateTime - pNode2->fileData.ftLastWriteTime.dwHighDateTime;
    int lowDiff = pNode1->fileData.ftLastWriteTime.dwLowDateTime - pNode2->fileData.ftLastWriteTime.dwLowDateTime;

    if(0 != highDiff)
    {
        return highDiff;
    }
    return lowDiff;
}

static void SwapTreeNode(BINARY_TREE_NODE *pNode1, BINARY_TREE_NODE *pNode2)
{
    size_t size = sizeof(WIN32_FIND_DATA);
    WIN32_FIND_DATA fileData;
    BINARY_TREE_NODE *pTmp = pNode1->pChild;
    
    pNode1->pChild = pNode2->pChild;
    pNode2->pChild = pTmp;
    memcpy(&fileData, &pNode1->fileData, size);
    memcpy(&pNode1->fileData, &pNode2->fileData, size);
    memcpy(&pNode2->fileData, &fileData, size);
}

static TREE_NODE_STACK* CreateStack()
{
    TREE_NODE_STACK *pStack = (TREE_NODE_STACK*)malloc(sizeof(TREE_NODE_STACK));
    if (NULL == pStack)
    {
        return NULL;
    }
    pStack->ppNodes = (BINARY_TREE_NODE **)malloc(sizeof(BINARY_TREE_NODE *) * ADD_STACK_SIZE_PER);
    if (NULL == pStack->ppNodes)
    {
        free(pStack);
        return NULL;
    }
    pStack->capacity = ADD_STACK_SIZE_PER;
    pStack->size = 0;

    return pStack;
}

static void DestroyStack(TREE_NODE_STACK *pStack)
{
    if (NULL != pStack->ppNodes)
    {
        free(pStack->ppNodes);
    }
    free(pStack);
}

static int Push(TREE_NODE_STACK *pStack, BINARY_TREE_NODE *pNode)
{
    if (pStack->capacity <= pStack->size)
    {
        BINARY_TREE_NODE **ppNodes = 
            (BINARY_TREE_NODE **)malloc(sizeof(BINARY_TREE_NODE *) * (pStack->capacity + ADD_STACK_SIZE_PER));
        if (NULL == ppNodes)
        {
            return ERR_MALLOC_MEM_FAILD;
        }
        memcpy(ppNodes, pStack->ppNodes, pStack->size * sizeof(BINARY_TREE_NODE *));
        free(pStack->ppNodes);
        pStack->ppNodes = ppNodes;
        pStack->capacity += ADD_STACK_SIZE_PER;
    }

    pStack->ppNodes[pStack->size] = pNode;
    pStack->size++;
    
    return ERR_SUCCESS;
}

static BINARY_TREE_NODE* Pop(TREE_NODE_STACK *pStack)
{
    if (pStack->size == 0)
    {
        return NULL;
    }
    pStack->size--;
    return pStack->ppNodes[pStack->size];
}

static BINARY_TREE_NODE* CreateTreeNode(TCHAR *pDirPath, int hierarchy)
{
    size_t pathLen;
    BINARY_TREE_NODE *pNode = NULL;

    pNode = (BINARY_TREE_NODE *)malloc(sizeof(BINARY_TREE_NODE));
    if (NULL == pNode)
    {
        return NULL;
    }
    StringCbLength(pDirPath, MAX_PATH * sizeof(TCHAR), &pathLen);
    StringCbCopyN(pNode->dirPath, MAX_PATH * sizeof(TCHAR), pDirPath, pathLen + 1);
    pNode->pChild = NULL;
    pNode->pBrother = NULL;
    pNode->hierarchy = hierarchy;

    return pNode;
}

static int ListBrothers(BINARY_TREE_NODE *pBrother, HANDLE hFind, TREE_NODE_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    BINARY_TREE_NODE *pNode = NULL;
    WIN32_FIND_DATA fileData;

    do 
    {
        if (FindNextFile(hFind, &fileData) != 0)
        {
            pNode = CreateTreeNode(pBrother->dirPath, pBrother->hierarchy);
            if (NULL == pNode)
            {
                ret = ERR_MALLOC_MEM_FAILD;
                break;
            }
            memcpy(&pNode->fileData, &fileData, sizeof(WIN32_FIND_DATA));

            pBrother->pBrother = pNode;
            if ((pNode->fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {//是一个文件夹
                ret = Push(pStack, pNode);
                if (ERR_SUCCESS != ret)
                {
                    break;
                }
            }
            pBrother = pNode;
        }
        else
        {//如果文件夹下的文件已遍历完,返回成功,否则返回错误
            ret = ERROR_NO_MORE_FILES == GetLastError() ? ERR_SUCCESS : ERR_FAILED;
            break;
        }
    } while (1);
    
    return ret;
}

static int ListDirectoryFiles(BINARY_TREE_NODE *parent, TREE_NODE_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    size_t dirPathLen = 0;
    size_t fNameLen = 0;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    BINARY_TREE_NODE *pNode = NULL;
    TCHAR dir[MAX_PATH];

    StringCbLength(parent->dirPath, MAX_PATH * sizeof(TCHAR), &dirPathLen);
    if (0 != parent->hierarchy)
    {//hierarchy = 0表示根节点
        StringCbLength(parent->fileData.cFileName, MAX_PATH * sizeof(TCHAR), &fNameLen);
    }
    if (dirPathLen + fNameLen >= (MAX_PATH - 1) * sizeof(TCHAR))
    {//还要追加"\*"以及结束符
        return ERR_STR_TOO_LONG;
    }
    StringCbCopyN(dir, MAX_PATH * sizeof(TCHAR), parent->dirPath, dirPathLen + 1);
    if (0 != fNameLen)
    {//追加当前文件夹名
        StringCbCatN(dir, MAX_PATH * sizeof(TCHAR), parent->fileData.cFileName, fNameLen);
        StringCbCatN(dir, MAX_PATH * sizeof(TCHAR), TEXT("\\"), sizeof(TCHAR));//追加一个目录符
    }

    pNode = CreateTreeNode(dir, parent->hierarchy + 1);
    if (NULL == pNode)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    //目录名后追加"\*"
    StringCbCatN(dir, MAX_PATH * sizeof(TCHAR), TEXT("\\*"), 2 * sizeof(TCHAR));
    hFind = FindFirstFile(dir, &pNode->fileData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        free(pNode);
        return GetLastError();
    }
    
    while (_tcscmp(pNode->fileData.cFileName, TEXT(".")) == 0
        || _tcscmp(pNode->fileData.cFileName, TEXT("..")) == 0)
    {//过滤掉特殊的文件夹"."和".."直到遍历到第一个真正有效的文件或文件夹
        ret = FindNextFile(hFind, &pNode->fileData);
        if (0 == ret)
        {//返回值为0表示失败
            FindClose(hFind);
			ret = GetLastError();
            return ERROR_NO_MORE_FILES == ret ? ERR_SUCCESS : ret;
        }
    }
    parent->pChild = pNode;
    if ((pNode->fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {//是一个文件夹
        ret = Push(pStack, pNode);
        if (ERR_SUCCESS != ret)
        {
            return ret;
        }
    }    
    ret = ListBrothers(pNode, hFind, pStack);
    FindClose(hFind);
    return ret;
}

int ListDirectoryFiles(char *dirPath, void **ppFileTree)
{
    int ret = ERR_SUCCESS;
    TCHAR dir[MAX_PATH];
    TREE_NODE_STACK *pStack = NULL;
    BINARY_TREE_NODE *pNode = NULL;
    int dirLen = strlen(dirPath) + 1;

    if (dirLen > MAX_PATH * sizeof(TCHAR))
    {
        return ERR_STR_TOO_LONG;
    }
    if (0 == MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, dirPath, dirLen, dir, MAX_PATH))
    {
        return GetLastError();
    }

    do 
    {
        pStack = CreateStack();
        if (NULL == pStack)
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }

        pNode = CreateTreeNode(dir, 0);//根节点的hierarchy = 0
        if (NULL == pNode)
        {
            ret = ERR_MALLOC_MEM_FAILD;
            break;
        }
        *ppFileTree = pNode;

        while(NULL != pNode)
        {
            ret = ListDirectoryFiles(pNode, pStack);
            if (ERR_SUCCESS != ret)
            {
                break;
            }
            pNode = Pop(pStack);
        }
    } while (0);

    if (NULL != pStack)
    {
        DestroyStack(pStack);
    }
    return ret;
}

static int SortBrotherNodes(BINARY_TREE_NODE *pNode, TREE_NODE_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    BINARY_TREE_NODE *p, *q, *pTmp;

    for (p = pNode; NULL != p; p = p->pBrother)
    {
        pTmp = p;
        for(q = p->pBrother; NULL != q; q = q->pBrother)
        {
            if (CompareTreeNode(pTmp, q) > 0)
            {
                pTmp = q;
            }
        }
        if (pTmp != p)
        {
            SwapTreeNode(p, pTmp);
        }
        if (NULL != p->pChild)
        {
            ret = Push(pStack, p->pChild);
            if (ERR_SUCCESS != ret)
            {
                break;
            }  
        }
    }
    return ret;
}

int SortByModifyTime(void *pFileTree)
{
    int ret = ERR_SUCCESS;
    TREE_NODE_STACK *pStack = NULL;
    BINARY_TREE_NODE *pNode = NULL;

    if (NULL == pFileTree)
    {
        return ERR_SUCCESS;
    }

    pStack = CreateStack();
    if (NULL == pStack)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    pNode = ((BINARY_TREE_NODE *) pFileTree)->pChild;//获取根节点的子节点
    while(NULL != pNode)
    {
        ret = SortBrotherNodes(pNode, pStack);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        pNode = Pop(pStack);
    }

    DestroyStack(pStack);
    return ret;
}

static int PrintChildren(BINARY_TREE_NODE *pNode, TREE_NODE_STACK *pStack)
{
    int ret = ERR_SUCCESS;
    BINARY_TREE_NODE *pBrother = NULL;
    
    while (NULL != pNode)
    {
        if (pNode->hierarchy > 1)
        {
            _tprintf(TEXT("|%*s|\n"), (pNode->hierarchy - 1) * 2, TEXT(" "));
            _tprintf(TEXT("|%*s"), (pNode->hierarchy - 1) * 2, TEXT(" "));
        }
        else
        {
            _tprintf(TEXT("|\n"));
        }
        _tprintf(TEXT("|%s%s\n"), TEXT("--"), pNode->fileData.cFileName);
        /**将兄弟节点压栈**/
        pBrother = pNode->pBrother;
        if(NULL != pBrother)
        {
            ret = Push(pStack, pBrother);
            if (ERR_SUCCESS != ret)
            {
                break;
            }
        }
        pNode = pNode->pChild;
    }

    return ret;
}

int PrintFileTree(void *pFileTree)
{
    int ret = ERR_SUCCESS;
    TREE_NODE_STACK *pStack = NULL;
    BINARY_TREE_NODE *pNode = NULL;
	TCHAR rootName[5] = {0};

    if (NULL == pFileTree)
    {
        return ERR_SUCCESS;
    }

    pStack = CreateStack();
    if (NULL == pStack)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    pNode = ((BINARY_TREE_NODE *) pFileTree)->pChild;//获取根节点的子节点
	if (NULL == pNode)
	{//没有子节点,也就是说该目录是一个空目录
		return ERR_SUCCESS;
	}
    StringCbCopyN(rootName, 5 * sizeof(TCHAR), pNode->dirPath, 2 * sizeof(TCHAR));
    StringCbCatN(rootName, 5 * sizeof(TCHAR), TEXT(".\n"), 3 * sizeof(TCHAR));
    _tprintf(TEXT("%s"), rootName);

    while(NULL != pNode)
    {
        ret = PrintChildren(pNode, pStack);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        pNode = Pop(pStack);
    }

    DestroyStack(pStack);
    return ret;
}

int FreeFileTree(void *pFileTree)
{
    int ret = ERR_SUCCESS;
    TREE_NODE_STACK  *pStack = NULL;
    BINARY_TREE_NODE *pNode = NULL;
    BINARY_TREE_NODE *pTmp = NULL;
    
    if (NULL == pFileTree)
    {
        return ERR_SUCCESS;
    }

    pStack = CreateStack();
    if (NULL == pStack)
    {
        return ERR_MALLOC_MEM_FAILD;
    }

    pNode = (BINARY_TREE_NODE *) pFileTree;
    do 
    {//广度优先遍历
        if (NULL != pNode->pBrother)
        {
            ret = Push(pStack, pNode);
            pNode = pNode->pBrother;
        }
        else if (NULL != pNode->pChild)
        {
            ret = Push(pStack, pNode);
            pNode = pNode->pChild;
        }
        else
        {
            pTmp = pNode;
            pNode = Pop(pStack);
            free(pTmp);
            if (NULL != pNode)
            {
                if (pNode->pBrother == pTmp)
                {
                    pNode->pBrother = NULL;
                }
                else
                {
                    pNode->pChild = NULL;
                }
            }
            else
            {
                break;
            }
        }
    } while (ERR_SUCCESS == ret);
    
    DestroyStack(pStack);
    return ret;
}