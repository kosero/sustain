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

Rect rect(float x, float y, float w, float h)
{
	Rect rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	return rect;
}
float rect_right(Rect r) { return r.x + r.w; }
float rect_bottom(Rect r) { return r.y + r.h; }
int rect_intersect(Rect a, Rect b)
{
	return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h &&
		a.y + a.h > b.y);
}
