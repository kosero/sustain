#include "sustain.h"

Vec2 vec2(float x, float y)
{
	Vec2 v;

	v.x = x;
	v.y = y;
	return v;
}

Vec2 vec2_add(Vec2 a, Vec2 b) { return vec2(a.x + b.x, a.y + b.y); }

Vec2 vec2_sub(Vec2 a, Vec2 b) { return vec2(a.x - b.x, a.y - b.y); }

Vec2 vec2_scale(Vec2 v, float s) { return vec2(v.x * s, v.y * s); }
