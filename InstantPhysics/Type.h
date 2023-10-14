#pragma once

#include <limits>

#ifdef DOUBLE_PRECISION
typedef double real;
#define real_min (-1.7976931348623158e+308)
#else
typedef float real;
#define real_min (-3.402823466e+38F)
#endif // DOUBLE_PRECISION
