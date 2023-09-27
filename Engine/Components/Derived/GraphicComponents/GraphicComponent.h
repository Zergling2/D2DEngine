#pragma once

#include "Components\Component.h"
#include "Components\Derived\Transform.h"

namespace D2DEngine
{
	class GraphicComponent : public Component
	{
		friend class Scene;
	public:
		GraphicComponent(GameObject* pGameObj) : Component(pGameObj) {}
		virtual ~GraphicComponent() = 0;		// 파생 클래스에서 무조건 구현 (왠만하면 메모리 관련 작업 필요함)

		virtual void Render() = 0;
	};
}
