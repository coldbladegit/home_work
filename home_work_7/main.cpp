#include <stdio.h>
#include <locale.h>
#include "err_no.h"
#include "file_tree.h"

int main(int argc, char **argv)
{
	int ret = ERR_SUCCESS;
	char *dir = NULL;
	void *pRootNode = NULL;

	if (argc < 2)
	{
		printf("please input: <directory path>\n");
		return 0;
	}
    setlocale(LC_ALL, "");
	dir = argv[1];
    do 
    {
        ret = ListDirectoryFiles(dir, &pRootNode);
        if (ERR_SUCCESS != ret)
        {
            printf("List directory failed!\n");
            break;
        }
        ret = SortByModifyTime(pRootNode);
        if (ERR_SUCCESS != ret)
        {
            printf("Sort file failed!\n");
            break;
        }
        PrintFileTree(pRootNode);
    } while (0);
    
    FreeFileTree(pRootNode);
    return 0;
}