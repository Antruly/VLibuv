#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <chrono>
#include <ctime>

class VTimerInfo {
private:
  VTimerInfo() {}
  ~VTimerInfo() {}

public:
  // 方法1：返回自纪元以来的毫秒数（UTC时区）
  static uint64_t getCurrentTimestamp();

  // 方法2：返回高精度时间戳（适合性能统计）
  static uint64_t getHighPrecisionTimestamp();

  // 获取本地时区时间戳
  static uint64_t getLocalTimestamp();
  static uint64_t getLegacyTimestamp();
};
