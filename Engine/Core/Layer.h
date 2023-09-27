#pragma once

#include "Object\Derived\GameObject\GameObject.h"

namespace D2DEngine
{
	enum class LayerType
	{
		Default = 0,			// 0

		IgnoreRaycast,			// 1
		UI,						// 2
		Water,					// 3
		Ground,					// 4
		Player,					// 5
		Interactable,			// 6
		Wall,					// 7
		Minimap,				// 8
		

		// #################
		Count
	};

	class Layer
	{
		friend class Engine;
		friend class Scene;
	private:
		inline Layer()
		{
		}
	public:
		inline void SetZOrder(int zOrder) { m_zOrder = zOrder; }
		inline int GetZOrder() const { return m_zOrder; }
	private:
		std::list<GameObject*> m_objectList;
		int m_zOrder;							// Layer z order
	};
}
