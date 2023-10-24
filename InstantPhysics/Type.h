#pragma once

#include <limits>

#ifdef DOUBLE_PRECISION
typedef double real;
#define real_max (+DBL_MAX)		// +1.7976931348623158e+308
#define real_min (-DBL_MAX)		// -1.7976931348623158e+308
#else
typedef float real;
#define real_max (+FLT_MAX)		// +3.402823466e+38F
#define real_min (-FLT_MAX)		// -3.402823466e+38F
#endif // DOUBLE_PRECISION
