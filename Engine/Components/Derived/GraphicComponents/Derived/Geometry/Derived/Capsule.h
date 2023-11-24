#pragma once

#include "Components\Derived\GraphicComponents\Derived\Geometry\Geometry.h"

namespace D2DEngine
{
	namespace Graphic
	{
		class Capsule : public Geometry
		{
		public:
			// Parameters
			// 1. Create with D2D1::Ellipse function.
			Capsule(GameObject* pGameObj, float radius, float length);
			virtual ~Capsule();

			inline ID2D1PathGeometry* GetGeometry() { return m_pShape; }
			inline const ID2D1PathGeometry* GetGeometry() const { return m_pShape; }
		public:
			ID2D1PathGeometry* m_pShape;
		};
	}
}
