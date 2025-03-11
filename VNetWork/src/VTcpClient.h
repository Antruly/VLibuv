#pragma once
#include "VIdle.h"
#include "VTcp.h"
#include <map>
#include <VAsync.h>
#include "VNetWorkDefine.h"


class VTcpClient : public VObject {
 public:
  // 新的构造函数，接受外部VLoop指针
  VTcpClient();
  VTcpClient(VLoop* externalLoop);
  virtual ~VTcpClient();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void initCallBackup();
  void stop();
  void close();
  void idleStart();
  void idleClose();

  void setIdleCb(std::function<void()> idle_cb);
  void setCloseCb(std::function<void(VTcpClient*)> close_cb);
  void setWriteCb(std::function<void(VTcpClient*, const VBuf*, int)> write_cb);
  void setReadCb(std::function<void(VTcpClient*, const VBuf*)> read_cb);
  void setConnectiondCb(std::function<void(int)> connectiond_cb);

  int connect(const char* addripv4, int port);
  int writeData(const VBuf& data);
  int writeNewData(const VBuf& data);

  int clientReadStart();

  void waitConnectFinish();
  void waitCloseFinish();
  void waitWriteFinish();

 public:
  VTCP_WORKER_STATUS getStatus();
   void setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce = false);
   void removStatus(VTCP_WORKER_STATUS vstatus);

  VTcp* getVTcp();
  VLoop* getVLoop();

  sockaddr_storage getLocalAddrs(std::string& ip, int& port);
  sockaddr_storage getPeerAddrs(std::string& ip, int& port);

  int setData(void *pdata);
  void *getData();

 protected:
  VIdle* getVIdle();

  
  virtual void readData(VTcp* client, const VBuf& data);

 protected:
  void on_idle(VIdle* vidle);
  void on_close(VHandle* client);
  void echo_write(VWrite* req, int status);
  void alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf);
  void echo_read(VStream* client, ssize_t nread, const VBuf* buf);

 protected:
  virtual void connectiond(int status);

 private:
  void on_connection(VConnect* req, int status);

 protected:
  std::function<void()> tcp_idle_cb;
  std::function<void(VTcpClient*)> tcp_close_cb;
  std::function<void(VTcpClient*, const VBuf*, int)> tcp_write_cb;
  std::function<void(VTcpClient*, size_t)> tcp_alloc_buffer_cb;
  std::function<void(VTcpClient*, const VBuf*)> tcp_read_cb;
  std::function<void(int)> tcp_client_connectiond_cb;

  sockaddr_in addr;
  // 事件循环池
  std::map<VBuf*, int64_t> alloc_buffers;
  int64_t buffer_num = 0L;
  
 private:
  void* vdata = nullptr;
  VLoop* loop = nullptr;
  VTcp* tcp = nullptr;
  VIdle* idle = nullptr;
  bool async_connect = false;
  bool async_close = false;
  bool async_write = false;
  VBuf* buffer_cache = nullptr;
 
  bool idle_run = false;
  bool tcp_run = false;
  bool own_loop = true;
  bool read_start = false;

 private:
  VTCP_WORKER_STATUS status = VTCP_WORKER_STATUS_NONE;
};