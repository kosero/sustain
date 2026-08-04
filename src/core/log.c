#include "core/log.h"
#include <stdarg.h>
#include <stdio.h>

int log_printf(LOG_LEVEL level, const char *format, ...)
{
	FILE *out = stdout;
	const char *prefix = "[INFO]: ";
	int err = 0;

	switch (level) {
	case LOG_LEVEL_WARN:
		out = stderr;
		prefix = "[WARN]: ";
		break;
	case LOG_LEVEL_ERROR:
		out = stderr;
		prefix = "[ERROR]: ";
		break;
	case LOG_LEVEL_INFO:
		break;
	}

	if (fputs(prefix, out) == EOF) {
		err = -1;
	}

	va_list args;
	va_start(args, format);
	if (vfprintf(out, format, args) < 0) {
		err = -1;
	}
	va_end(args);

	if (fputs("\n", out) == EOF) {
		err = -1;
	}
	return err;
}
