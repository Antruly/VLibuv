#pragma once
#include "VTimerInfo.h"

// 方法1：返回自纪元以来的毫秒数（UTC时区）

uint64_t VTimerInfo::getCurrentTimestamp() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

// 方法2：返回高精度时间戳（适合性能统计）

uint64_t VTimerInfo::getHighPrecisionTimestamp() {
  using namespace std::chrono;
  return duration_cast<microseconds>(
             high_resolution_clock::now().time_since_epoch())
      .count();
}


// 获取本地时区时间戳

uint64_t VTimerInfo::getLocalTimestamp() {
  // 获取系统时间
  auto now = std::chrono::system_clock::now();

  // 转换为本地时间
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm local_tm = *std::localtime(&t);

  // 计算本地时间戳
  auto local_now =
      std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             local_now.time_since_epoch())
      .count();
}

uint64_t VTimerInfo::getLegacyTimestamp() {
#ifdef _WIN32
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  ULARGE_INTEGER uli;
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;
  return uli.QuadPart / 10000 - 11644473600000LL;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
#endif
}
