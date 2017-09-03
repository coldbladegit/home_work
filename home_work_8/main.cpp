#include <stdio.h>
#include "err_no.h"
#include "lib_malloc_free.h"

int main (int argc, char **argv)
{
	int ret = ERR_SUCCESS;
	void *p1, *p2, *p3, *p4, *p5;

	ret = lib_init(8);
	if (ERR_SUCCESS != ret)
	{
		printf("failed to initialize lib!!!\n");
		return 0;
	}
	
	lib_malloc(&p1, 16, __FILE__, __LINE__);
	lib_malloc(&p2, 16, __FILE__, __LINE__);
	lib_malloc(&p3, 16, __FILE__, __LINE__);
	lib_malloc(&p4, 16, __FILE__, __LINE__);
	lib_malloc(&p5, 16, __FILE__, __LINE__);

	lib_print_memory();

	lib_destroy();
	return 0;
}