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

#ifdef __ORBIS__
#include <orbis/libkernel.h>
#endif

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

#define LOGGER_MAX_BUFFER 0x500

enum LogLevels {
  LL_None,
  LL_Info,
  LL_Warn,
  LL_Error,
  LL_Debug,
  LL_All
};

#ifndef __ORBIS__
typedef struct {
  int type;                 //0x00
  int req_id;               //0x04
  int priority;             //0x08
  int msg_id;               //0x0C
  int target_id;            //0x10
  int user_id;              //0x14
  int unk1;                 //0x18
  int unk2;                 //0x1C
  int app_id;               //0x20
  int error_num;            //0x24
  int unk3;                 //0x28
  char use_icon_image_uri;  //0x2C
  char message[1024];       //0x2D
  char uri[1024];           //0x42D
  char unkstr[1024];        //0x82D
} OrbisNotificationRequest; //Size = 0xC30
#endif

void logSetLogLevel(LogLevels log_level);
LogLevels logGetLogLevel();

const char *_logFormatOutput(LogLevels log_level, bool colorize, const char *function, int32_t line, const char *format, ...);

const char *_logPrettyFunction(const char *prettyFunction);

void _logSystem(LogLevels log_level, const char *function, int32_t line, const char *format, ...);
#define logSystem(log_level, format, ...)                                    \
  do {                                                                       \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);   \
    if (!pretty_function) {                                                  \
      break;                                                                 \
    }                                                                        \
    _logSystem(log_level, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                           \
  } while (0)
void logSystemUnformatted(LogLevels log_level, const char *format, ...);
void logSystemSetLogLevel(LogLevels log_level);
LogLevels logSystemGetLogLevel();

void _logFile(LogLevels log_level, const char *path, const char *function, int32_t line, const char *format, ...);
#define logFile(log_level, file, format, ...)                                    \
  do {                                                                           \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);       \
    if (!pretty_function) {                                                      \
      break;                                                                     \
    }                                                                            \
    _logFile(log_level, file, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                               \
  } while (0)
void logFileUnformatted(LogLevels log_level, const char *path, const char *format, ...);
void logFileSetLogLevel(LogLevels log_level);
LogLevels logFileGetLogLevel();
bool logFileOpen(const char *path);
void logFileClose();
FILE *logFileGetLogFile();

bool logSocketSetIp(const char *ip_address);
const char *logSocketGetIp();
void logSocketSetPort(uint16_t port);
uint16_t logSocketGetPort();

bool logSocketOpen();
void logSocketClose();
void _sendSocket(const char *ip_address, uint16_t port, const char *data, size_t len);
void _logSocket(LogLevels log_level, const char *ip_address, uint16_t port, const char *function, int32_t line, const char *format, ...);
#define logSocket(log_level, ip_address, port, format, ...)                                    \
  do {                                                                                         \
    const char *pretty_function = _logPrettyFunction(__PRETTY_FUNCTION__);                     \
    if (!pretty_function) {                                                                    \
      break;                                                                                   \
    }                                                                                          \
    _logSocket(log_level, ip_address, port, pretty_function, __LINE__, format, ##__VA_ARGS__); \
    free((void *)pretty_function);                                                             \
  } while (0)
void logSocketUnformatted(LogLevels log_level, const char *ip_address, uint16_t port, const char *format, ...);
void logSocketSetLogLevel(LogLevels log_level);
LogLevels logSocketGetLogLevel();

void logNotification(const char *format, ...);
void logNotificationCustomImage(const char *icon_uri, const char *format, ...);
void logNotificationCustom(int device, OrbisNotificationRequest *notification_request, size_t size, bool blocking);

const char *logHexdump(LogLevels log_level, const void *ptr, int len, bool colorize = false);

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

#ifdef __ORBIS__
    syscall(601, 7, log_stream.str().c_str(), 0);
#else
    std::cout << log_stream.str();
#endif
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
