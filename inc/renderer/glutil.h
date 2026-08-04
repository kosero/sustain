#ifndef SUSTAIN_GLUTIL_H
#define SUSTAIN_GLUTIL_H

#include <stddef.h>
#include <stdint.h>

static inline const void *gl_attrib_offset(size_t bytes)
{
	// NOLINTNEXTLINE(performance-no-int-to-ptr)
	return (const void *)(uintptr_t)bytes;
}

#endif // SUSTAIN_GLUTIL_H
