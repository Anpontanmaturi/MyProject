#include <stdlib.h>
#include "mathf.h"

float Mathf::RandomRange(float min, float max)
{
	float random = static_cast<float>(rand()) / RAND_MAX;
	return min + (max - min) * random;
}