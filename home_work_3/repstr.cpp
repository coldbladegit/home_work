#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "repstr.h"
#include "err_no.h"

#define MAX_BUFF_SIZE   1024
#define MAX_WORD_LEN    1024

typedef struct _FILE_READER_BUFFER{
    int         reserved;//一次解析后剩余的字串
    int         charCnt;//当前缓冲中整个字串的长度
    int         rOffset;//遍历偏移
    int         wOffset;//写偏移
    int         wordLen;
    FILE        *pRStream;
    const char  *pSrcWord;
    char        *pBuf;
}FILE_READER;

typedef struct _FILE_WRITER {
    int         wordLen;
    FILE        *pWStream;
    const char  *pDestWord;
}FILE_WRITER;

static int Initialize(REP_WORD_PARAM *pRepWordParam, FILE_READER *pReader, FILE_WRITER *pWriter)
{
    fopen_s(&pReader->pRStream, pRepWordParam->pSrcFile, "r+t");
    fopen_s(&pWriter->pWStream, pRepWordParam->pDestFile, "w+t");
    if (NULL == pReader->pRStream || NULL == pWriter->pWStream)
    {
        pReader->pBuf = NULL;//防止释放的时候由于没初值导致错误的释放
        return ERR_OPEN_FILE_FAILD;
    }

    pReader->pBuf = (char *)malloc(MAX_BUFF_SIZE * sizeof(char));
    if (NULL == pReader->pBuf)
    {
        return ERR_MALLOC_MEM_FAILD;
    }
    pReader->pSrcWord = pRepWordParam->pSrcWord;
    pReader->reserved = 0;
    pReader->charCnt = 0;
    pReader->rOffset = 0;
    pReader->wOffset = 0;
    pReader->wordLen = strlen(pRepWordParam->pSrcWord);

    pWriter->pDestWord = pRepWordParam->pDestWord;
    pWriter->wordLen = strlen(pRepWordParam->pDestWord);

    return ERR_SUCCESS;
}

static void Destroy(FILE_READER *pReader, FILE_WRITER *pWriter)
{
    if (NULL != pReader->pRStream)
    {
        fclose(pReader->pRStream);
    }
    if (NULL != pReader->pBuf)
    {
        free(pReader->pBuf);
    }

    if (NULL != pWriter->pWStream)
    {
        fclose(pWriter->pWStream);
    }
}

static inline int ReadFile(FILE_READER *pReader)
{
    if (feof(pReader->pRStream))
    {
        return ERR_EOF_FILE;//文件末尾
    }
    int reserved = pReader->reserved;
    int readCount = fread(pReader->pBuf + reserved, sizeof(char), MAX_BUFF_SIZE - reserved, pReader->pRStream);
    pReader->rOffset = 0;
    pReader->wOffset = 0;
    pReader->reserved = 0;
    pReader->charCnt = readCount + reserved;
    return ERR_SUCCESS;
}

static inline void WriteFile(FILE_WRITER *pWriter, FILE_READER *pReader, bool isReplace)
{
    int writeLen = pReader->rOffset - pReader->wOffset;
    if (writeLen > 0)
    {
        fwrite(pReader->pBuf + pReader->wOffset, sizeof(char), writeLen, pWriter->pWStream);
        pReader->wOffset += writeLen;
    }
    if (isReplace)
    {
        fwrite(pWriter->pDestWord, sizeof(char), pWriter->wordLen, pWriter->pWStream);
        pReader->wOffset += pReader->wordLen;
    }
}

static inline int FindNextWord(FILE_READER *pReader)
{
    for (int index = pReader->rOffset; index < pReader->charCnt; ++index)
    {
        if (pReader->pBuf[index] == ' ')
        {
            return index - pReader->rOffset;
        }
    }

    return -1;
}

static int DoParse(FILE_READER *pReader, FILE_WRITER *pWriter)
{
    int ret = ERR_SUCCESS, wordLen = 0;

    do 
    {
        wordLen = FindNextWord(pReader);
        if (-1 == wordLen)
        {
            if (pReader->charCnt - pReader->rOffset > MAX_WORD_LEN)
            {
                ret = ERR_WORD_TOO_LONG;
            }
            break;
        }
        if (wordLen == pReader->wordLen && memcmp(pReader->pSrcWord, pReader->pBuf + pReader->rOffset, wordLen) == 0)
        {//遇到单词替换的情况就去写一次文件
            WriteFile(pWriter, pReader, true);
        }
        pReader->rOffset += wordLen + 1;//跳过空格字符
    } while (pReader->rOffset < pReader->charCnt);
    
    if (ERR_SUCCESS == ret)
    {
        //将缓冲区中的之前遍历了但未写入文件的字串写入文件
        WriteFile(pWriter, pReader, false);
        pReader->reserved = pReader->charCnt - pReader->rOffset;
        if (pReader->reserved > 0 && pReader->rOffset > 0)
        {//缓冲区中最后一个字符不是空格,则移动未解析的字串到缓冲区的首部
            memmove(pReader->pBuf,  pReader->pBuf + pReader->rOffset, pReader->reserved);
        }
    }
    return ret;
}

static int DoReplace(FILE_READER *pReader, FILE_WRITER *pWriter)
{
    int ret;

    do 
    {
        ret = ReadFile(pReader);
        if (ERR_SUCCESS == ret)
        {
            ret = DoParse(pReader, pWriter);
        }
    } while (ERR_SUCCESS == ret);

    if (ERR_EOF_FILE == ret)
    {
        if (pReader->reserved > 0)
        {//处理最后遗留在缓冲区中的字串
            if (pReader->reserved == pReader->wordLen 
                && memcmp(pReader->pSrcWord, pReader->pBuf + pReader->rOffset, pReader->wordLen) == 0)
            {//要替换的单词
                WriteFile(pWriter, pReader, true);
            }
            else
            {
                pReader->rOffset = pReader->reserved;
                WriteFile(pWriter, pReader, false);
            }
        }
        ret = ERR_SUCCESS;
    }
    return ret;
}

int ReplaceWord(REP_WORD_PARAM *pRepWordParam)
{
    int ret = ERR_SUCCESS;
    FILE_READER fReader;
    FILE_WRITER fWriter;

    ret = Initialize(pRepWordParam, &fReader, &fWriter);
    if (ERR_SUCCESS == ret)
    {
        ret = DoReplace(&fReader, &fWriter);
    }
    Destroy(&fReader, &fWriter);
    return ret;
}