#include "Collider.h"
#include "Common\IPMacro.h"

using namespace ip;

Collider::~Collider()
{
	DeleteAABBNode();
}

bool Collider::Initialize()
{
	return CreateAABBNode();
}

bool Collider::CreateAABBNode()
{
	// Create AABB Node
	m_AABB.pMinXNode = new (std::nothrow)AABBNode(this, Type::MIN);
	m_AABB.pMaxXNode = new (std::nothrow)AABBNode(this, Type::MAX);

	if (!m_AABB.pMinXNode || !m_AABB.pMaxXNode)
		return false;

	return true;
}

void Collider::DeleteAABBNode()
{
	IPSafeDelete(m_AABB.pMinXNode);
	IPSafeDelete(m_AABB.pMaxXNode);
}
