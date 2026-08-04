#ifndef SUSTAIN_CAMERA_H
#define SUSTAIN_CAMERA_H

#include <cglm/struct.h>

typedef struct {
	vec3s position;
	vec3s target;
	vec3s up;
	float fovy;
	float znear;
	float zfar;
} Camera3D;

#endif // SUSTAIN_CAMERA_H
