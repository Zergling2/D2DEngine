#pragma once

#include "Common\Math2D.h"
#include "Core\Processor\Processor.h"
#include "Components\RigidBody.h"
#include "Components\CircleCollider.h"
#include "Components\PolygonCollider.h"

const unsigned int FIXED_FREQUENCY = 60U;
const float FIXED_DELTA_TIME = 1.0F / (float)FIXED_FREQUENCY;