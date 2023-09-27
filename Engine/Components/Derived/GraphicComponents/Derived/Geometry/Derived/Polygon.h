#pragma once

#include "Components\Derived\GraphicComponents\Derived\Geometry\Geometry.h"

namespace D2DEngine
{
	class Polygon : public Geometry
	{
	public:
		// [Parameters]
		// D2D1_POINT_2F vArr[]: Vertex array
		// UINT count: Number of vertex buffer items.
		Polygon(GameObject* pGameObj, D2D1_POINT_2F vArr[], UINT count);
		virtual ~Polygon();

		inline ID2D1PathGeometry* GetGeometry() { return m_pShape; }
		inline const ID2D1PathGeometry* GetGeometry() const { return m_pShape; }

	protected:
		virtual void Render() override;
	private:
		BrushType m_bt;
		ID2D1SolidColorBrush* m_pBrush;
		ID2D1PathGeometry* m_pShape;
	};
}