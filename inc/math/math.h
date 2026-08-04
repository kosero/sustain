#ifndef SUSTAIN_MATH_H
#define SUSTAIN_MATH_H

#include <stdbool.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} Rect;

bool rect_contains(Rect rect, float x, float y);

#endif // SUSTAIN_MATH_H
