#include "VLogger.h"
#include <iostream>
#include "VLibuvInclude.h"
#include <fcntl.h>
#include <fstream>

VLogger::VLogger() : close_file(false), enable(false){
  char* buffer = new char[10240]();
  size_t size = 10240;
  std::string curpath;
  // 获取当前工作目录的绝对路径
  int result = uv_cwd(buffer, &size);
  if (!result) {
    curpath = std::string(buffer, size);
  } 

  this->log_encoding = SystemEncoding();

  this->setLogFileDirectory(curpath + "/logs");
  delete buffer;

  this->setLogTypes(VLOGGER_TYPE::VLOGGER_TYPE_ALL);

 #ifdef WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE) {
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
      dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hOut, dwMode);
    }
  }
#endif  // WIN32

  

}
VLogger::~VLogger() {

}

std::string VLogger::getCurrentDate()
{
    std::time_t time = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return std::string(buffer);
}

std::string VLogger::getCurrentTime()
{
    std::time_t time = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::string VLogger::getCurrentDateTime()
{
    std::time_t time = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

void VLogger::setEnable(bool eb){
    enable = eb; }

bool VLogger::getEnable() const { return enable; }

void VLogger::setLogTypes(VLOGGER_TYPE type) { log_types = type; }

VLOGGER_TYPE VLogger::getLogTypes() const { return log_types; }

void VLogger::setCloseFile(bool closefile) {
  close_file = closefile;
}

std::string VLogger::getLogFilePath(const std::string& filename) {
  if (logFileDirectory.empty()) {
    return filename;
  } else {
    return logFileDirectory + "/" + filename;
  }
}

// 获取当前日期
std::string VLogger::getDate(const std::time_t& time) {
  std::tm* timeinfo = std::localtime(&time);
  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  return std::string(buffer);
}

// 获取当前时间

std::string VLogger::getTime(const std::time_t& time) {
  std::tm* timeinfo = std::localtime(&time);
  char buffer[80];
  std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
  return std::string(buffer);
}

std::string VLogger::getDateTime(const std::time_t& time)
{
    std::tm* timeinfo = std::localtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

void VLogger::setLogFileDirectory(const std::string& directory) {
  logFileDirectory = directory;
  // 如果指定的目录不存在，则创建它
  if (!directory.empty()) {
    if (_access(directory.c_str(), 0) == -1) {
      if (mkdir(directory.c_str()) != 0) {
        std::cerr << "Error: Failed to create directory." << std::endl;
        logFileDirectory = "";  // 设置目录失败时，将目录设置为空
      }
    }
    
  }
}

void VLogger::setEncoding(VString::Encoding encoding)
{

}

void VLogger::log(const std::string& filepath,
                  std::ofstream& logfile,
                  const std::string& level,
                  const std::string& message,
                  const std::string& colour,
                  std::string& filename,
                  std::string& logMessage) {
  if (!filepath.empty()) {
    if (_access(filepath.c_str(), 0) == -1) {
      if (mkdir(filepath.c_str()) != 0) {
        std::cerr << "Error: Failed to create directory." << std::endl;
        return;
      }
    }
  } else {
    return;
  }
  time_now = std::time(nullptr);
 
  logMessage =
      getDate(time_now) + " " + getTime(time_now) + " " + level + " " + message;
  VString vlogMessage = ((VString)logMessage).toString(log_encoding);
  std::cout << colour << vlogMessage.c_str()
            << VCORE_ANSI_COLOR_RESET
            << std::endl;

  if (!enable)
      return;

  if (!logfile.is_open()) {
    filename = getDate(time_now) + ".log";
    logfile.open(filepath + "/" + filename, std::ios_base::app);
  } 

  if (logfile.is_open()) {
    logfile.write(vlogMessage.c_str(), vlogMessage.size());
    logfile.put('\n');  // 写入日志文件
    if (close_file)
    close(logfile);
  } else {
    std::cout << level << " log file is not open" << filepath;
  }
}

void VLogger::close(std::ofstream& logfile) {
  if (logfile.is_open()) {
    logfile.close();
  }
}