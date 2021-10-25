// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// License: GPL-3.0 License

#ifndef LIBLOG_H
#define LIBLOG_H

#ifdef __cplusplus
#include <iostream>
#include <sstream>
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <orbis/libkernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
#define KGRY "\x1b[90m"

enum LogLevels {
  LL_None,
  LL_Info,
  LL_Warn,
  LL_Error,
  LL_Debug,
  LL_All
};

void (*logSetLogLevel)(LogLevels log_level) = NULL;
LogLevels (*logGetLogLevel)() = NULL;

const char *(*_logFormatOutput)(LogLevels log_level, bool colorize, const char *function, int32_t line, const char *format, ...) = NULL;

const char *(*_logPrettyFunction)(const char *prettyFunction) = NULL;

void (*_logSystem)(LogLevels log_level, const char *function, int32_t line, const char *format, ...) = NULL;
#define logSystem(log_level, format, ...)                                    \
  do {                                                                       \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);   \
    if (!pretty_function) {                                                  \
      break;                                                                 \
    }                                                                        \
    _logSystem(log_level, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                           \
  } while (0)
void (*logSystemUnformatted)(LogLevels log_level, const char *format, ...) = NULL;
void (*logSystemSetLogLevel)(LogLevels log_level) = NULL;
LogLevels (*logSystemGetLogLevel)() = NULL;

void (*_logFile)(LogLevels log_level, const char *path, const char *function, int32_t line, const char *format, ...) = NULL;
#define logFile(log_level, file, format, ...)                                    \
  do {                                                                           \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);       \
    if (!pretty_function) {                                                      \
      break;                                                                     \
    }                                                                            \
    _logFile(log_level, file, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                               \
  } while (0)
void (*logFileUnformatted)(LogLevels log_level, const char *path, const char *format, ...) = NULL;
void (*logFileSetLogLevel)(LogLevels log_level) = NULL;
LogLevels (*logFileGetLogLevel)() = NULL;
bool (*logFileOpen)(const char *path) = NULL;
void (*logFileClose)() = NULL;
FILE *(*logFileGetLogFile)() = NULL;

bool (*logSocketSetIp)(const char *ip_address) = NULL;
const char *(*logSocketGetIp)() = NULL;
void (*logSocketSetPort)(uint16_t port) = NULL;
uint16_t (*logSocketGetPort)() = NULL;

bool (*logSocketOpen)() = NULL;
void (*logSocketClose)() = NULL;
void (*_sendSocket)(const char *ip_address, uint16_t port, const char *data, size_t len) = NULL;
void (*_logSocket)(LogLevels log_level, const char *ip_address, uint16_t port, const char *function, int32_t line, const char *format, ...) = NULL;
#define logSocket(log_level, ip_address, port, format, ...)                                    \
  do {                                                                                         \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);                     \
    if (!pretty_function) {                                                                    \
      break;                                                                                   \
    }                                                                                          \
    _logSocket(log_level, ip_address, port, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                                             \
  } while (0)
void (*logSocketUnformatted)(LogLevels log_level, const char *ip_address, uint16_t port, const char *format, ...) = NULL;
void (*logSocketSetLogLevel)(LogLevels log_level) = NULL;
LogLevels (*logSocketGetLogLevel)() = NULL;

void (*logNotification)(const char *format, ...) = NULL;
void (*logNotificationCustomImage)(const char *icon_uri, const char *format, ...) = NULL;
void (*logNotificationCustom)(int device, OrbisNotificationRequest *notification_request, size_t size, bool blocking) = NULL;

const char *(*logHexdump)(LogLevels log_level, const void *ptr, int len, bool colorize) = NULL;

#define logHexdumpSystem(log_level, ptr, len)                          \
  do {                                                                 \
    const char *formatted_hex = logHexdump(log_level, ptr, len, true); \
    if (!formatted_hex) {                                              \
      break;                                                           \
    }                                                                  \
    logSystemUnformatted(log_level, formatted_hex);                    \
    free((void *)formatted_hex);                                       \
  } while (0)
#define logHexdumpSystemUnformatted(log_level, ptr, len)                \
  do {                                                                  \
    const char *formatted_hex = logHexdump(log_level, ptr, len, false); \
    if (!formatted_hex) {                                               \
      break;                                                            \
    }                                                                   \
    logSystemUnformatted(log_level, formatted_hex);                     \
    free((void *)formatted_hex);                                        \
  } while (0)
#define logHexdumpFile(log_level, path, ptr, len)                       \
  do {                                                                  \
    const char *formatted_hex = logHexdump(log_level, ptr, len, false); \
    if (!formatted_hex) {                                               \
      break;                                                            \
    }                                                                   \
    logFileUnformatted(log_level, path, formatted_hex);                 \
    free((void *)formatted_hex);                                        \
  } while (0)
