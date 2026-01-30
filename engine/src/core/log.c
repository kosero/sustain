#include "sustain/core/log.h"
#include <stdarg.h>

void SN_Log_Set_Level(SNLogLevel level) {
  SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, (SDL_LogPriority)level);
}

void SN_Log(SNLogLevel level, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, (SDL_LogPriority)level, fmt,
                  ap);
  va_end(ap);
}
