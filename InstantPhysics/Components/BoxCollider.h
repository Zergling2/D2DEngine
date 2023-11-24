#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	// Box Collider vertices
	//         |
	//   [3]   |   [2]
	//         |
	// ------------------
	//		   |
	//   [0]   |   [1]
	//         |

	class BoxCollider : public Collider
	{
	public:
		// x0, y0, x1, y1, x2, y2, x3, y3
		BoxCollider(const math::Size2 size);
		virtual ~BoxCollider() override
		{
			if (m_pLocalVertices != nullptr)
			{
				delete[] m_pLocalVertices;		// m_pWorldVertices´Â delete X
				m_pLocalVertices = nullptr;
			}
		}
		// ==========================================================================
		inline real ExtentX() const { return m_pLocalVertices[2].x; }
		inline real ExtentY() const { return m_pLocalVertices[2].y; }
		const math::Vector2 GetFaceNormal(uint16_t faceIndex) const;
		// void GetWorldMinMax(const math::Vector2 projAxis, real* pMin, real* pMax) const;
		uint16_t SupportIndex(const math::Vector2 direction) const;
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;
	public:
		math::Vector2* m_pLocalVertices;		// CCW
		math::Vector2* m_pWorldVertices;
	};
}
