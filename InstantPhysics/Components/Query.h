#pragma once

#include "Common\Math2D.h"

namespace ip
{
	struct Query
	{
		bool axisExist;					// separating axis
		uint16_t refFaceIndex;
		uint16_t incSupportIndex;
		real bestDistance;
		math::Vector2 refFaceNormal;		// 함수 밖에서 노말 재계산 방지
	};
}