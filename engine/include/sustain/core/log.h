#ifndef SUSTAIN_CORE_LOG_H
#define SUSTAIN_CORE_LOG_H

#include <SDL3/SDL.h>

typedef enum SNLogLevel {
  SN_LOG_DEBUG = SDL_LOG_PRIORITY_DEBUG,
  SN_LOG_INFO = SDL_LOG_PRIORITY_INFO,
  SN_LOG_WARN = SDL_LOG_PRIORITY_WARN,
  SN_LOG_ERROR = SDL_LOG_PRIORITY_ERROR
} SNLogLevel;

void SN_Log_Set_Level(SNLogLevel level);
void SN_Log(SNLogLevel level, const char *fmt, ...);

#define SLOG_DEBUG(...) SN_Log(SN_LOG_DEBUG, __VA_ARGS__)
#define SLOG_INFO(...) SN_Log(SN_LOG_INFO, __VA_ARGS__)
#define SLOG_WARN(...) SN_Log(SN_LOG_WARN, __VA_ARGS__)
#define SLOG_ERROR(...) SN_Log(SN_LOG_ERROR, __VA_ARGS__)

#endif
