#ifndef SUSTAIN_LOG_H
#define SUSTAIN_LOG_H

typedef enum {
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
} LOG_LEVEL;

void log_printf(LOG_LEVEL level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#endif // SUSTAIN_LOG_H
