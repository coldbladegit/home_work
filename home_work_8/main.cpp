#include <stdio.h>
#include "err_no.h"
#include "lib_malloc_free.h"

int DoMalloc(void *pMems[], int len);

int DoFree(void *pMems[], int len);

int main (int argc, char **argv)
{
	int ret = ERR_SUCCESS, used = 0, capacity = 8;
    void *pMems[5] = {NULL};

	ret = lib_init(capacity);
	if (ERR_SUCCESS != ret)
	{
		printf("failed to initialize lib!!!\n");
		return 0;
	}
	
    do 
    {
        ret = DoMalloc(pMems, 5);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
        lib_get_used(&used);
        printf("∏∫‘ÿ“Ú◊”(used / capacity): (%d / %d)\n", used, capacity);
        ret = DoFree(pMems, 3);
        if (ERR_SUCCESS != ret)
        {
            break;
        }

        ret = lib_print_memory();
    } while (0);

    if (ERR_NOT_INITIALIZED == ret)
    {
        printf("the lib has not initialized!!!\n");
    }
    else if (ERR_MALLOC_MEM_FAILD == ret)
    {
        printf("there is not enough memory!!!\n");
    }
    else if (ERR_NOT_FIND == ret)
    {
        printf("the memory has been free completed !!!\n");
    }
	lib_destroy();
	return 0;
}

int DoMalloc(void *pMems[], int len)
{
    int ret = ERR_SUCCESS;

    for (int index = 0; index < len; ++index)
    {
        ret = lib_malloc(&pMems[index], 16);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
    }
    return ret;
}

int DoFree(void *pMems[], int len)
{
    int ret = ERR_SUCCESS;

    for (int index = 0; index < len; ++index)
    {
        ret = lib_free(pMems[index]);
        if (ERR_SUCCESS != ret)
        {
            break;
        }
    }
    return ret;
}