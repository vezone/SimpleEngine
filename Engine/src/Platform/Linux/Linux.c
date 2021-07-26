#include "Linux.h"
#if LINUX_PLATFORM
#include <sys/resource.h>
#include "Utils/Logger.h"

void
linux_set_current_stack_size(i64 currentBytesNumber, i64 maxBytesNumber)
{
    struct rlimit resource_limit;
    i32 result = getrlimit(RLIMIT_STACK, &resource_limit);
    if (result < 0)
    {
	GERROR("Get RLIMIT_STACK error!\n");
	return;
    }
    else
    {
	GLOG("Resource limit: %ld (max: %ld)\n", (i64)resource_limit.rlim_cur, (i64)resource_limit.rlim_max);
	resource_limit.rlim_cur = (i64) currentBytesNumber;
	resource_limit.rlim_max = (i64) maxBytesNumber;
	result = setrlimit(RLIMIT_STACK, &resource_limit);
	if (result == 0)
	{
	    GLOG("New Resource limit: %ld mb (max: %ld)\n", i64(TOMB(resource_limit.rlim_cur)), i64(resource_limit.rlim_max));
	}
    }
}
#endif