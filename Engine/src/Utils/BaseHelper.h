#ifndef BASE_HELPER_H
#define BASE_HELPER_H

#define DO_SINGLE_TIME(action)			\
    {						\
	static i32 lock = 1;			\
	if (lock)				\
	{					\
	    lock = 0;				\
	    action;				\
	}					\
    }

#define DO_MANY_TIME(action, count)		\
    {						\
	static i32 lock = count;		\
	if (lock)				\
	{					\
	    --lock;				\
	    action;				\
	}					\
    }

#endif
