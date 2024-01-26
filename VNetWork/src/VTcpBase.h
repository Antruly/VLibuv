#pragma once
#include "VIdle.h"
#include "VTcp.h"
enum VTCP_WORKER_STATUS {
  VTCP_WORKER_STATUS_NONE = 0x00,  // 无状态

  // libuv提供的连接状态
  VTCP_WORKER_STATUS_CONNECTING = 0x01,    // 连接中
  VTCP_WORKER_STATUS_CONNECTED = 0x02,     // 已连接
  VTCP_WORKER_STATUS_DISCONNECTED = 0x04,  // 已断开连接

  // 读写状态
  VTCP_WORKER_STATUS_READING = 0x08,  // 可读取数据
  VTCP_WORKER_STATUS_WRITING = 0x10,  // 可写入数据

  // 数据处理状态
  VTCP_WORKER_STATUS_PROCESSING = 0x20,  // 正在处理数据

  // 断线重连状态
  VTCP_WORKER_STATUS_RECONNECTING = 0x40,  // 重新连接中

  // libuv提供的错误状态
  VTCP_WORKER_STATUS_ERROR_NONE = 0x80,            // 无错误
  VTCP_WORKER_STATUS_ERROR_UNKNOWN = 0x100,        // 未知错误
  VTCP_WORKER_STATUS_ERROR_EADDRNOTAVAIL = 0x200,  // 地址不可用
  VTCP_WORKER_STATUS_ERROR_ECONNRESET = 0x400,     // 连接被重置

  // 其他状态
  VTCP_WORKER_STATUS_CUSTOM1 = 0x800,   // 自定义状态1
  VTCP_WORKER_STATUS_CUSTOM2 = 0x1000,  // 自定义状态2

};

class VTcpBase : public VObject {
 public:
  VTcpBase();
  virtual ~VTcpBase();

 public:
  virtual int run(uv_run_mode md = UV_RUN_DEFAULT);
  void idleStart();
  void idleClose();

  virtual void writeData(VTcp* client, const VBuf& data);
  virtual void writeNewData(VTcp* client, const VBuf& data);

  virtual void setIdleCb(std::function<void()> idle_cb);
  virtual void setCloseCb(std::function<void(VTcp*)> close_cb);
  virtual void setWriteCb(
      std::function<void(VTcp*, const VBuf*, int)> write_cb);
  virtual void setReadCb(std::function<void(VTcp*, const VBuf*)> read_cb);

  virtual void close();

  VTCP_WORKER_STATUS getStatus();

 protected:
  VTcp* getVTcp();
  VLoop* getVLoop();
  VIdle* getVIdle();

  void setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce = false);
  virtual void readData(VTcp* client, const VBuf& data);

 protected:
  void on_idle(VIdle* vidle);
  void on_close(VHandle* client);
  void echo_write(VWrite* req, int status);
  void alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf);
  void echo_read(VStream* client, ssize_t nread, const VBuf* buf);

 protected:
  std::function<void()> tcp_idle_cb;
  std::function<void(VTcp*)> tcp_close_cb;
  std::function<void(VTcp*, const VBuf*, int)> tcp_write_cb;
  std::function<void(VTcp*, size_t)> tcp_alloc_buffer_cb;
  std::function<void(VTcp*, const VBuf*)> tcp_read_cb;

  sockaddr_in addr;

 private:
  VLoop* loop = nullptr;
  VTcp* tcp = nullptr;
  VIdle* idle = nullptr;

 private:
  VTCP_WORKER_STATUS status = VTCP_WORKER_STATUS_NONE;
};