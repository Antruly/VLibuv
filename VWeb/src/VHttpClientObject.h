#pragma once
#include "VHttpClient.h"
#include "VWebDefine.h"
#include "VTimerInfo.h"



class VHttpClientObject {
public:
  VHttpClientObject();
  ~VHttpClientObject();

  // 新增连接状态管理
  enum ConnectionState {
    IDLE,         // 空闲状态等待新请求
    READING_HEAD, // 正在读取请求头
    READING_BODY, // 正在读取请求体
    SENDING       // 正在发送响应
  };

  // 重置状态以处理新请求
  void resetForNextRequest();

  // 成员变量
  ConnectionState state = IDLE;
  uint64_t _last_active_time = 0;   // 最后活动时间戳
  VTimer *_timeout_timer = nullptr; // 超时定时器
  VHttpClient *client = nullptr;
  VWebInterface *web_interface = nullptr;
  VBuf cache_data;
  void *data = nullptr;
  bool client_close = false;
};
