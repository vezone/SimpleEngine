#ifndef BASEMATH_H
#define BASEMATH_H

#include "Utils/Types.h"

force_inline f32
pow2f(f32 x)
{
    return x * x;
}

force_inline i32
number_rank(i32 number)
{
    i32 rank = 0;
    for (; ;)
    {
	number /= 10;
	if (number != 0)
	{
	    ++rank;
	}
	else
	{
	    return rank;
	}
    }
}

//TODO: rename this function
force_inline i32
number_of_rank(i32 number, i32 rank)
{
    if (rank <= 0)
    {
	return number;
    }

    for (i32 i = 0; i < rank; i++)
    {
	number *= 10;
    }

    return number;
}

force_inline i32
number_of_digit(i64 number, i8 digit)
{
    i32 i;

    if (number_rank(number) < digit)
    {
	return 0;
    }

    if (number < 0)
    {
	number *= -1;
    }

    if (digit == 0)
    {
	return (number % 10);
    }

    number %= number_of_rank(1, (digit + 1));
    number /= number_of_rank(1, digit);

    return number;
}

#endif