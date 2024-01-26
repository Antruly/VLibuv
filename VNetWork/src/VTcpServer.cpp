#include "VTcpServer.h"

VTcpServer::VTcpServer() : VTcpBase() {}

VTcpServer::~VTcpServer() {}

void VTcpServer::on_close_client(VHandle* client) {
  if (this->tcp_close_cb)
    this->tcp_close_cb(reinterpret_cast<VTcp*>(client));
  delete client;
}

void VTcpServer::on_new_connection(VStream* tcp, int status) {
  STD_L_ZERO_ERROR_SHOW(status, "New connection");

  VTcp* client = new VTcp(this->getVLoop());

  if (tcp->accept(client) == 0) {
    client->readStart(
        std::bind(&VTcpServer::alloc_buffer, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&VTcpServer::echo_read, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    this->newConnection((VTcp*)tcp);
  } else {
    delete client;
  }
}

int VTcpServer::listenIpv4(const char* addripv4, int port, int flags) {
  int ret;
  ret = this->getVTcp()->bindIpv4(addripv4, port, flags);
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "bindIpv4");
  ret = this->getVTcp()->listen(
      std::bind(&VTcpServer::on_new_connection, this, std::placeholders::_1,
                std::placeholders::_2),
      128);
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "listenIpv4");
  return ret;
}

void VTcpServer::closeClient(VTcp* client) {
  client->close(
      std::bind(&VTcpServer::on_close_client, this, std::placeholders::_1));
}

void VTcpServer::setNewConnectionCb(
    std::function<void(VTcp*)> new_connection_cb) {
  this->tcpService_new_connection_cb = new_connection_cb;
}

void VTcpServer::newConnection(VTcp* client) {
#ifdef _DEBUG
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  client->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    printf("new client connected from IP: %s, Port: %d\n", peer_ip,
           ntohs(addrs.sin_port));
  } else {
    fprintf(stderr, "Failed to get peer IP address\n");
  }
#endif
#endif
#endif  // _DEBUG
  if (tcpService_new_connection_cb)
    this->tcpService_new_connection_cb(client);
}
