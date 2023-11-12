#pragma once

#include "Type.h"

namespace ip
{
	namespace parameter
	{
		constexpr real EPA_EPSILON = real(0.0001);
		constexpr unsigned int PHYSICS_UPDATE_FREQUENCY = 60U;
		constexpr real PHYSICS_UPDATE_INTERVAL = real(1.0) / (real)PHYSICS_UPDATE_FREQUENCY;
		constexpr unsigned int EPA_ITERATION_MAX = 32U;
		constexpr uint16_t FMOD_FREQUENCY_MASK = uint16_t(0x0200);		// 512번 회전마다 mod
	}
}
