// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// License: GPL-3.0 License

#ifdef __cplusplus
#include <sstream>
#endif

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifdef __ORBIS__
#include <orbis/libkernel.h>
#endif

#include "libLog.h"

// Globals
LogLevels g_log_level = LL_All;
int g_socket = -1;
FILE *g_log_file;
char g_socket_log_ip[16];
uint16_t g_socket_log_port;

LogLevels g_system_log_level = LL_None;
LogLevels g_socket_log_level = LL_None;
LogLevels g_file_log_level = LL_None;

inline static const char *strlaststr(const char *haystack, const char *needle) {
  char *loc = 0;
  char *found = 0;
  size_t pos = 0;

  while ((found = strstr(haystack + pos, needle)) != 0) {
    loc = found;
    pos = (found - haystack) + 1;
  }

  return loc;
}

// https://www.tutorialspoint.com/c-program-to-validate-an-ip-address
inline static bool validate_number(char *str) {
  while (*str) {
    if (!isdigit(*str)) {
      return false;
    }
    str++;
  }

  return true;
}

// https://www.tutorialspoint.com/c-program-to-validate-an-ip-address
inline static bool validate_ip(const char *ip) {
  char *temp_ip = strdup(ip);

  int dots = 0;

  char *ptr;
  ptr = strtok(temp_ip, ".");
  if (!ptr) {
    free(temp_ip);
    return false;
  }

  while (ptr) {
    if (!validate_number(ptr)) {
      free(temp_ip);
      return false;
    }
    int num = atoi(ptr); // Flawfinder: ignore. Value is checked below
    if (num >= 0 && num <= 255) {
      ptr = strtok(NULL, ".");
      if (ptr) {
        dots++;
      }
    } else {
      free(temp_ip);
      return false;
    }
  }
  if (dots != 3) {
    free(temp_ip);
    return false;
  }

  return true;
}

void logSetLogLevel(LogLevels log_level) {
  g_log_level = log_level;
}

LogLevels logGetLogLevel() {
  return g_log_level;
}

