#include <stdio.h>
#include <locale.h>
#include "err_no.h"
#include "file_tree.h"

int main(int argc, char **argv)
{
    int ret = ERR_SUCCESS;
    void *pRootNode = NULL;
    char *dir = "F:\\ColdBladeLib\\";
    
    setlocale(LC_ALL, "");
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