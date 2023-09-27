#pragma once

#include "Headers\Essential.h"

namespace D2DEngine
{
	/*
	enum class ComponentType
	{
		Transform,
		Script,
		

		// GRAPHIC COMPONENT TYPE
		Graphic = 0x00FFFFFF,
		Polygon,
		Rectangle,
		RoundedRectangle,
		Ellipse
	};
	*/
	

	// Base class of all components used in the engine.
	class Component
	{
	public:
		inline Component(class GameObject* pGameObj)
			: m_pGameObject(pGameObj)
			, m_enable(true)
		{
		}
		virtual ~Component() = 0;

		virtual void OnEnable() {};
		virtual void OnDisable() {};
		virtual void OnDestroy() {};

		// Returns the name of the object.
		inline void Enable() { m_enable = true; OnEnable(); }
		inline void Disable() { m_enable = false; OnDisable(); }
		inline bool CheckEnable() { return m_enable; }

		inline class GameObject* GetGameObject() const { return m_pGameObject; }
	private:
		class GameObject* m_pGameObject;
		bool m_enable;
	};
}
