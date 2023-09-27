#pragma once

#include "Components\Derived\GraphicComponents\GraphicComponent.h"

namespace D2DEngine
{
	enum class BrushType
	{
		SolidColorBrush,
		LinearColorBrush,
		RadialGradientBrush,
		BitmapBrush,
		ColorBrush
	};

	class Geometry : public GraphicComponent
	{
	public:
		Geometry(GameObject* pGameObj) : GraphicComponent(pGameObj) {}
		virtual ~Geometry() = 0;		// 파생 클래스에서 무조건 구현 (왠만하면 메모리 관련 작업 필요함)

		virtual void Render() = 0;
	};
}
