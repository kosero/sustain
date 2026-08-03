#ifndef SUSTAIN_LOG_H
#define SUSTAIN_LOG_H

typedef enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
} LOG_LEVEL;

#if defined(__GNUC__) || defined(__clang__)
    #define PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
#else
    #define PRINTF_FORMAT(fmt, args)
#endif

void log_printf(LOG_LEVEL level, const char *format, ...) PRINTF_FORMAT(2, 3);

#endif // SUSTAIN_LOG_H