#define logHexdumpSocket(log_level, ip_address, port, ptr, len)        \
  do {                                                                 \
    const char *formatted_hex = logHexdump(log_level, ptr, len, true); \
    if (!formatted_hex) {                                              \
      break;                                                           \
    }                                                                  \
    logSocketUnformatted(log_level, ip_address, port, formatted_hex);  \
    free((void *)formatted_hex);                                       \
  } while (0)
#define logHexdumpSocketUnformatted(log_level, ip_address, port, ptr, len) \
  do {                                                                     \
    const char *formatted_hex = logHexdump(log_level, ptr, len, false);    \
    if (!formatted_hex) {                                                  \
      break;                                                               \
    }                                                                      \
    logSocketUnformatted(log_level, ip_address, port, formatted_hex);      \
    free((void *)formatted_hex);                                           \
  } while (0)

int logInitalize(int handle) {
  if (
      sceKernelDlsym(handle, "logSetLogLevel", (void **)&logSetLogLevel) != 0 ||
      sceKernelDlsym(handle, "logGetLogLevel", (void **)&logGetLogLevel) != 0 ||

      sceKernelDlsym(handle, "_logFormatOutput", (void **)&_logFormatOutput) != 0 ||
      sceKernelDlsym(handle, "_logPrettyFunction", (void **)&_logPrettyFunction) != 0 ||

      sceKernelDlsym(handle, "_logSystem", (void **)&_logSystem) != 0 ||
      sceKernelDlsym(handle, "logSystemUnformatted", (void **)&logSystemUnformatted) != 0 ||
      sceKernelDlsym(handle, "logSystemSetLogLevel", (void **)&logSystemSetLogLevel) != 0 ||
      sceKernelDlsym(handle, "logSystemGetLogLevel", (void **)&logSystemGetLogLevel) != 0 ||

      sceKernelDlsym(handle, "_logFile", (void **)&_logFile) != 0 ||
      sceKernelDlsym(handle, "logFileUnformatted", (void **)&logFileUnformatted) != 0 ||
      sceKernelDlsym(handle, "logFileSetLogLevel", (void **)&logFileSetLogLevel) != 0 ||
      sceKernelDlsym(handle, "logFileGetLogLevel", (void **)&logFileGetLogLevel) != 0 ||
      sceKernelDlsym(handle, "logFileOpen", (void **)&logFileOpen) != 0 ||
      sceKernelDlsym(handle, "logFileClose", (void **)&logFileClose) != 0 ||
      sceKernelDlsym(handle, "logFileGetLogFile", (void **)&logFileGetLogFile) != 0 ||

      sceKernelDlsym(handle, "logSocketSetIp", (void **)&logSocketSetIp) != 0 ||
      sceKernelDlsym(handle, "logSocketGetIp", (void **)&logSocketGetIp) != 0 ||
      sceKernelDlsym(handle, "logSocketSetPort", (void **)&logSocketSetPort) != 0 ||
      sceKernelDlsym(handle, "logSocketGetPort", (void **)&logSocketGetPort) != 0 ||

      sceKernelDlsym(handle, "logSocketOpen", (void **)&logSocketOpen) != 0 ||
      sceKernelDlsym(handle, "logSocketClose", (void **)&logSocketClose) != 0 ||
      sceKernelDlsym(handle, "_sendSocket", (void **)&_sendSocket) != 0 ||
      sceKernelDlsym(handle, "_logSocket", (void **)&_logSocket) != 0 ||
      sceKernelDlsym(handle, "logSocketUnformatted", (void **)&logSocketUnformatted) != 0 ||
      sceKernelDlsym(handle, "logSocketSetLogLevel", (void **)&logSocketSetLogLevel) != 0 ||
      sceKernelDlsym(handle, "logSocketGetLogLevel", (void **)&logSocketGetLogLevel) != 0 ||

      sceKernelDlsym(handle, "logNotification", (void **)&logNotification) != 0 ||
      sceKernelDlsym(handle, "logNotificationCustomImage", (void **)&logNotificationCustomImage) != 0 ||
      sceKernelDlsym(handle, "logNotificationCustom", (void **)&logNotificationCustom) != 0 ||

      sceKernelDlsym(handle, "logHexdump", (void **)&logHexdump) != 0) {
    return -1;
  }

  return 0;
}

#ifdef __cplusplus
}

