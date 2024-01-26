#pragma once
#include "VTcpBase.h"

class VTcpClient : public VTcpBase {
 public:
  VTcpClient();
  ~VTcpClient();

 public:
  int connect(const char* addripv4, int port);
  void writeData(const VBuf& data);
  void writeNewData(const VBuf& data);

  void setConnectiondCb(std::function<void(int)> connectiond_cb);

 protected:
  virtual void connectiond(int status);

 private:
  void on_connection(VConnect* req, int status);

 protected:
  std::function<void(int)> tcp_client_connectiond_cb;

 private:
};