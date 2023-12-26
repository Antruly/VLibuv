#pragma once
#include "VTcp.h"
class VTcpService {
 public:
  VTcpService();
  ~VTcpService();

 private:
  void on_close();
  void echo_write(VWrite* req, int status);
  void alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf);
  void echo_read(VStream* client, ssize_t nread, const VBuf* buf);
  void on_new_connection(VStream* tcp, int status);

 protected:
  VTcp* getVTcpServer();
  VLoop* getVLoop();

 public:
  int run(uv_run_mode md = UV_RUN_DEFAULT);

  int listenIpv4(const char* addripv4, int port, int flags);

   virtual void newConnection(VTcp* client);

  void writeData(VTcp* client, const VBuf& data);

  virtual void readData(VTcp* client, const VBuf data);

 private:
  VLoop* loop = nullptr;
  VTcp* tcpServer = nullptr;
  sockaddr_in addr;

 private:
};