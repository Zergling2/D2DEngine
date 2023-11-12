#include "Collider.h"
#include "Common\Parameters.h"
#include "Components\RigidBody.h"

using namespace ip;

Collider::~Collider()
{
	if (m_pRigidBody)
	{
		delete m_pRigidBody;
		m_pRigidBody = nullptr;
	}
}

bool Collider::CreateRigidBody(real mass, real inertia, real restitution, real sf, real df, bool isStatic)
{
	if (m_pRigidBody != nullptr)
		return false;

	m_pRigidBody = new RigidBody(*this, mass, inertia, restitution, sf, df, isStatic);
	return true;
}

void Collider::SetRotation(real radian)
{
	m_radian = radian;
	if (++m_rotateCounter & parameter::FMOD_FREQUENCY_MASK)
	{
		m_rotateCounter = 0;
		m_radian = std::fmod(m_radian, math::PIx2);
	}
}

void Collider::Rotate(real radian)
{
	m_radian += radian;
	if (++m_rotateCounter & parameter::FMOD_FREQUENCY_MASK)
	{
		m_rotateCounter = 0;
		m_radian = std::fmod(m_radian, math::PIx2);
	}
}
