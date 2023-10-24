#pragma once

#ifdef DOUBLE_PRECISION
#define EPA_EPSILON 0.001
#else
#define EPA_EPSILON 0.001f
#endif // DOUBLE_PRECISION

const unsigned int EPA_ITERATION_MAX = 32U;