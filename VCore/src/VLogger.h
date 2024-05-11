#pragma once
#include "VCoreDefine.h"
#include "VObject.h"
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <io.h>
#include <iomanip>
#include <iostream>
#include <string>
#include "VString.h"

#define VLOGGER_LOG_ASSERT_NAME "[ASSERT]"
#define VLOGGER_LOG_ERROR_NAME "[ERROR]"
#define VLOGGER_LOG_WARN_NAME "[WARN]"
#define VLOGGER_LOG_INFO_NAME "[INFO]"
#define VLOGGER_LOG_DEBUG_NAME "[DEBUG]"
#define VLOGGER_LOG_VERBOSE_NAME "[VERBOSE]"

#define VLOGGER_LOG_ASSERT_COLOR VCORE_ANSI_COLOR_MAGENTA
#define VLOGGER_LOG_ERROR_COLOR VCORE_ANSI_COLOR_RED
#define VLOGGER_LOG_WARN_COLOR VCORE_ANSI_COLOR_YELLOW
#define VLOGGER_LOG_INFO_COLOR VCORE_ANSI_COLOR_GREEN
#define VLOGGER_LOG_DEBUG_COLOR VCORE_ANSI_COLOR_CYAN
#define VLOGGER_LOG_VERBOSE_COLOR VCORE_ANSI_COLOR_WHITE

enum VLOGGER_TYPE {
  VLOGGER_TYPE_ASSERT = 0x01,
  VLOGGER_TYPE_ERROR = 0x02,
  VLOGGER_TYPE_WARN = 0x04,
  VLOGGER_TYPE_INFO = 0x08,
  VLOGGER_TYPE_DEBUG = 0x10,
  VLOGGER_TYPE_VERBOSE = 0x20,
  VLOGGER_TYPE_ALL = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_ERROR |
                     VLOGGER_TYPE_WARN | VLOGGER_TYPE_INFO |
                     VLOGGER_TYPE_DEBUG | VLOGGER_TYPE_VERBOSE,

  VLOGGER_TYPE_NO_ASSERT = VLOGGER_TYPE_ERROR | VLOGGER_TYPE_WARN |
                           VLOGGER_TYPE_INFO | VLOGGER_TYPE_DEBUG |
                           VLOGGER_TYPE_VERBOSE,
  VLOGGER_TYPE_NO_ERROR = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_WARN |
                          VLOGGER_TYPE_INFO | VLOGGER_TYPE_DEBUG |
                          VLOGGER_TYPE_VERBOSE,
  VLOGGER_TYPE_NO_WARN = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_ERROR |
                         VLOGGER_TYPE_INFO | VLOGGER_TYPE_DEBUG |
                         VLOGGER_TYPE_VERBOSE,
  VLOGGER_TYPE_NO_INFO = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_ERROR |
                         VLOGGER_TYPE_WARN | VLOGGER_TYPE_DEBUG |
                         VLOGGER_TYPE_VERBOSE,
  VLOGGER_TYPE_NO_DEBUG = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_ERROR |
                          VLOGGER_TYPE_WARN | VLOGGER_TYPE_INFO |
                          VLOGGER_TYPE_VERBOSE,
  VLOGGER_TYPE_NO_VERBOSE = VLOGGER_TYPE_ASSERT | VLOGGER_TYPE_ERROR |
                            VLOGGER_TYPE_WARN | VLOGGER_TYPE_INFO |
                            VLOGGER_TYPE_DEBUG
};

#define VLOGGER_DEFINE_LOG_TYPE(Type, TypeName, Name, Colour)                  \
private:                                                                       \
  std::ofstream logfile_##TypeName;                                            \
                                                                               \
