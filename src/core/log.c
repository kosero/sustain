#include "core/log.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(__APPLE__)
#include <time.h>
#include <unistd.h>
#define LOG_HAVE_ISATTY 1
#endif

static LOG_LEVEL log_get_min_level(void)
{
	static int initialized = 0;
	static LOG_LEVEL min_level = LOG_LEVEL_INFO;

	if (!initialized) {
		// NOLINTNEXTLINE(concurrency-mt-unsafe)
		const char *env = getenv("SUSTAIN_LOG_LEVEL");
		if (env != NULL && *env != '\0') {
			char lower[16];
			int i = 0;
			for (; env[i] != '\0' && i < 15; i++) {
				lower[i] = (char)tolower((unsigned char)env[i]);
			}
			lower[i] = '\0';

			if (strcmp(lower, "trace") == 0) {
				min_level = LOG_LEVEL_TRACE;
			} else if (strcmp(lower, "debug") == 0) {
				min_level = LOG_LEVEL_DEBUG;
			} else if (strcmp(lower, "info") == 0) {
				min_level = LOG_LEVEL_INFO;
			} else if (strcmp(lower, "warn") == 0) {
				min_level = LOG_LEVEL_WARN;
			} else if (strcmp(lower, "error") == 0) {
				min_level = LOG_LEVEL_ERROR;
			} else {
				char *end = NULL;
				long value = strtol(env, &end, 10);
				if (end != env && value >= LOG_LEVEL_TRACE &&
				    value <= LOG_LEVEL_ERROR) {
					min_level = (LOG_LEVEL)value;
				}
			}
		}
		initialized = 1;
	}
	return min_level;
}

bool log_level_enabled(LOG_LEVEL level) { return level >= log_get_min_level(); }

static double log_time_ms(void)
{
#ifdef LOG_HAVE_ISATTY
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		return ((double)ts.tv_sec * 1000.0) +
		       ((double)ts.tv_nsec / 1e6);
	}
#endif
	return 0.0;
}

static const char *log_color(LOG_LEVEL level)
{
	switch (level) {
	case LOG_LEVEL_TRACE:
		return "\033[90m";
	case LOG_LEVEL_DEBUG:
		return "\033[36m";
	case LOG_LEVEL_INFO:
		return "\033[32m";
	case LOG_LEVEL_WARN:
		return "\033[33m";
	case LOG_LEVEL_ERROR:
		return "\033[31m";
	}
	return "";
}

static const char *log_level_name(LOG_LEVEL level)
{
	switch (level) {
	case LOG_LEVEL_TRACE:
		return "TRACE";
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	}
	return "?";
}

static bool log_use_color(void)
{
#ifdef LOG_HAVE_ISATTY
	return (bool)(isatty(STDERR_FILENO) || isatty(STDOUT_FILENO));
#else
	return false;
#endif
}

int log_printf(LOG_LEVEL level, const char *format, ...)
{
	if (!log_level_enabled(level)) {
		return 0;
	}

	int err = 0;

	if (log_use_color()) {
		err |= fprintf(stderr, "%s[%7.1f] [%s] ", log_color(level),
			       log_time_ms(), log_level_name(level));
	} else {
		err |= fprintf(stderr, "[%7.1f] [%s] ", log_time_ms(),
			       log_level_name(level));
	}

	va_list args = {0};
	va_start(args, format);
	err |= vfprintf(stderr, format, args);
	va_end(args);

	if (log_use_color()) {
		err |= fputs("\033[0m\n", stderr);
	} else {
		err |= fputc('\n', stderr);
	}
	return err < 0 ? -1 : 0;
}
