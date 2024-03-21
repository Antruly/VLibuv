#pragma once
#include "VIdle.h"
#include "VTcp.h"
#include <map>
#include <VAsync.h>

enum VTCP_WORKER_STATUS {
  VTCP_WORKER_STATUS_NONE = 0x00,  // 无状态

  // libuv提供的连接状态
  VTCP_WORKER_STATUS_CONNECTING = 0x01,    // 连接中
  VTCP_WORKER_STATUS_CONNECTED = 0x02,     // 已连接
  VTCP_WORKER_STATUS_DISCONNECTED = 0x04,  // 已断开连接

  VTCP_WORKER_STATUS_LISTENING = 0x08,  // 监听中
  VTCP_WORKER_STATUS_CLOSED = 0x10,     // 已关闭

  // 读写状态
  VTCP_WORKER_STATUS_READING = 0x20,  // 可读取数据
  VTCP_WORKER_STATUS_WRITING = 0x40,  // 可写入数据

  // 数据处理状态
  VTCP_WORKER_STATUS_PROCESSING = 0x80,  // 正在处理数据

  // 断线重连状态
  VTCP_WORKER_STATUS_RECONNECTING = 0x100,  // 重新连接中

  // libuv提供的错误状态
  VTCP_WORKER_STATUS_ERROR_NONE = 0x200,           // 无错误
  VTCP_WORKER_STATUS_ERROR_UNKNOWN = 0x400,        // 未知错误
  VTCP_WORKER_STATUS_ERROR_EADDRNOTAVAIL = 0x800,  // 地址不可用
  VTCP_WORKER_STATUS_ERROR_ECONNRESET = 0x1000,    // 连接被重置

};

class VTcpClient : public VObject {
 public:
  // 新的构造函数，接受外部VLoop指针
  VTcpClient();
  VTcpClient(VLoop* externalLoop);
  virtual ~VTcpClient();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void close();
  void idleStart();
  void idleClose();

  void setIdleCb(std::function<void()> idle_cb);
  void setCloseCb(std::function<void(VTcpClient*)> close_cb);
  void setWriteCb(std::function<void(VTcpClient*, const VBuf*, int)> write_cb);
  void setReadCb(std::function<void(VTcpClient*, const VBuf*)> read_cb);
  void setConnectiondCb(std::function<void(int)> connectiond_cb);

  int connect(const char* addripv4, int port);
  void writeData(const VBuf& data);
  void writeNewData(const VBuf& data);

  void clientReadStart();

  void waitConnectFinish();
  void waitCloseFinish();
  void waitWriteFinish();

 public:
  VTCP_WORKER_STATUS getStatus();

  VTcp* getVTcp();
  VLoop* getVLoop();

  sockaddr_storage getLocalAddrs(std::string& ip, int& port);
  sockaddr_storage getPeerAddrs(std::string& ip, int& port);

 protected:
  VIdle* getVIdle();

  void setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce = false);
  void removStatus(VTCP_WORKER_STATUS vstatus);
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
  VLoop* loop = nullptr;
  VTcp* tcp = nullptr;
  VIdle* idle = nullptr;
  VAsync* async_connect = nullptr;
  VAsync* async_close = nullptr;
  VAsync* async_write = nullptr;
  VBuf* buffer_cache = nullptr;
 
  bool idle_run = false;
  bool tcp_run = false;
  bool own_loop = true;

 private:
  VTCP_WORKER_STATUS status = VTCP_WORKER_STATUS_NONE;
};