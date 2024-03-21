#pragma once
#include "VAsync.h"
#include "VTcpClient.h"
#include "VTimer.h"
#include "VThreadPool.h"
#include <mutex>
class VTcpServer : public VObject {
 public:
  VTcpServer(int maxLoops = 100);
  virtual ~VTcpServer();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void close();
  void idleStart();
  void idleClose();

  int listenIpv4(const char* addripv4, int port, int flags);

  void closeClient(VTcpClient* tcpClient);
  void releaseClient(VTcpClient* tcpClient);

 public:
  void setIdleCb(std::function<void()> idle_cb);
  void setCloseCb(std::function<void(VTcp*)> close_cb);
  // 将根据事件池分配客户端单独事件循环线程
  void setNewClientCb(std::function<void(VTcpClient*)> new_client_cb);

 public:
  VTCP_WORKER_STATUS getStatus();
  VThreadPool* getVThreadPool();
  VTcp* getVTcp();
  VLoop* getVLoop();

 protected:
  VIdle* getVIdle();

  void setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce = false);
  void removStatus(VTCP_WORKER_STATUS vstatus);

 protected:
  void on_timer(VTimer* vtimer);
  void on_idle(VIdle* vidle);
  void on_close(VHandle* client);
  void on_new_connection(VStream* tcp, int status);

  // 异步回调函数
  void on_async_callback(VAsync* handle);

 protected:
  virtual void newClient(VTcpClient* client);

 protected:
  std::function<void()> tcp_idle_cb;
  std::function<void(VTcp*)> tcp_close_cb;
  std::function<void(VTcpClient*)> new_client_cb;

 private:
  // 事件循环池
  //VLoopPool* loopPool = nullptr;
  VThreadPool* threadpool = nullptr;
  VLoop* loop = nullptr;
  VTcp* tcp = nullptr;
  VIdle* idle = nullptr;
  VTimer* timer = nullptr;
 

  sockaddr_in addr;
  VTCP_WORKER_STATUS status = VTCP_WORKER_STATUS_NONE;
  VAsync* tcpService_async;
  std::vector<VTcpClient*> tcpservice_wait_close_client;
  std::vector<VTcpClient*> tcpservice_wait_callback_client;
  std::mutex tcpservice_wait_close_mutex;
  std::mutex tcpservice_wait_callback_mutex;
  bool idle_run = false;
 
};