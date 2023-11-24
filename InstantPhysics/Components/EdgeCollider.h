#pragma once

#include "Components\Collider.h"
#include <vector>

namespace ip
{
	class EdgeCollider : public Collider
	{
	public:
		EdgeCollider(math::Vector2*);
		virtual ~EdgeCollider() override;
	private:
		virtual void UpdateCollider() override;
	};
}