class SystemLog {
public:
  SystemLog(bool formatted, const char *function, int32_t line) {
    do_format = formatted;
    if (do_format) {
      if (logGetLogLevel() <= LL_None) {
        skip = true;
        return;
      }

      if (logSystemGetLogLevel() > logGetLogLevel()) {
        skip = true;
        return;
      }

      const char *level_string = "None";
      const char *level_color = KNRM;

      switch (logSystemGetLogLevel()) {
      case LL_Info:
        level_string = "Info";
        level_color = KGRN;
        break;
      case LL_Warn:
        level_string = "Warn";
        level_color = KYEL;
        break;
      case LL_Error:
        level_string = "Error";
        level_color = KRED;
        break;
      case LL_Debug:
        level_string = "Debug";
        level_color = KGRY;
        break;
      case LL_None:
      default:
        level_string = "None";
        level_color = KNRM;
        break;
      }
      log_stream << level_color << "[" << level_string << "] " << _logPrettyFunction(function) << ":" << line << " : ";
    }
  }

  template <class T>
  SystemLog &operator<<(const T &v) {
    log_stream << v;
    return *this;
  }

  ~SystemLog() {
    if (do_format) {
      if (skip) {
        return;
      }
      log_stream << KNRM << std::endl;
    }

    syscall(601, 7, log_stream.str().c_str(), 0);
    log_stream.str("");
  }

private:
  bool skip = false;
  bool do_format = false;
  std::stringstream log_stream;
};

class FileLog {
public:
  FileLog(bool formatted, const char *function, int32_t line) {
    do_format = formatted;
    if (do_format) {
      if (logGetLogLevel() <= LL_None) {
        skip = true;
        return;
      }

      if (logFileGetLogLevel() > logGetLogLevel()) {
        skip = true;
        return;
      }

      const char *level_string = "None";

      switch (logFileGetLogLevel()) {
      case LL_Info:
        level_string = "Info";
        break;
      case LL_Warn:
        level_string = "Warn";
        break;
      case LL_Error:
        level_string = "Error";
        break;
      case LL_Debug:
        level_string = "Debug";
        break;
      case LL_None:
      default:
        level_string = "None";
        break;
      }
      log_stream << "[" << level_string << "] " << _logPrettyFunction(function) << ":" << line << " : ";
    }
  }

  template <class T>
  FileLog &operator<<(const T &v) {
    log_stream << v;
    return *this;
  }

  ~FileLog() {
    if (!logFileGetLogFile()) {
      return;
    }
    if (do_format) {
      if (skip) {
        return;
      }
      log_stream << std::endl;
    }
    fprintf(logFileGetLogFile(), "%s", log_stream.str().c_str());
    log_stream.str("");
  }

private:
  bool skip = false;
  bool do_format = false;
  std::stringstream log_stream;
};

class SocketLog {
public:
  SocketLog(bool formatted, const char *function, int32_t line) {
    do_format = formatted;
    if (do_format) {
      if (logGetLogLevel() <= LL_None) {
        skip = true;
        return;
      }

      if (logSocketGetLogLevel() > logGetLogLevel()) {
        skip = true;
        return;
      }
      const char *level_string = "None";
      const char *level_color = KNRM;

      switch (logSocketGetLogLevel()) {
      case LL_Info:
        level_string = "Info";
        level_color = KGRN;
        break;
      case LL_Warn:
        level_string = "Warn";
        level_color = KYEL;
        break;
      case LL_Error:
        level_string = "Error";
        level_color = KRED;
        break;
      case LL_Debug:
        level_string = "Debug";
        level_color = KGRY;
        break;
      case LL_None:
      default:
        level_string = "None";
        level_color = KNRM;
        break;
      }
      log_stream << level_color << "[" << level_string << "] " << _logPrettyFunction(function) << ":" << line << " : ";
    }
  }

  template <class T>
  SocketLog &operator<<(const T &v) {
    log_stream << v;
    return *this;
  }

  ~SocketLog() {
    if (do_format) {
      if (skip) {
        return;
      }
      log_stream << KNRM << std::endl;
    }
    _sendSocket(logSocketGetIp(), logSocketGetPort(), log_stream.str().c_str(), log_stream.str().size());
    log_stream.str("");
  }

private:
  bool skip = false;
  bool do_format = false;
  std::stringstream log_stream;
};

#define SYSTEMLOG SystemLog(true, __PRETTY_FUNCTION__, __LINE__)
#define FILELOG FileLog(true, __PRETTY_FUNCTION__, __LINE__)
#define SOCKETLOG SocketLog(true, __PRETTY_FUNCTION__, __LINE__)
#define SYSTEMLOG_UNFORMATTED SystemLog(false, __PRETTY_FUNCTION__, __LINE__)
#define FILELOG_UNFORMATTED FileLog(false, __PRETTY_FUNCTION__, __LINE__)
#define SOCKETLOG_UNFORMATTED SocketLog(false, __PRETTY_FUNCTION__, __LINE__)

#endif

#endif
