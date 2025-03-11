#include "VHttpClientObject.h"
VHttpClientObject::VHttpClientObject() {}

VHttpClientObject::~VHttpClientObject() {

  if (client != nullptr) {
    delete client;
    client = nullptr;
  }
}

// ����״̬�Դ���������

void VHttpClientObject::resetForNextRequest() {
  client->getVHttpRequest()->resetParser();
  client->getVHttpResponse()->resetParser();
  state = IDLE;
  _last_active_time = VTimerInfo::getCurrentTimestamp();
}
