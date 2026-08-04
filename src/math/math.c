#include "math/math.h"

bool rect_contains(Rect rect, float x, float y)
{
	return (bool)(x >= rect.x && x < rect.x + rect.w && y >= rect.y &&
		      y < rect.y + rect.h);
}
