#include "AABB.h"

// Collider로부터 AABB 계산
// void ip::AABB::Initialize(const ip::math::Vector2 vertices[], int count)
// {
// #ifdef _DEBUG
// 	if (count < 3)
// 		*reinterpret_cast<int*>(0x00000000) = 0;
// #endif
// 
// 	m_min.x = m_max.x = vertices[0].x;
// 	m_min.y = m_max.y = vertices[0].y;
// 
// 	// 모든 점을 순회하면서 AABB 업데이트
// 	for (int i = 1; i < count; i++)
// 	{
// 		m_min.x = std::fmin(m_min.x, vertices->x);
// 		m_min.y = std::fmin(m_min.y, vertices->y);
// 		m_max.x = std::fmax(m_max.x, vertices->x);
// 		m_max.y = std::fmax(m_max.y, vertices->y);
// 	}
// }


/*
// deprecated
void ip::AABB::Update(const ip::math::Matrix3x2R& transform)
{
	float cosine = transform._11;
	float sine = transform._12;

	ip::math::Vector2 vNewMin;
	ip::math::Vector2 vNewMax;

	if (cosine >= 0.0f)		// -90' ~ 90'
	{
		if (sine >= 0.0f)	// [1] -90' ~ 90' ∩ 0' ~ 180' = 0' ~ 90'
		{
			vNewMin.x = m_objectMin.x * cosine - m_objectMax.y * sine;
			vNewMin.y = m_objectMin.x * sine + m_objectMin.y * cosine;

			vNewMax.x = m_objectMax.x * cosine - m_objectMin.y * sine;
			vNewMax.y = m_objectMax.x * sine + m_objectMax.y * cosine;
		}
		else				// [2] -90' ~ 90' ∩ -180' ~ -0' = -90' ~ -0'
		{
			vNewMin.x = m_objectMin.x * cosine - m_objectMin.y * sine;
			vNewMin.y = m_objectMax.x * sine + m_objectMin.y * cosine;

			vNewMax.x = m_objectMax.x * cosine - m_objectMax.y * sine;
			vNewMax.y = m_objectMin.x * sine + m_objectMax.y * cosine;
		}
	}
	else					// -180' ~ -90' ∪ 90' ~ 180'
	{
		if (sine >= 0.0f)	// [3] (-180' ~ -90' ∪ 90' ~ 180') ∩ 0' ~ 180' = 90' ~ 180'
		{
			vNewMin.x = m_objectMax.x * cosine - m_objectMax.y * sine;
			vNewMin.y = m_objectMin.x * sine + m_objectMax.y * cosine;

			vNewMax.x = m_objectMin.x * cosine - m_objectMin.y * sine;
			vNewMax.y = m_objectMax.x * sine + m_objectMin.y * cosine;
		}
		else				// [4] (-180' ~ -90' ∪ 90' ~ 180') ∩ -180' ~ -0' = -180' ~ -90'
		{
			vNewMin.x = m_objectMax.x * cosine - m_objectMin.y * sine;
			vNewMin.y = m_objectMax.x * sine + m_objectMax.y * cosine;

			vNewMax.x = m_objectMin.x * cosine - m_objectMax.y * sine;
			vNewMax.y = m_objectMin.x * sine + m_objectMin.y * cosine;
		}
	}

	m_worldMin = ip::math::Vector2(vNewMin.x + transform._31, vNewMin.y + transform._32) + m_aabbObjectOrigin;
	m_worldMax = ip::math::Vector2(vNewMax.x + transform._31, vNewMax.y + transform._32) + m_aabbObjectOrigin;
}
*/

/*
// deprecated
void ip::AABB::Update(const ip::math::Matrix3x2R& transform)
{
	float cosine = transform._11;
	float sine = transform._12;

	ip::math::Vector2 vNewMin;
	ip::math::Vector2 vNewMax;

	if (cosine >= 0.0f)		// -90' ~ 90'
	{
		if (sine >= 0.0f)	// [1] -90' ~ 90' ∩ 0' ~ 180' = 0' ~ 90'
		{
			vNewMin.x = m_objectMin.x * cosine - m_objectMax.y * sine;
			vNewMin.y = m_objectMin.x * sine + m_objectMin.y * cosine;

			vNewMax.x = m_objectMax.x * cosine - m_objectMin.y * sine;
			vNewMax.y = m_objectMax.x * sine + m_objectMax.y * cosine;
		}
		else				// [2] -90' ~ 90' ∩ -180' ~ -0' = -90' ~ -0'
		{
			vNewMin.x = m_objectMin.x * cosine - m_objectMin.y * sine;
			vNewMin.y = m_objectMax.x * sine + m_objectMin.y * cosine;

			vNewMax.x = m_objectMax.x * cosine - m_objectMax.y * sine;
			vNewMax.y = m_objectMin.x * sine + m_objectMax.y * cosine;
		}
	}
	else					// -180' ~ -90' ∪ 90' ~ 180'
	{
		if (sine >= 0.0f)	// [3] (-180' ~ -90' ∪ 90' ~ 180') ∩ 0' ~ 180' = 90' ~ 180'
		{
			vNewMin.x = m_objectMax.x * cosine - m_objectMax.y * sine;
			vNewMin.y = m_objectMin.x * sine + m_objectMax.y * cosine;

			vNewMax.x = m_objectMin.x * cosine - m_objectMin.y * sine;
			vNewMax.y = m_objectMax.x * sine + m_objectMin.y * cosine;
		}
		else				// [4] (-180' ~ -90' ∪ 90' ~ 180') ∩ -180' ~ -0' = -180' ~ -90'
		{
			vNewMin.x = m_objectMax.x * cosine - m_objectMin.y * sine;
			vNewMin.y = m_objectMax.x * sine + m_objectMax.y * cosine;

			vNewMax.x = m_objectMin.x * cosine - m_objectMax.y * sine;
			vNewMax.y = m_objectMin.x * sine + m_objectMin.y * cosine;
		}
	}

	//	a b		cos sin
	//			-sin cos

	ip::math::Vector2 offset(
		m_aabbObjectOrigin.x * cosine - m_aabbObjectOrigin.y * sine,
		m_aabbObjectOrigin.x * sine - m_aabbObjectOrigin.y * cosine
	);

	m_worldMin = ip::math::Vector2(vNewMin.x + transform._31, vNewMin.y + transform._32) + offset;
	m_worldMax = ip::math::Vector2(vNewMax.x + transform._31, vNewMax.y + transform._32) + offset;
}
*/