public:                                                                        \
  void log##TypeName(const char *format, ...) {                                \
    if ((log_types & Type) == 0) {                                             \
      return;                                                                  \
    }                                                                          \
    va_list args;                                                              \
    va_start(args, format);                                                    \
    int size = _vscprintf(format, args);                                       \
    if (size < 0) {                                                            \
      va_end(args);                                                            \
      return;                                                                  \
    }                                                                          \
    char *buffer = new char[size + 1];                                         \
    vsnprintf_s(buffer, size + 1, _TRUNCATE, format, args);                    \
    std::string filename;                                                      \
    std::string logMessage;                                                    \
    log(logFileDirectory + std::string("/") + #TypeName, logfile_##TypeName,   \
        Name, buffer, Colour, filename, logMessage);                           \
    delete[] buffer;                                                           \
    va_end(args);                                                              \
  }

#define VLOGGER_DEFINE_LOG_TYPE_DEBUG(TypeName)                                \
public:                                                                        \
  inline void log##TypeName(const char *format, ...) {}

#define Log VLogger::defaultVLogger()

class VLogger : public VObject {
public:
  static VLogger *defaultVLogger() {
    static VLogger log;
    return &log;
  }

  // 获取当前日期
  std::string getCurrentDate();

  // 获取当前时间
  std::string getCurrentTime();

  // 获取当前日期时间
  std::string getCurrentDateTime();

  void setEnable(bool eb);

  bool getEnable() const;

  void setLogTypes(VLOGGER_TYPE type);

  VLOGGER_TYPE getLogTypes() const;

private:
  VLOGGER_TYPE log_types;
  VString::Encoding log_encoding;
  std::time_t time_now;
  std::string logFileDirectory;
  bool close_file;
  bool enable;

  void setCloseFile(bool closefile);
  // 获取完整的日志文件路径
  std::string getLogFilePath(const std::string &filename);

  // 获取日期
  std::string getDate(const std::time_t &time);

  // 获取时间
  std::string getTime(const std::time_t &time);

  // 获取日期时间
  std::string getDateTime(const std::time_t &time);

public:
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_ASSERT, Assert,
                          VLOGGER_LOG_ASSERT_NAME, VLOGGER_LOG_ASSERT_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_ERROR, Error,
                          VLOGGER_LOG_ERROR_NAME, VLOGGER_LOG_ERROR_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_WARN, Warn,
                          VLOGGER_LOG_WARN_NAME, VLOGGER_LOG_WARN_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_INFO, Info,
                          VLOGGER_LOG_INFO_NAME, VLOGGER_LOG_INFO_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_VERBOSE, Verbose,
                          VLOGGER_LOG_VERBOSE_NAME, VLOGGER_LOG_VERBOSE_COLOR);
#ifdef _DEBUG
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_DEBUG, Debug,
                          VLOGGER_LOG_DEBUG_NAME, VLOGGER_LOG_DEBUG_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_ASSERT, DebugAssert,
                          VLOGGER_LOG_ASSERT_NAME, VLOGGER_LOG_ASSERT_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_ERROR, DebugError,
                          VLOGGER_LOG_ERROR_NAME, VLOGGER_LOG_ERROR_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_WARN, DebugWarn,
                          VLOGGER_LOG_WARN_NAME, VLOGGER_LOG_WARN_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_INFO, DebugInfo,
                          VLOGGER_LOG_INFO_NAME, VLOGGER_LOG_INFO_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(VLOGGER_TYPE::VLOGGER_TYPE_VERBOSE, DebugVerbose,
                          VLOGGER_LOG_VERBOSE_NAME, VLOGGER_LOG_VERBOSE_COLOR);
#else
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(Debug);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugAssert);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugError);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugWarn);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugInfo);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugVerbose);
#endif // DEBUG

public:
  VLogger();
  ~VLogger();

  // 设置日志文件目录
  void setLogFileDirectory(const std::string &directory);

  void setEncoding(VString::Encoding encoding);

  // 记录日志
  void log(const std::string &filepath, std::ofstream &logfile,
           const std::string &level, const std::string &message,
           const std::string &colour, std::string &filename,
           std::string &logMessage);

  // 关闭日志文件
  void close(std::ofstream &logfile);
};

#undef VLOGGER_DEFINE_LOG_TYPE