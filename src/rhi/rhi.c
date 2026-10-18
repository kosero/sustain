#include "dev/rhi/rhi.h"
#include "dev/rhi/opengl/opengl.h"

typedef struct RHI_Context {
	const RHI_Ops *ops;
} RHI_Context;

static RHI_Context *rhi_context(void)
{
	static RHI_Context ctx;
	return &ctx;
}

int rhi_init(Backend backend, const char *title, int w, int h)
{
	RHI_Context *ctx = rhi_context();

	switch (backend) {
	case BACKEND_OPENGL:
		ctx->ops = &gl_ops;
		break;
	default:
		return -1;
	}

	return ctx->ops->init(title, w, h);
}

const RHI_Ops *rhi(void) { return rhi_context()->ops; }
