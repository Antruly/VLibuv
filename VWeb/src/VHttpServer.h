#pragma once
#include "VHttpClient.h"
#include <VTcpServer.h>
class VHttpServer : public VObject {
 public:
  VHttpServer(size_t numThreads = 10,
             size_t maxThreads = 1024,
             size_t minIdleThreads = 5,
             size_t maxIdleThreads = 100,
             size_t maxTaskQueueSize = 1024);

  VHttpServer(VTcpServer* tcpServer);
  virtual ~VHttpServer();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void close();

  int listenIpv4(const char* addripv4, int port, int flags);


 public:
  void setCloseCb(std::function<void(VTcp*)> close_cb);
  // 将根据事件池分配客户端单独事件循环线程
  void setNewClientCb(std::function<void(VTcpClient*)> new_client_cb);

 public:

 protected:
  virtual void newHttpClient(VHttpClient* client);

 protected:
  std::function<void(VTcpClient*)> http_server_listen_finish_cb;
  std::function<void(VTcp*)> http_server_close_cb;
  std::function<void(VTcpClient*)> http_server_new_client_cb;
  

 private:
  VTcpServer* tcp_server_ = nullptr;


};