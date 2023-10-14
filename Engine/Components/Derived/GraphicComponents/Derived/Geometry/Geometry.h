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
		virtual ~Geometry() = 0;
	};
}
