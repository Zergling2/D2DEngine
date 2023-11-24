#include "LayerCollisionMatrix.h"

using namespace ip;

#define o true
#define x false

/*
			Default	UI		CarBody	Wheel	Ground	Wall
Default		o		x		o		o		o		o
UI			x		x		x		x		x		x
CarBody		o		x		o		o		o		o
Wheel		o		x		o		o		o		o
Ground		o		x		o		o		x		x
Wall		o		x		o		o		x		x
*/

alignas(64) const bool LayerCollisionMatrix::s_LCM[LayerCount][LayerCount] =
{
	o, x, o, o, o, o,
	x, x, x, x, x, x,
	o, x, o, o, o, o,
	o, x, o, o, o, o,
	o, x, o, o, x, x,
	o, x, o, o, x, x
};