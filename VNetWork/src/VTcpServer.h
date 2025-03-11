#pragma once
#include "VAsync.h"
#include "VTcpClient.h"
#include "VTimer.h"
#include "VThreadPool.h"
#include "VNetWorkDefine.h"
#include <mutex>
class VTcpServer : public VObject {
 public:
  VTcpServer(size_t numThreads = 10,
             size_t maxThreads = 1024,
             size_t minIdleThreads = 5,
             size_t maxIdleThreads = 100,
             size_t maxTaskQueueSize = 1024);
  virtual ~VTcpServer();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void close();
  void stop();
  void idleStart();
  void idleClose();

  int listenIpv4(const char* addripv4, int port, int flags);

  void closeClient(VTcpClient* tcpClient);

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

  int setData(void *pdata);
  void *getData();

 protected:
  VIdle* getVIdle();

  void setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce = false);
  void removStatus(VTCP_WORKER_STATUS vstatus);

 protected:
  void on_timer(VTimer* vtimer);
  void on_idle(VIdle* vidle);
  void on_close(VHandle* client);
  void on_new_connection(VStream* tcp, int status);

 protected:
  virtual void newClient(VTcpClient* client);

 protected:
  std::function<void()> tcp_idle_cb;
  std::function<void(VTcp*)> tcp_close_cb;
  std::function<void(VTcpClient*)> new_client_cb;

 private:
  void *vdata = nullptr;
  VThreadPool* threadpool = nullptr;
  VLoop* loop = nullptr;
  VTcp* tcp = nullptr;
  VIdle* idle = nullptr;
  VTimer* timer = nullptr;

  VTCP_WORKER_STATUS status = VTCP_WORKER_STATUS_NONE;
  bool idle_run = false;
 
};