#include "BoxCollider.h"
#include "Core\Acc\AABB.h"
#include "Components\RigidBody.h"

using namespace ip;

ip::BoxCollider::BoxCollider(const math::Size2 size)
	: Collider(ColliderType::BOX)
{
	m_pLocalVertices = new math::Vector2[4 * 2];
	m_pWorldVertices = m_pLocalVertices + 4;

	real widthHalf = size.width * real(0.5);
	real heightHalf = size.height * real(0.5);
	m_pLocalVertices[0].x = -widthHalf;	m_pLocalVertices[0].y = -heightHalf;
	m_pLocalVertices[1].x = +widthHalf;	m_pLocalVertices[1].y = -heightHalf;
	m_pLocalVertices[2].x = +widthHalf;	m_pLocalVertices[2].y = +heightHalf;
	m_pLocalVertices[3].x = -widthHalf;	m_pLocalVertices[3].y = +heightHalf;
}

const math::Vector2 BoxCollider::GetFaceNormal(uint16_t faceIndex) const
{
	math::Vector2 face = m_pWorldVertices[(faceIndex + 1) & uint16_t(0x0003)] - m_pWorldVertices[faceIndex];
	return math::Vector2::Normalized(math::Vector2(face.y, -face.x));
}

uint16_t BoxCollider::SupportIndex(const math::Vector2 direction) const
{
	uint16_t bestProjectionIndex = 0;
	real bestProjection = math::Vector2::Dot(m_pWorldVertices[0], direction);
	real dotValue[3];

	dotValue[0] = math::Vector2::Dot(m_pWorldVertices[1], direction);
	dotValue[1] = math::Vector2::Dot(m_pWorldVertices[2], direction);
	dotValue[2] = math::Vector2::Dot(m_pWorldVertices[3], direction);

	if (dotValue[0] > bestProjection)
	{
		bestProjection = dotValue[0];
		bestProjectionIndex = 1;
	}
	if (dotValue[1] > bestProjection)
	{
		bestProjection = dotValue[1];
		bestProjectionIndex = 2;
	}
	if (dotValue[2] > bestProjection)
	{
		bestProjectionIndex = 3;
	}

	return bestProjectionIndex;
}

const math::Vector2 ip::BoxCollider::SupportPoint(const math::Vector2& direction) const
{
	return m_pWorldVertices[SupportIndex(direction)];
}

void BoxCollider::ComputeAABB()
{
	// SIMD code

	// 4개 묶음에서 남는 공간들은 유효한 값 중 하나 골라서 채워놔야 함

	// 4개 한번에 처리 및 최대 최소 구하기
}