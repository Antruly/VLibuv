#pragma once
#include "VTcpBase.h"
class VTcpServer : public VTcpBase {
 public:
  VTcpServer();
  ~VTcpServer();

 public:
  int listenIpv4(const char* addripv4, int port, int flags);

  void closeClient(VTcp* client);

  void setNewConnectionCb(std::function<void(VTcp*)> new_connection_cb);

 protected:
  void on_new_connection(VStream* tcp, int status);
  void on_close_client(VHandle* client);

 protected:
  virtual void newConnection(VTcp* client);

 protected:
  std::function<void(VTcp*)> tcpService_new_connection_cb;

 private:
};