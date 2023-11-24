#pragma once

namespace ip
{
	// LayerCollisionMatrix Type
	enum CollisionLayer
	{
		Default,
		UI,
		CarBody,
		Wheel,
		Ground,
		Wall,


		// =========
		LayerCount//
		// =========
	};

	class LayerCollisionMatrix
	{
	public:
		// Layer Collision Matrix
		alignas(64) static const bool s_LCM[LayerCount][LayerCount];
	};
}
