#ifndef BASEMATH_H
#define BASEMATH_H

#include "MathTypes.h"
#include "Utils/Types.h"
//TODO(bies): remove this
#include <math.h>

force_inline i32
i32_pow(i32 x, i32 n)
{
    i32 result = 1;

    if (n == 0 || x == 1)
    {
	return 1;
    }

    while (n)
    {
	result *= x;
	--n;
    }

    return result;
}

force_inline f32
pow2f(f32 x)
{
    return x * x;
}

/*
  number_rank(1000) == 3;
*/
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

#define EPSILON 0.0001
force_inline i32
f32_equal(f32 a, f32 b)
{
    return (fabs(a - b) <= EPSILON);
}

force_inline f32
rad(f32 degrees)
{
    return (MATH_PI / 180) * degrees;
}

force_inline f32
deg(f32 radians)
{
    return (180 / MATH_PI) * radians;
}

#endif
