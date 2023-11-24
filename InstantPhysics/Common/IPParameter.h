#pragma once

#include "Common\Type.h"

namespace ip
{
	namespace parameter
	{
		constexpr real EPA_EPSILON = real(0.0001);
		constexpr unsigned int PHYSICS_UPDATE_FREQUENCY = 120;
		constexpr real PHYSICS_UPDATE_INTERVAL = real(1.0) / (real)PHYSICS_UPDATE_FREQUENCY;
		constexpr unsigned int SUB_STEP_COUNT = 1;
		constexpr unsigned int EPA_ITERATION_MAX = 32;
		constexpr long long ACTIVE_SET_MAX_SIZE = 64 * 4 * 1024;		// 64 PAGE
		constexpr long long AABBSWEEPLIST_MAX_SIZE = 128 * 4 * 1024;	// 128 PAGE
		constexpr long long ADDITIONAL_ALLOC_SIZE = 4 * 1024;
		constexpr unsigned int MAX_THREADS = 8;
		constexpr long PHYSOBJ_MULTITHREAD_UPDATE_LOWER_LIMIT = 256;
	}
}
