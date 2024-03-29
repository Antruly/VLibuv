#include <VTcpClient.h>
#include <VString.h>
#include <thread>

VBuf readCacheData;

void idleCallback() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
void closeCallback(VTcpClient* client) {

}
void writeCallback(VTcpClient* client, const VBuf* data, int status) {
  delete data;
  STD_L_ZERO_ERROR_SHOW(status, "writeCallback is error");
}
void readCallback(VTcpClient* client, const VBuf* data) {
  readCacheData.appand(*data);
}
void connectiondCallback(int status) {
  STD_L_ZERO_ERROR_SHOW(status, "connectiondCallback is error");
}

int main() {
  VTcpClient client;
  
  client.setIdleCb(idleCallback);
  client.setCloseCb(closeCallback);
  client.setWriteCb(writeCallback);
  client.setReadCb(readCallback);
  client.setConnectiondCb(connectiondCallback);

  int ret = client.connect("127.0.0.1", 8075);
  if (ret) {
    return -1;
  }
  
  client.run(UV_RUN_ONCE);

  if ((client.getStatus() &
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) == 0) {
    return -2;
  }

  VBuf* wData = new VBuf("123456", 6);
  client.writeData(*wData);
  client.run(UV_RUN_ONCE);

  client.clientReadStart();

  while (true) {
    client.run(UV_RUN_NOWAIT);
      if (readCacheData.size() > 0) {
      VString strData(readCacheData.getConstData(), readCacheData.size());
        readCacheData.clear();
        printf("%s", strData.c_str());

        if (strData == VString("exit")) {
          break;
        }
      }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  client.close();
  return 0;
}