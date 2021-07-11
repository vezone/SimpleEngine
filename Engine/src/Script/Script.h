#ifndef SCRIPT_H
#define SCRIPT_H

extern int
add(int a, int b);

void
script_test();

/*
Q:
How we will understand that this concrete code we is what we need to build?
- Userspace code is fixed


Steps:
- Building Shared Lib
- FileSystemWatcher should say that file has been changed
- Load .so (or .dll) through out our script_load func

*/


#endif
