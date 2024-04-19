#pragma once
#include "VLibuv.h"
#include "VWebSocketClient.h"
#include <inttypes.h>

#define __STDC_FORMAT_MACROS
bool isClose = false;

void setCallback(VWebSocketClient& websocketClient) {
  websocketClient.setWebsocketConnectiondCb(
      [&](VWebSocketClient* websocketClient, int status) {
    if (status < 0) {
          Log->logDebugError("websocket connect error \n");
      return;
    }
  });

  websocketClient.setWebsocketRecvBodyCb(
      [&](VWebSocketClient* websocketClient, const VBuf* data) -> bool {
    
      Log->logDebug("%s\n", data->toString().c_str());
      return true;
  });

  websocketClient.setWebsocketSendFinishCb(
      [&](VWebSocketClient* websocketClient, int status) {
    if (status < 0) {
          Log->logDebugError("websocket connect error \n");
      return;
    }
  });

   websocketClient.setWebsocketCloseCb(
      [&](VWebSocketClient* websocketClient, int status) {
    if (status < 0) {
          Log->logDebugError("websocket close error \n");
      return;
    }
    isClose = true;
  });

   websocketClient.setConnectiondCb([&](VHttpClient* httpClient, int status) {
     if (status < 0) {
       Log->logDebugError("tcp connect error \n");
       return;
     }
   });
    websocketClient.setCloseCb(
       [&](VHttpClient* httpClient, int status) {
         if (status < 0) {
       Log->logDebugError("tcp close error \n");
           return;
         }
         isClose = true;
       });
}


int main() {
  VWebSocketClient websocketClient;
  setCallback(websocketClient);
  
  std::string websocketUrl = "ws://127.0.0.1:8085";
  std::string websocketUrl2 = "ws://192.168.1.210:8085/detection-admin/vklWebSocket";
  
  
  if (!websocketClient.websocketConnect(websocketUrl2)) {
    return -1;
  }

  uint64_t times = websocketClient.websocketPing();
  Log->logDebug("%s ping times %" PRIu64 " ms \n",
                         websocketUrl2.c_str(), times);
 

  

  std::string strData = "123";
  std::string strData2 = "hello world!";
  VBuf data(strData);
  VBuf data2(strData2);

  websocket_flags flags =
      static_cast<websocket_flags>(WS_OP_TEXT | WS_HAS_MASK);
  websocket_flags flags2 = static_cast<websocket_flags>(
      WS_OP_CONTINUE | WS_FINAL_FRAME | WS_HAS_MASK);
  char mask[4] = {0};
  websocketClient.getVWebSocketParser()->generateMask(mask);

  websocketClient.websocketSend(data, flags, mask);
  websocketClient.websocketSend(data2, flags2, mask);

  while (!isClose) {
    websocketClient.run(UV_RUN_NOWAIT);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  websocketClient.websocketClose();
  return 0;
}
