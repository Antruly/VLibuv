#include "VTcpService.h"

VTcpService::VTcpService() : addr() {
  loop = new VLoop();
  tcpServer = new VTcp(loop);
}

VTcpService::~VTcpService() {
  delete tcpServer;
  delete loop;
}

void VTcpService::on_close() {}

void VTcpService::echo_write(VWrite* req, int status) {
  if (status) {
    fprintf(stderr, "Write error %s\n", uv_strerror(status));
  }
  delete req->getBuf();
  delete req;
}

void VTcpService::alloc_buffer(VHandle* handle,
                                      size_t suggested_size,
                                      VBuf* buf) {
  buf->resize(suggested_size);
}

void VTcpService::echo_read(VStream* client,
                                   ssize_t nread,
                                   const VBuf* buf) {
  if (nread > 0) {
    VBuf bf;

    bf.clone(*buf);

    bf.resize(nread);

    this->readData((VTcp*)client, bf);

  } else if (nread < 0) {
    if (nread != UV_EOF)
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    delete client;
  }

  ((VBuf*)buf)->clean();
}

void VTcpService::on_new_connection(VStream* tcp, int status) {
  STD_L_ZERO_ERROR_SHOW(status, "New connection");

  VTcp* client = new VTcp(this->loop);

  if (tcp->accept(client) == 0) {
    client->readStart(
        std::bind(&VTcpService::alloc_buffer, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&VTcpService::echo_read, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    this->newConnection((VTcp*)tcp);
  } else {
    delete client;
  }
}

VTcp* VTcpService::getVTcpServer() {
  return tcpServer;
}

VLoop* VTcpService::getVLoop() {
  return loop;
}

int VTcpService::run(uv_run_mode md) {
  return loop->exec(md);
}

int VTcpService::listenIpv4(const char* addripv4, int port, int flags) {
  int ret;
  ret = tcpServer->bindIpv4(addripv4, port, flags);
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "bindIpv4");
  ret =
      tcpServer->listen(std::bind(&VTcpService::on_new_connection, this,
                                  std::placeholders::_1, std::placeholders::_2),
                        128);
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "listenIpv4");
  return ret;
}

void VTcpService::newConnection(VTcp* client) {
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
 
}

void VTcpService::writeData(VTcp* client, const VBuf& data) {
  VWrite* req = new VWrite();
  VBuf* bf = new VBuf(data);
  req->setBuf(bf);
  client->write(req, bf, 1,
                std::bind(&VTcpService::echo_write, this, std::placeholders::_1,
                          std::placeholders::_2));
}

void VTcpService::readData(VTcp* client, const VBuf data) {
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  client->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    VBuf newData(data);
    newData.resize(data.size() + 1);
    printf("IP: %s, Port: %d readData:%s\n", peer_ip, ntohs(addrs.sin_port),
           newData.getData());
  } else {
    fprintf(stderr, "Failed to get peer IP address\n");
  }
#endif
#endif

}
