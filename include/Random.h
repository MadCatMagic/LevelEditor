#pragma once
#include "Vector.h"

namespace Random
{
	// random number from 0 to 1
	extern float random();
	// inclusive min and max
	extern float range(float min, float max);
	// inclusive min and max
	extern int randint(int min, int max);

	// random v2 from 0,0 to 1,1
	extern v2 randv2();
	// random v3 from 0,0,0 to 1,1,1
	extern v3 randv3();

	// return a random point on the perimeter of a unit circle
	extern v2 onUnitCircle();
	// return a random point in a unit circle
	extern v2 inUnitCircle();

	// return a random point on the surface of a unit sphere
	extern v3 onUnitSphere();
	// return a random point in a unit sphere
	extern v3 inUnitSphere();
}