const char *_logFormatOutput(LogLevels log_level, bool colorize, const char *function, int32_t line, const char *format, ...) {
  const char *level_string = "None";
  const char *level_color = KNRM;

  switch (log_level) {
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

  char *buffer = (char *)malloc(LOGGER_MAX_BUFFER);
  if (!buffer) {
    return NULL;
  }

  char *final_buffer = (char *)malloc(LOGGER_MAX_BUFFER);
  if (!final_buffer) {
    free(buffer);
    return NULL;
  }

  memset(buffer, '\0', LOGGER_MAX_BUFFER);
  memset(final_buffer, '\0', LOGGER_MAX_BUFFER);

  va_list args;
  va_start(args, format);

  vsnprintf(buffer, LOGGER_MAX_BUFFER - 1, format, args);

  if (colorize) {
    snprintf(final_buffer, LOGGER_MAX_BUFFER - 1, "%s[%s] %s:%d : %s %s\n", level_color, level_string, function, line, buffer, KNRM);
  } else {
    snprintf(final_buffer, LOGGER_MAX_BUFFER - 1, "[%s] %s:%d : %s\n", level_string, function, line, buffer);
  }

  free(buffer);

  return final_buffer;
}

const char *_logPrettyFunction(const char *prettyFunction) {
  const char *temp_pos = strlaststr(prettyFunction, " ");
  size_t pos = temp_pos - prettyFunction + 1;

  size_t len = strlen(prettyFunction) - pos - 1;
  char *output_buffer = (char *)malloc(len);
  if (!output_buffer) {
    return NULL;
  }
  memset(output_buffer, '\0', len);
  memcpy(output_buffer, prettyFunction + pos, len - 1); // Copy all but last char as it should be null term

  return output_buffer;
}

void _logSystem(LogLevels log_level, const char *function, int32_t line, const char *format, ...) {
  if (g_log_level <= LL_None) {
    return;
  }

  if (log_level > g_log_level) {
    return;
  }

  va_list args;
  va_start(args, format);

  const char *formatted_output = _logFormatOutput(log_level, true, function, line, format, args);

  if (!formatted_output) {
    return;
  }

#ifdef __ORBIS__
  syscall(601, 7, _logFormatOutput(log_level, true, function, line, format, args), 0);
#else
  printf("%s", formatted_output);
#endif

  free((void *)formatted_output);
}

void logSystemUnformatted(LogLevels log_level, const char *format, ...) {
  if (g_log_level <= LL_None) {
    return;
  }

  if (log_level > g_log_level) {
    return;
  }

  char *buffer = (char *)malloc(LOGGER_MAX_BUFFER);
  if (!buffer) {
    return;
  }

  memset(buffer, '\0', LOGGER_MAX_BUFFER);

  va_list args;
  va_start(args, format);

  vsnprintf(buffer, LOGGER_MAX_BUFFER - 1, format, args);

#ifdef __ORBIS__
  syscall(601, 7, buffer, 0);
#else
  printf("%s", buffer);
#endif
  free((void *)buffer);
}

void logSystemSetLogLevel(LogLevels log_level) {
  g_system_log_level = log_level;
}

LogLevels logSystemGetLogLevel() {
  return g_system_log_level;
}

void _logFile(LogLevels log_level, const char *path, const char *function, int32_t line, const char *format, ...) {
  if (g_log_level <= LL_None) {
    return;
  }

  if (log_level > g_log_level) {
    return;
  }

  va_list args;
  va_start(args, format);

  const char *formatted_output = _logFormatOutput(log_level, false, function, line, format, args);

  if (!formatted_output) {
    return;
  }

  FILE *fp = fopen(path, "a");
  if (!fp) {
    free((void *)formatted_output);
    return;
  }
  fprintf(fp, "%s", formatted_output);
  fclose(fp);

  free((void *)formatted_output);
}

void logFileUnformatted(LogLevels log_level, const char *path, const char *format, ...) {
  if (g_log_level <= LL_None) {
    return;
  }

  if (log_level > g_log_level) {
    return;
  }

  char *buffer = (char *)malloc(LOGGER_MAX_BUFFER);
  if (!buffer) {
    return;
  }

  memset(buffer, '\0', LOGGER_MAX_BUFFER);

  va_list args;
  va_start(args, format);

  vsnprintf(buffer, LOGGER_MAX_BUFFER - 1, format, args);

  FILE *fp = fopen(path, "a");
  if (!fp) {
    free((void *)buffer);
    return;
  }
  fprintf(fp, "%s", buffer);
  fclose(fp);

  free((void *)buffer);
}

void logFileSetLogLevel(LogLevels log_level) {
  g_file_log_level = log_level;
}

LogLevels logFileGetLogLevel() {
  return g_file_log_level;
}

bool logFileOpen(const char *path) {
  if (g_log_file) {
    // We should either close the currently open file and open the new file, or return false to "fail"
    return false; // or logFileClose();
  }

  g_log_file = fopen(path, "a");

  if (!g_log_file) {
    return false;
  }

  return true;
}

void logFileClose() {
  if (!g_log_file) {
    return;
  }

  fclose(g_log_file);
  g_log_file = NULL;
}

FILE *logFileGetLogFile() {
  return g_log_file;
}

bool logSocketSetIp(const char *ip_address) {
  if (!validate_ip(ip_address)) {
    return false;
  }

  memset(g_socket_log_ip, '\0', sizeof(g_socket_log_ip));
  memcpy(g_socket_log_ip, ip_address, strnlen(ip_address, sizeof(g_socket_log_ip) - 1));

  return true;
}

const char *logSocketGetIp() {
  return g_socket_log_ip;
}

void logSocketSetPort(uint16_t port) {
  g_socket_log_port = port;
}

uint16_t logSocketGetPort() {
  return g_socket_log_port;
}

bool logSocketOpen() {
  if (g_socket < 0) {
    if ((g_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return false;
    }
  }

  return true;
}

void logSocketClose() {
  if (g_socket < 0) {
    return;
  }

  close(g_socket);
  g_socket = -1;
}

void _sendSocket(const char *ip_address, uint16_t port, const char *data, size_t len) {
  struct sockaddr_in servaddr;
  memset(&servaddr, '\0', sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ip_address);
  servaddr.sin_port = htons(port);

  sendto(g_socket, data, len, 0, (sockaddr *)&servaddr, sizeof(servaddr));
}

void _logSocket(LogLevels log_level, const char *ip_address, uint16_t port, const char *function, int32_t line, const char *format, ...) {
  if (g_socket < 0) {
    return;
  }

  if (!ip_address) {
    return;
  }

  va_list args;
  va_start(args, format);

  const char *formatted_output = _logFormatOutput(log_level, true, function, line, format, args);

  if (!formatted_output) {
    return;
  }

  _sendSocket(ip_address, port, formatted_output, strnlen(formatted_output, LOGGER_MAX_BUFFER));
  free((void *)formatted_output);
}

void logSocketUnformatted(LogLevels log_level, const char *ip_address, uint16_t port, const char *format, ...) {
  if (g_log_level <= LL_None) {
    return;
  }

  if (log_level > g_log_level) {
    return;
  }

  if (g_socket < 0) {
    return;
  }

  if (!ip_address) {
    return;
  }

  char *buffer = (char *)malloc(LOGGER_MAX_BUFFER);
  if (!buffer) {
    return;
  }

  memset(buffer, '\0', LOGGER_MAX_BUFFER);

  va_list args;
  va_start(args, format);

  vsnprintf(buffer, LOGGER_MAX_BUFFER - 1, format, args);

  _sendSocket(ip_address, port, buffer, strnlen(buffer, LOGGER_MAX_BUFFER));
  free((void *)buffer);
}

void logSocketSetLogLevel(LogLevels log_level) {
  g_socket_log_level = log_level;
}

LogLevels logSocketGetLogLevel() {
  return g_socket_log_level;
}

void logNotification(const char *format, ...) {
  OrbisNotificationRequest notification_request;
  memset(&notification_request, '\0', sizeof(notification_request));

  const char icon_uri[38] = "cxml://psnotification/tex_icon_system"; // Flawfinder: ignore. It's statically size but it's only used in this function, and it's size it is used
  notification_request.type = 0;
  notification_request.unk3 = 0;
  notification_request.use_icon_image_uri = 1;
  notification_request.target_id = -1;
  memcpy(notification_request.uri, icon_uri, sizeof(icon_uri)); // Flawfinder: ignore. icon_uri has a fixed size here and is smaller than notification_request.uri

  va_list args;
  va_start(args, format);

  snprintf(notification_request.message, sizeof(notification_request.message), format, args);

#ifdef __ORBIS__
  sceKernelSendNotificationRequest(0, &notification_request, sizeof(notification_request), 0);
#endif
}

void logNotificationCustomImage(const char *icon_uri, const char *format, ...) {
  OrbisNotificationRequest notification_request;
  memset(&notification_request, '\0', sizeof(notification_request));

  notification_request.type = 0;
  notification_request.unk3 = 0;
  notification_request.use_icon_image_uri = 1;
  notification_request.target_id = -1;
  if (strnlen(icon_uri, sizeof(notification_request.uri)) + 1 <= sizeof(notification_request.uri)) { // This is just a check to make sure it's smaller than, or equal to, the buffer it's being moved to
    memcpy(notification_request.uri, icon_uri, strlen(icon_uri) + 1);                                // Flawfinder: ignore. Size is checked above
  } else {
    const char default_icon_uri[38] = "cxml://psnotification/tex_icon_system";    // Flawfinder: ignore. It's statically size but it's only used in the next line, and it's size it is used
    memcpy(notification_request.uri, default_icon_uri, sizeof(default_icon_uri)); // Flawfinder: ignore. default_icon_uri has a fixed size here and is smaller than notification_request.uri
  }

  va_list args;
  va_start(args, format);

  snprintf(notification_request.message, sizeof(notification_request.message), format, args);

#ifdef __ORBIS__
  sceKernelSendNotificationRequest(0, &notification_request, sizeof(notification_request), 0);
#endif
}

void logNotificationCustom(int device, OrbisNotificationRequest *notification_request, size_t size, bool blocking) {
#ifdef __ORBIS__
  sceKernelSendNotificationRequest(device, notification_request, size, blocking);
#else
  (void)device;
  (void)notification_request;
  (void)size;
  (void)blocking;
#endif
}

inline static int _logHexdumpSize(int input) {
  int lines = input / 0x10;
  if (lines == 0) {
    lines++;
  }

  int remain = input % 0x10;
  if (remain > 0) {
    lines++;
  }

  int total = lines * 77 + 1;
  if (remain != 0) {
    total -= 0x10 - remain;
  }

  return total + 10; // +10 so there is space for the colorization option
}

// Hexdump based on: https://stackoverflow.com/a/29865
const char *logHexdump(LogLevels log_level, const void *ptr, int len, bool colorize) {
  unsigned char *buf = (unsigned char *)ptr;
  char *ret = (char *)malloc(_logHexdumpSize(len));
  if (!ret) {
    return NULL;
  }
  memset(ret, '\0', _logHexdumpSize(len));
  int offset = 0;

  if (colorize) {
    switch (log_level) {
    case LL_Info:
      memcpy(ret + offset, KGRN, sizeof(KGRN) - 1);
      offset += sizeof(KGRN) - 1;
      break;
    case LL_Warn:
      memcpy(ret + offset, KYEL, sizeof(KYEL) - 1);
      offset += sizeof(KYEL) - 1;
      break;
    case LL_Error:
      memcpy(ret + offset, KRED, sizeof(KRED) - 1);
      offset += sizeof(KRED) - 1;
      break;
    case LL_Debug:
      memcpy(ret + offset, KGRY, sizeof(KGRY) - 1);
      offset += sizeof(KGRY) - 1;
      break;
    case LL_None:
    default:
      memcpy(ret + offset, KNRM, sizeof(KNRM) - 1);
      offset += sizeof(KNRM) - 1;
      break;
    }
  }

  for (int i = 0; i < len; i += 0x10) {
    sprintf(ret + offset, "%08X: ", i);
    offset += 10;
    for (int j = 0; j < 0x10; j++) {
      if (i + j < len) {
        sprintf(ret + offset, "%02X ", buf[i + j]);
        offset += 3;
      } else {
        memcpy(ret + offset, "   ", 3);
        offset += 3;
      }
      if (j == 7) {
        memcpy(ret + offset, " ", 1);
        offset++;
      }
    }

    memcpy(ret + offset, " ", 1);
    offset++;

    for (int j = 0; j < 0x10; j++) {
      if (i + j < len) {
        if (isprint(buf[i + j])) {
          sprintf(ret + offset, "%c", buf[i + j]);
          offset++;
        } else {
          memcpy(ret + offset, ".", 1);
          offset++;
        }
      }
    }

    if (i + 0x10 < len) {
      memcpy(ret + offset, "\n", 1);
      offset++;
    }
  }
  if (colorize) {
    memcpy(ret + offset, KNRM, sizeof(KNRM) - 1);
    offset += sizeof(KNRM) - 1;
  }

  memcpy(ret + offset, "\n", 1);

  return ret;
}

#ifndef __ORBIS__
int main() {
  logSystem(LL_Error, "This is a logSystem call");
  logSystemUnformatted(LL_Error, "This is a logSystemUnformatted call\n"); // Completely unformatted
  SYSTEMLOG << "This is a SYSTEMLOG call";                                 // C++ style, can set a log level with `logSystemSetLogLevel(LL_Info);`
                                                                           // Subsequent will continute to use the same level unless it is changed again
  SYSTEMLOG_UNFORMATTED << "This is a SYSTEMLOG_UNFORMATTED call\n";       // C++ style, completely unformatted

  logFile(LL_Error, "./log.txt", "This is a logFile call");
  logFileUnformatted(LL_Error, "./log.txt", "This is a logFileUnformatted call\n"); // Completely unformatted
  logFileOpen("./log.txt");
  FILELOG << "This is a FILELOG call";                           // C++ style, can set a log level with `logFileSetLogLevel(LL_Info);`
                                                                 // Subsequent will continute to use the same level unless it is changed again
  FILELOG_UNFORMATTED << "This is a FILELOG_UNFORMATTED call\n"; // C++ style, completely unformatted
  logFileClose();

  logSocketOpen();
  logSocket(LL_Error, "127.0.0.1", 9023, "This is a logSocket call");
  logSocketUnformatted(LL_Error, "127.0.0.1", 9023, "This is a logSocketUnformatted call\n"); // Completely unformatted
  logSocketSetIp("127.0.0.1");
  logSocketSetPort(9023);
  SOCKETLOG << "This is a SOCKETLOG call";                           // C++ style, can set a log level with `logSocketSetLogLevel(LL_Info);`
                                                                     // Subsequent will continute to use the same level unless it is changed again
  SOCKETLOG_UNFORMATTED << "This is a SOCKETLOG_UNFORMATTED call\n"; // C++ style, completely unformatted
  logSocketClose();

  char test[0x20]; // Flawfinder: ignore, only used for testing
  memset(test, 0xCC, sizeof(test));

  logHexdumpSystem(LL_Error, test, sizeof(test));
  logHexdumpSystemUnformatted(LL_Error, test, sizeof(test));
  logHexdumpFile(LL_Error, "./log.txt", test, sizeof(test));
  logSocketOpen();
  logHexdumpSocket(LL_Error, "127.0.0.1", 9023, test, sizeof(test));
  logHexdumpSocketUnformatted(LL_Error, "127.0.0.1", 9023, test, sizeof(test));
  logSocketClose();

  return 0;
}
#endif
