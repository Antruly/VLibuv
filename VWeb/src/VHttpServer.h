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

  VTcpServer *getVTcpServer();
 public:
  void setCloseCb(std::function<void(VTcpServer *)> close_cb);
  // 将根据事件池分配客户端单独事件循环线程
  void
  setNewHttpClientCb(std::function<void(VHttpClient *)> new_http_client_cb);

 protected:
   void on_server_tcp_close(VTcp *tcp);
   void on_new_tcp_client(VTcpClient *client);


 protected:
   virtual void httpServerClose(VTcpServer *server);
   virtual void newHttpClient(VHttpClient *client);
   
  

 protected:
   std::function<void(VTcpServer *)> http_server_close_cb;
   std::function<void(VHttpClient *)> http_server_new_http_client_cb;
  

 private:
  VTcpServer* tcp_server_ = nullptr;


};