#ifndef _REPSTR_H
#define _REPSTR_H

typedef struct _REP_WORD_PARAM {
    const char *pSrcFile;
    const char *pSrcWord;
    const char *pDestWord;
    const char *pDestFile;
}REP_WORD_PARAM;

int ReplaceWord(REP_WORD_PARAM *pRepWordParam);

#endif//_REPSTR_H