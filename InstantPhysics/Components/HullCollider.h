#pragma once
#include "Components\Collider.h"

namespace ip
{
	class HullCollider : public Collider
	{
	public:
		// x0, y0, x1, y1, x2, y2, x3, y3
		HullCollider(class PhysicsObject* pPhysObj, CollisionLayer layer);
		virtual ~HullCollider() override;
		bool Initialize(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount);
		// ==========================================================================
		inline uint16_t GetNextVertexIndex(uint16_t vertexIndex) const
		{
			if (vertexIndex == m_vertexCount - 1)
				return 0;
			else
				return vertexIndex + 1;
		}
		
		inline uint16_t GetPrevVertexIndex(uint16_t vertexIndex) const
		{
			if (vertexIndex == 0)
				return m_vertexCount - 1;
			else
				return vertexIndex - 1;
		}
		__forceinline uint16_t GetNextFaceIndex(uint16_t faceIndex) const { return GetNextVertexIndex(faceIndex); }
		__forceinline uint16_t GetPrevFaceIndex(uint16_t faceIndex) const { return GetPrevVertexIndex(faceIndex); }
		const math::Vector2 GetFaceNormal(uint16_t faceIndex) const;
		uint16_t SupportIndex(const math::Vector2 direction) const;
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// 월드 좌표, 월드 벡터에 대해 수행
		virtual void UpdateCollider() override;
	public:
		uint16_t m_vertexCount;
		real* m_pLocalX;		// CCW
		real* m_pLocalY;
		real* m_pWorldX;
		real* m_pWorldY;
	};
}
