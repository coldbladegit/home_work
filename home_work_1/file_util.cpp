#include <stdio.h>
#include <memory.h>
#include <io.h>

#include "file_util.h"
#include "err_no.h"

#define READ_BUF_SIZE                         1024  //1K
#define LINE_CHAR_CNT                         16
#define LINE_DATA_ASCII_BUF_SIZE              (LINE_CHAR_CNT * 3)

typedef struct _FILE_INFO_EX 
{
    const char  *pFilePath;
    FILE        *pStream;
    long        fileSize;
}FILE_INFO_EX, *PFILE_INFO_EX;

typedef struct _LINE_DATA
{
    short charCnt; //������ַ�����
    short curIndex;//ascBuf��ǰ��ƫ��
    int   lineIndex;//�����ݵ�����
    char  ascBuf[LINE_DATA_ASCII_BUF_SIZE + 1];//���һ��Ϊ������
    char  charBuf[LINE_CHAR_CNT + 1];//���һ��Ϊ������
}LINE_DATA, *PLINE_DATA;

static inline void AppendChar(LINE_DATA *pData, char c)
{
    //sprintf�ڲ��Ὣcת��int����,�������������븺��,����ɷ���λ������
    pData->curIndex += sprintf(pData->ascBuf + pData->curIndex, "%02x", (unsigned char)c);
    pData->ascBuf[pData->curIndex] = ' ';//��sprintf��д�ġ�\0���滻�ɿո�
    pData->curIndex++;

    if (c >= 32 && c < 127)//�ɼ��ַ�
    {
        pData->charBuf[pData->charCnt] = c;
    }
    else
    {
        pData->charBuf[pData->charCnt] = '.';
    }
    pData->charCnt++;
}

static inline void PrintLineData(LINE_DATA *pData)
{
    printf("%05xh: %s", pData->lineIndex, pData->ascBuf);
    printf("%s\n", pData->charBuf);

    pData->charCnt = 0;
    pData->curIndex = 0;
    pData->lineIndex += LINE_CHAR_CNT;
    memset(pData->ascBuf, ' ', LINE_DATA_ASCII_BUF_SIZE);
    memset(pData->charBuf, 0, sizeof(pData->charBuf));
}

static int Initialize(const char *fPath, LINE_DATA *pData, FILE_INFO_EX *pFileInfo)
{
    pFileInfo->pStream = fopen(fPath, "r+b");
    if (NULL == pFileInfo->pStream)
    {
        return ERR_OPEN_FILE_FAILD;
    }
    pFileInfo->pFilePath = fPath;
    pFileInfo->fileSize = _filelength(_fileno(pFileInfo->pStream));

    pData->charCnt = 0;
    pData->curIndex = 0;
    pData->lineIndex = 0;
    memset(pData->ascBuf, ' ', LINE_DATA_ASCII_BUF_SIZE);
    pData->ascBuf[LINE_DATA_ASCII_BUF_SIZE] = '\0';//Ĭ�����һ��Ϊ������
    memset(pData->charBuf, 0, sizeof(pData->charBuf));

    return ERR_SUCCESS;
}

static void Destroy()
{
    //TODO:
}

static void PrintHeader(FILE_INFO_EX *pFileInfo) 
{//start read file
    printf("====== %s ====== len: %ld(%04xh)\n", pFileInfo->pFilePath, pFileInfo->fileSize, pFileInfo->fileSize);
    printf("        ");
    for (int i = 0; i < LINE_CHAR_CNT - 1; ++i)
    {
        printf("%02x ", i);    
    }
    printf("%02x\n", LINE_CHAR_CNT - 1);
}

static void PrintBuffer(char *pBuf, int cnt, LINE_DATA *pData)
{
    for (int i = 0; i < cnt; ++i)
    {
        AppendChar(pData, *(pBuf + i));
        if (LINE_CHAR_CNT == pData->charCnt)
        {//һ�����ݳɹ��������
            PrintLineData(pData);
        }
    }
}

int PrintFile(const char *fPath)
{
    char buf[READ_BUF_SIZE];
    int readCnt = 0, ret = ERR_SUCCESS;
    FILE_INFO_EX fileInfo;
    LINE_DATA lineData;
    PFILE_INFO_EX pFileInfo = &fileInfo;
    PLINE_DATA    pLineData = &lineData;

    //initialize FILE_INFO_EX and LINE_DATA
    ret = Initialize(fPath, pLineData, pFileInfo);
    if (ERR_SUCCESS != ret)
    {
        return ret;
    }
    
    PrintHeader(pFileInfo);
    
    while (!feof(pFileInfo->pStream))
    {
        readCnt = fread(buf, 1, READ_BUF_SIZE, pFileInfo->pStream);
        ret = ferror(pFileInfo->pStream);
        if (0 != ret)
        {
            break;
        }
        PrintBuffer(buf, readCnt, pLineData);
    }

    if (0 != pLineData->charCnt)
    {//��ӡ������һ��û��������������
        printf("%05xh: %s", pLineData->lineIndex, pLineData->ascBuf);
        printf("%s\n", pLineData->charBuf);
    }
    
    fclose(pFileInfo->pStream);
    //destroy (current is do nothing)
    Destroy();
    return ret;
}