#ifndef SUSTAIN_LOG_H
#define SUSTAIN_LOG_H

#include <stdbool.h>

typedef enum {
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
} LOG_LEVEL;

#if defined(__GNUC__) || defined(__clang__)
#define PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define PRINTF_FORMAT(fmt, args)
#endif

bool log_level_enabled(LOG_LEVEL level);

int log_printf(LOG_LEVEL level, const char *format, ...) PRINTF_FORMAT(2, 3);

#define LOG_TRACE(...) log_printf(LOG_LEVEL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) log_printf(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) log_printf(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log_printf(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) log_printf(LOG_LEVEL_ERROR, __VA_ARGS__)

#endif // SUSTAIN_LOG_H
