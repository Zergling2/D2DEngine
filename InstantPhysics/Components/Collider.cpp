#include "Collider.h"
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

bool Collider::CreateRigidBody(real mass, real inertia, real restitution, bool isStatic)
{
	if (m_pRigidBody != nullptr)
		return false;

	m_pRigidBody = new RigidBody(this, mass, inertia, restitution, isStatic);
	return true;
}
