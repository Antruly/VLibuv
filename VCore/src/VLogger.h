#pragma once
#include <direct.h>
#include <io.h>
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "VCoreDefine.h"
#include "VObject.h"

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

#define VLOGGER_DEFINE_LOG_TYPE(Type, Name, Colour)                         \
private:                                                                    \
    std::ofstream logfile_##Type;                                          \
                                                                            \
public:                                                                     \
    void log##Type(const char* format, ...) {                              \
        va_list args;                                                      \
        va_start(args, format);                                            \
        int size = _vscprintf(format, args);                               \
        if (size < 0) {                                                    \
            va_end(args);                                                  \
            return;                                                        \
        }                                                                   \
        char* buffer = new char[size + 1];                                 \
        vsnprintf_s(buffer, size + 1, _TRUNCATE, format, args);            \
        std::string filename;                                              \
        std::string logMessage;                                            \
        log(logFileDirectory + std::string("/") + #Type, logfile_##Type, Name, \
            buffer, Colour, filename, logMessage);                         \
        delete[] buffer;                                                   \
        va_end(args);                                                      \
    }


#define VLOGGER_DEFINE_LOG_TYPE_DEBUG(Type) \
  inline void log##Type(const char* format, ...) {}

#define Log VLogger::defaultVLogger()

class VLogger : public VObject {
 public:
  static VLogger* defaultVLogger() {
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

 private:
  std::time_t time_now;
  std::string logFileDirectory;
  bool close_file;
  bool enable;

 
  void setCloseFile(bool closefile);
  // 获取完整的日志文件路径
  std::string getLogFilePath(const std::string& filename);

  // 获取日期
  std::string getDate(const std::time_t& time);

  // 获取时间
  std::string getTime(const std::time_t& time);

  // 获取日期时间
  std::string getDateTime(const std::time_t& time);

  VLOGGER_DEFINE_LOG_TYPE(Assert,
                          VLOGGER_LOG_ASSERT_NAME,
                          VLOGGER_LOG_ASSERT_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(Error,
                          VLOGGER_LOG_ERROR_NAME,
                          VLOGGER_LOG_ERROR_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(Warn, VLOGGER_LOG_WARN_NAME, VLOGGER_LOG_WARN_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(Info, VLOGGER_LOG_INFO_NAME, VLOGGER_LOG_INFO_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(Verbose,
                          VLOGGER_LOG_VERBOSE_NAME,
                          VLOGGER_LOG_VERBOSE_COLOR);
#ifdef _DEBUG
  VLOGGER_DEFINE_LOG_TYPE(Debug,
                          VLOGGER_LOG_DEBUG_NAME,
                          VLOGGER_LOG_DEBUG_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(DebugAssert,
                          VLOGGER_LOG_ASSERT_NAME,
                          VLOGGER_LOG_ASSERT_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(DebugError,
                          VLOGGER_LOG_ERROR_NAME,
                          VLOGGER_LOG_ERROR_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(DebugWarn,
                          VLOGGER_LOG_WARN_NAME,
                          VLOGGER_LOG_WARN_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(DebugInfo,
                          VLOGGER_LOG_INFO_NAME,
                          VLOGGER_LOG_INFO_COLOR);
  VLOGGER_DEFINE_LOG_TYPE(DebugVerbose,
                          VLOGGER_LOG_VERBOSE_NAME,
                          VLOGGER_LOG_VERBOSE_COLOR);
#else
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(Debug);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugAssert);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugError);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugWarn);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugInfo);
  VLOGGER_DEFINE_LOG_TYPE_DEBUG(DebugVerbose);
#endif  // DEBUG

 public:
  VLogger();
  ~VLogger();

  // 设置日志文件目录
  void setLogFileDirectory(const std::string& directory);

  // 记录日志
  void log(const std::string& filepath,
           std::ofstream& logfile,
           const std::string& level,
           const std::string& message,
           const std::string& colour,
           std::string& filename,
           std::string& logMessage);

  // 关闭日志文件
  void close(std::ofstream& logfile);
};

#undef VLOGGER_DEFINE_LOG_TYPE