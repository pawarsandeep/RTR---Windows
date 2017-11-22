#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(void)
{
	void *handle;
	char *error;
	typedef float(*pfn)(float);
	pfn pfnSquare = NULL;
	handle = dlopen("./SharedObject.so", RTLD_LAZY);
	if(!handle)
	{
		fputs(dlerror(), stderr);
		exit(1);
	}
	pfnSquare = (pfn)dlsym(handle, "makeSquare");
	if((error = dlerror()) !=NULL)
	{
		fputs(error, stderr);
		exit(1);
	}
	pfnSquare(2.5f);
	dlclose(handle);
	return(0);
}
