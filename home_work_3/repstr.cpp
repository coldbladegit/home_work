#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "repstr.h"
#include "err_no.h"

#define MAX_BUFF_SIZE 1024

typedef struct _FILE_READER_BUFFER{
    int         reserved;//一次解析后剩余的字串
    int         strLen;//当前缓冲中整个字串的长度
    int         offset;//偏移
    int         wordLen;
    FILE        *pReader;
    const char  *pSrcWord;
    char        *pBuf;
}FILE_READER_BUFFER;

typedef struct _FILE_WRITER_BUFFER{
    int         offset;
    int         wordLen;
    FILE        *pWriter;
    const char  *pDestWord;
    char        pBuf[MAX_BUFF_SIZE];
}FILE_WRITER_BUFFER;

static int Initialize(REP_WORD_PARAM *pRepWordParam, FILE_READER_BUFFER *pRBuf, FILE_WRITER_BUFFER *pWBuf)
{
    fopen_s(&pRBuf->pReader, pRepWordParam->pSrcFile, "r+t");
    fopen_s(&pWBuf->pWriter, pRepWordParam->pDestFile, "w+t");
    if (NULL == pRBuf->pReader || NULL == pWBuf->pWriter)
    {
        return ERR_OPEN_FILE_FAILD;
    }

    pRBuf->pBuf = (char *)malloc(MAX_BUFF_SIZE * sizeof(char));
    if (NULL == pRBuf->pBuf)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pRBuf->pSrcWord = pRepWordParam->pSrcWord;
    pRBuf->reserved = 0;
    pRBuf->strLen = 0;
    pRBuf->offset = 0;
    pRBuf->wordLen = strlen(pRepWordParam->pSrcWord);

    pWBuf->pDestWord = pRepWordParam->pDestWord;
    pWBuf->offset = 0;
    pWBuf->wordLen = strlen(pRepWordParam->pDestWord);

    return ERR_SUCCESS;
}

static void Destroy(FILE_READER_BUFFER *pRBuf, FILE_WRITER_BUFFER *pWBuf)
{
    if (NULL != pRBuf->pReader)
    {
        fclose(pRBuf->pReader);
    }
    if (NULL != pRBuf->pBuf)
    {
        free(pRBuf->pBuf);
    }

    if (NULL != pWBuf->pWriter)
    {
        fclose(pWBuf->pWriter);
    }
}

static inline int ReadBuffer(FILE_READER_BUFFER *pRBuf)
{
    if (feof(pRBuf->pReader))
    {
        return ERR_EOF_FILE;//文件末尾
    }
    int reserved = pRBuf->reserved;
    int readCount = fread(pRBuf->pBuf + reserved, sizeof(char), MAX_BUFF_SIZE - reserved - 1, pRBuf->pReader);
    pRBuf->offset = 0;
    pRBuf->reserved = 0;
    pRBuf->strLen = readCount + reserved;
    pRBuf->pBuf[pRBuf->strLen] = '\0';//追加一个结束符
    return ERR_SUCCESS;
}

static inline void WriteBuffer(FILE_WRITER_BUFFER *pWBuf, const char *pWord)
{
    int wordLen = strlen(pWord);
    if (0 == wordLen)
    {
        return;
    }
    if (pWBuf->offset + wordLen >= MAX_BUFF_SIZE)
    {//将要超出写文件的缓冲,先将当前的缓冲写入文件
        fwrite(pWBuf->pBuf, sizeof(char), pWBuf->offset, pWBuf->pWriter);
        pWBuf->offset = 0;
    }
    memcpy(pWBuf->pBuf + pWBuf->offset, pWord, wordLen);
    pWBuf->offset += wordLen;
}

static inline int FirstStr(char *pBuf, char *pWord)
{
    int strLen;
    char *pStr = strchr(pBuf, ' ');

    if (NULL == pStr)
    {
        return ERR_FAILED;
    }
    strLen = pStr - pBuf;
    memcpy(pWord, pBuf, strLen);
    pWord[strLen] = '\0';
    return ERR_SUCCESS;
}

static inline int DoParse(FILE_READER_BUFFER *pRBuf, FILE_WRITER_BUFFER *pWBuf)
{
    char pWord[MAX_BUFF_SIZE];

    if (ERR_SUCCESS != FirstStr(pRBuf->pBuf, pWord))
    {
        return ERR_WORD_TOO_LONG;
    }
    do
    {
        pRBuf->offset += strlen(pWord) + 1;//跳过单词后面的空格
        if (strcmp(pWord, pRBuf->pSrcWord) == 0)
        {
            WriteBuffer(pWBuf, pWBuf->pDestWord);
        }
        else
        {
            WriteBuffer(pWBuf, pWord);
        }
        //往写缓冲中追加一个空格
        WriteBuffer(pWBuf, " ");

        if (pRBuf->offset > pRBuf->strLen - 1)
        {//已到读缓冲中字串的末尾
            break;
        }
    } while (ERR_SUCCESS == FirstStr(pRBuf->pBuf + pRBuf->offset, pWord));
    if (pRBuf->offset == pRBuf->strLen)
    {//读缓冲中的字串最后一个字符为空格
        //往写缓冲中追加一个空格
        WriteBuffer(pWBuf, " ");
    }
    else 
    {
        pRBuf->reserved = pRBuf->strLen - pRBuf->offset;
        memcpy(pRBuf->pBuf, pRBuf->pBuf + pRBuf->offset, pRBuf->reserved);//将剩余字串移动到缓冲最前端
    }
    return ERR_SUCCESS;
}

static int DoReplace(FILE_READER_BUFFER *pRBuf, FILE_WRITER_BUFFER *pWBuf)
{
    int ret;

    do 
    {
        ret = ReadBuffer(pRBuf);
        if (ERR_SUCCESS == ret)
        {
            ret = DoParse(pRBuf, pWBuf);
        }
    } while (ERR_SUCCESS == ret);
    if (ERR_SUCCESS == ret || ERR_EOF_FILE == ret)
    {
        if (pRBuf->reserved != 0)
        {
            WriteBuffer(pWBuf, pRBuf->pBuf + pRBuf->offset);
        }
        if (pWBuf->offset != 0)
        {
            fwrite(pWBuf->pBuf, sizeof(char), pWBuf->offset, pWBuf->pWriter);
        }
    }
    return ret;
}

int ReplaceWord(REP_WORD_PARAM *pRepWordParam)
{
    int ret = ERR_SUCCESS;
    FILE_READER_BUFFER fReader;
    FILE_WRITER_BUFFER fWriter;

    ret = Initialize(pRepWordParam, &fReader, &fWriter);
    if (ERR_SUCCESS == ret)
    {
        ret = DoReplace(&fReader, &fWriter);
        if (ERR_EOF_FILE == ret)
        {
            ret = ERR_SUCCESS;
        }
    }
    Destroy(&fReader, &fWriter);
    return ret;
}