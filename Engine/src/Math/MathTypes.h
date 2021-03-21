#ifndef MATHTYPES
#define MATHTYPES

#include "Utils/Types.h"

// e
#define MATH_E         2.71828182845904523536028747135266250
// log2(e)
#define MATH_LOG2E     1.44269504088896340735992468100189214
// log10(e)
#define MATH_LOG10E    0.434294481903251827651128918916605082
// loge(2)
#define MATH_LN2       0.693147180559945309417232121458176568
// loge(10)
#define MATH_LN10      2.30258509299404568401799145468436421
// pi
#define MATH_PI        3.14159265358979323846264338327950288
// pi/2
#define MATH_PI_2      1.57079632679489661923132169163975144
// pi/4
#define MATH_PI_4      0.785398163397448309615660845819875721
// 1/pi
#define MATH_1_PI      0.318309886183790671537767526745028724  
// 2/pi
#define MATH_2_PI      0.636619772367581343075535053490057448
// 2/sqrt(pi)
#define MATH_2_SQRTPI  1.12837916709551257389615890312154517
// sqrt(2)
#define MATH_SQRT2     1.41421356237309504880168872420969808
// 1/sqrt(2)
#define MATH_SQRT1_2   0.707106781186547524400844362104849039

#define MATH_Ef        ((f32) MATH_E)
#define MATH_LOG2Ef    ((f32) MATH_LOG2E) 
#define MATH_LOG10Ef   ((f32) MATH_LOG10E)   
#define MATH_LN2f      ((f32) MATH_LN2)  
#define MATH_LN10f     ((f32) MATH_LN10)
#define MATH_PIf       ((f32) MATH_PI)
#define MATH_PI_2f     ((f32) MATH_PI_2f)
#define MATH_PI_4f     ((f32) MATH_PI_4f)
#define MATH_1_PIf     ((f32) MATH_1_PI)
#define MATH_2_PIf     ((f32) MATH_2_PI)
#define MATH_2_SQRTPIf ((f32) MATH_2_SQRTPI)
#define MATH_SQRT2f    ((f32) MATH_SQRT2)
#define MATH_SQRT1_2f  ((f32) MATH_SQRT1_2)

typedef f32 Vector2f[2];
typedef f32 Vector3f[3];
typedef f32 Vector4f[4];

typedef i32 Vector2i[2];
typedef i32 Vector3i[3];
typedef i32 Vector4i[4];

typedef f64 Vector2d[2];
typedef f64 Vector3d[3];
typedef f64 Vector4d[4];

typedef Vector3f Matrix3f[3];
typedef Vector4f Matrix4f[4];

typedef Vector3i Matrix3i[3];
typedef Vector3i Matrix4i[4];

#endif
