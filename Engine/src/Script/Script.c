#include "Script.h"

#if LINUX_PLATFORM
#include <dlfcn.h>
#include <stdio.h>

int
add(int a, int b)
{
    return a + b;
}

typedef int (*AddPredicate)(int, int);

#define script_test_func(func) printf("Result %s == %d\n", #func, func)

void
script_test()
{
    void* sharedLib = dlopen("assets/SharedLibs/libScript.so", RTLD_LAZY);
    if (!sharedLib)
    {
	return;
    }

    AddPredicate addFunction;
    char* error;
    addFunction = dlsym(sharedLib, "add");
    if ((error = dlerror()) != NULL)
    {
	return;
    }

    script_test_func(add(10, 15));

    dlclose(sharedLib);
}
#endif