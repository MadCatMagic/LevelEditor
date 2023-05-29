#include "Random.h"
#include <cstdlib>

#define PI 3.14159265359f
#define TWOPI 6.28318530718f

namespace Random
{
	static float random()
	{
		return (float)rand() / RAND_MAX;
	}

	float range(float min, float max)
	{
		return random() * (max - min) + min;
	}

	static int randint(int min, int max)
	{
		return rand() % (max - min + 1) + min;
	}

	v2 randv2()
	{
		return v2(random(), random());
	}

	v3 randv3()
	{
		return v3(random(), random(), random());
	}

	v2 onUnitCircle()
	{
		float value = random() * TWOPI;
		return v2(cosf(value), sinf(value));
	}

	v2 inUnitCircle()
	{
		float theta = random() * TWOPI;
		float r = sqrtf(random());
		return v2(cosf(theta), sinf(theta)) * r;
	}

	v3 onUnitSphere()
	{
		float theta = random() * TWOPI;
		float phi = acosf(2.0f * random() - 1.0f);
		return v3(
			sinf(phi) * cosf(theta),
			sinf(phi) * sinf(theta),
			cosf(phi)
		);
	}

	v3 inUnitSphere()
	{
		float theta = random() * TWOPI;
		float phi = acosf(2.0f * random() - 1.0f);
		float r = powf(random(), 1.0f / 3.0f);
		return v3(
			r * sinf(phi) * cosf(theta),
			r * sinf(phi) * sinf(theta),
			r * cosf(phi)
		);
	}
}