#include "VHttpClientObject.h"
VHttpClientObject::VHttpClientObject() {}

VHttpClientObject::~VHttpClientObject() {

  if (client != nullptr) {
    delete client;
    client = nullptr;
  }
}

// 重置状态以处理新请求

void VHttpClientObject::resetForNextRequest() {
  client->getVHttpRequest()->resetParser();
  client->getVHttpResponse()->resetParser();
  state = IDLE;
  _last_active_time = VTimerInfo::getCurrentTimestamp();
}
