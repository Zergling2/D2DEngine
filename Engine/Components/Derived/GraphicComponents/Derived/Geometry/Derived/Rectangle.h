#pragma once

#include "Components\Derived\GraphicComponents\Derived\Geometry\Geometry.h"

namespace D2DEngine
{
	// Final class
	class Rectangle : public Geometry
	{
	public:
		Rectangle(GameObject* pGameObj, const D2D_RECT_F& shape);
		virtual ~Rectangle();

		inline ID2D1RectangleGeometry* GetGeometry() { return m_pShape; }
		inline const ID2D1RectangleGeometry* GetGeometry() const { return m_pShape; }

		virtual void Render() override;
	private:
		BrushType m_bt;
		ID2D1SolidColorBrush* m_pBrush;
		ID2D1RectangleGeometry* m_pShape;
	};
}
