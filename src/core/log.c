#include "core/log.h"
#include <stdarg.h>
#include <stdio.h>

void log_printf(LOG_LEVEL level, const char *format, ...)  {
  va_list args;
  va_start(args, format);

  switch (level) {
    case LOG_LEVEL_INFO: printf("[INFO]: "); break;
    case LOG_LEVEL_WARN: printf("[WARN]: "); break;
    case LOG_LEVEL_ERROR: printf("[ERROR]: "); break;
  }

  vprintf(format, args);
  va_end(args);
  printf("\n");
}
