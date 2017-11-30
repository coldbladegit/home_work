#ifndef _ERR_NO_H
#define _ERR_NO_H

#include <assert.h>

#define G_ASSERT(p)     if (!(p)) {\
                            printf("%s %d is asserted!\n", __FILE__, __LINE__);\
                        }\
                        assert(p);

#define ERR_SUCCESS             0
#define ERR_FAILED              -1
#define ERR_INVALID_PARAM       -2
#define ERR_MALLOC_MEM_FAILD    100
#define ERR_OPEN_FILE_FAILD     101
#define ERR_READ_FILE_FAILED    102
#define ERR_EOF_FILE            103
#define ERR_INVALID_OP          104
#define ERR_WORD_TOO_LONG       105
#define ERR_STACK_FULL          106
#define ERR_STACK_EMPTY         107
#define ERR_STR_TOO_LONG        108
#define ERR_NOT_FIND            109
#define ERR_NOT_INITIALIZED		110
#define ERR_INIT_SOCKET_FAILED  111
#define ERR_SESSION_IS_WORKING  112
#define ERR_SESSION_NOT_START   113
#define ERR_INVALID_INDEX       114

#endif//_ERR_NO_H