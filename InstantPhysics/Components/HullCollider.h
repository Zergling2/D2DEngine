#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class HullCollider : public Collider
	{
	public:
		// x0, y0, x1, y1, x2, y2, x3, y3
		HullCollider(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount);
		virtual ~HullCollider() override
		{
			if (m_pLocalVertices != nullptr)
			{
				delete[] m_pLocalVertices;		// m_pWorldVertices는 delete X
				m_pLocalVertices = nullptr;
			}
		}
		// ==========================================================================
		inline uint16_t GetNextVertexIndex(uint16_t vertexIndex) const
		{
			uint16_t nextIndex = vertexIndex + uint16_t(1);
			if (nextIndex == m_vertexCount)
				nextIndex = 0;
			return nextIndex;
		}
		
		inline uint16_t GetPrevVertexIndex(uint16_t vertexIndex) const
		{
			uint16_t prevIndex = vertexIndex - uint16_t(1);
			if (prevIndex == uint16_t(0xFFFF))
				prevIndex = m_vertexCount - 1;
			return prevIndex;
		}
		__forceinline uint16_t GetNextFaceIndex(uint16_t faceIndex) const { return GetNextVertexIndex(faceIndex); }
		__forceinline uint16_t GetPrevFaceIndex(uint16_t faceIndex) const { return GetPrevVertexIndex(faceIndex); }
		const math::Vector2 GetFaceNormal(uint16_t faceIndex) const;
		uint16_t SupportIndex(const math::Vector2 direction) const;
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// 월드 좌표, 월드 벡터에 대해 수행
		virtual void ComputeAABB() override;
	public:
		uint16_t m_vertexCount;
		math::Vector2* m_pLocalVertices;		// CCW
		math::Vector2* m_pWorldVertices;
	};
}
