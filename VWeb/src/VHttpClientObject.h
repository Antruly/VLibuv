#pragma once
#include "VHttpClient.h"
#include "VWebDefine.h"
#include "VTimerInfo.h"



class VHttpClientObject {
public:
  VHttpClientObject();
  ~VHttpClientObject();

  // ��������״̬����
  enum ConnectionState {
    IDLE,         // ����״̬�ȴ�������
    READING_HEAD, // ���ڶ�ȡ����ͷ
    READING_BODY, // ���ڶ�ȡ������
    SENDING       // ���ڷ�����Ӧ
  };

  // ����״̬�Դ���������
  void resetForNextRequest();

  // ��Ա����
  ConnectionState state = IDLE;
  uint64_t _last_active_time = 0;   // ���ʱ���
  VTimer *_timeout_timer = nullptr; // ��ʱ��ʱ��
  VHttpClient *client = nullptr;
  VWebInterface *web_interface = nullptr;
  VBuf cache_data;
  void *data = nullptr;
  bool client_close = false;
};
