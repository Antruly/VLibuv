#pragma once
#include "VLibuv.h"
#include "VTcpServer.h"
#include "VThread.h"
#include "VWrite.h"

VTcpServer tcpServer(1,1024,50,512,1024);
std::string getTestData =
    "HTTP/1.1 200 OK\r\nContent-Type: text/html; "
    "charset=utf-8\r\nContent-Length: 123\r\nConnection: "
    "keep-alive\r\n\r\n<!DOCTYPE html><html><head><title>Sample "
    "Response</title></head><body><h1>Hello, World!</h1></body></html>";
std::string getJson =
    "HTTP/1.1 200 OK\r\nContent-Type: application/json; "
    "charset=utf-8\r\nContent-Length: 123\r\nConnection: "
    "keep-alive\r\n\r\n{\"message\": \"Hello, World!\"}";
int64_t tcpServer_index = 0;
int64_t tcpServer_maxIndex = 0;
int64_t tcpServer_readIndex = 0;
int64_t tcpServer_writeIndex = 0;
int64_t tcpServer_userMaxIndex = 0;

void tcpServer_close(VTcp* client) {
  printf("server close !\n");
}

void tcpClient_close(VTcpClient* client) {
  tcpServer_index--;
  printf("client close !\n");
}

void tcpClient_write(VTcpClient* client, const VBuf* data, int status) {
  tcpServer_writeIndex++;
  delete data;
}

void tcpClient_read(VTcpClient* client, const VBuf* data) {
  tcpServer_readIndex++;
  //printf("client data:%s\n", data->getData());

  VBuf* newdata = new VBuf(data->getData(), data->size());
  client->writeData(*newdata);
}

void newClient(VTcpClient* client) {
  tcpServer_index++;
  tcpServer_maxIndex++;
  if (tcpServer_userMaxIndex < tcpServer_index) {
    tcpServer_userMaxIndex = tcpServer_index;
  }

  client->setCloseCb(tcpClient_close);
  client->setReadCb(tcpClient_read);
  client->setWriteCb(tcpClient_write);
  client->clientReadStart();
  printf("new client connect !\n");
  client->run();
}

int main() {
  tcpServer.setCloseCb(tcpServer_close);
  tcpServer.setNewClientCb(newClient);

  VThread td;
  td.start(
      [&](void* data) {
        printf("listenIpv4:%s port:8075\n\n", "0.0.0.0");
        tcpServer.listenIpv4("0.0.0.0", 8075, 0);
        tcpServer.run();
      },
      nullptr);

  while (true) {
    
    printf("new client  curruntClient:%lld userMaxIndex:%lld maxClient:%lld\n",
           tcpServer_index, tcpServer_userMaxIndex, tcpServer_maxIndex);
    printf("new client  readIndex:%lld writeIndex:%lld\n\n",
           tcpServer_readIndex, tcpServer_writeIndex);

   VThreadPool::Statistics info = tcpServer.getVThreadPool()->getStatistics();
    printf("idleThreads:%zu\n", info.idleThreads.load());
   printf("numThreads:%zu\n", info.numThreads.load());
    printf("taskQueueSize:%zu\n", info.taskQueueSize.load());
   printf("workingThreads:%zu\n", info.workingThreads.load());
   printf("workedNumber:%zu\n", info.workedNumber.load());
    

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  td.join();

  return 0;
}
