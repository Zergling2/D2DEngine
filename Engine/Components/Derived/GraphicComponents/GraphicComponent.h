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
		virtual ~GraphicComponent() = 0;		// �Ļ� Ŭ�������� ������ ���� (�ظ��ϸ� �޸� ���� �۾� �ʿ���)

		virtual void Render() = 0;
	};
}
