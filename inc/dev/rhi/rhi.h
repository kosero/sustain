#ifndef SUSTAIN_RHI_H
#define SUSTAIN_RHI_H

#include "sustain.h"

typedef enum Backend {
	BACKEND_OPENGL,
} Backend;

typedef struct RHI_Ops {
	int (*init)(const char *title, int w, int h);
	void (*clear)(Color color);
	void (*present)(void);
	void (*draw_rect)(int x, int y, int w, int h, Color color);
} RHI_Ops;

int rhi_init(Backend backend, const char *title, int w, int h);
const RHI_Ops *rhi(void);

#endif // SUSTAIN_RHI_H
