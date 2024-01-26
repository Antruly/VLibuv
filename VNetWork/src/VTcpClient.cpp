#include "VTcpClient.h"

VTcpClient::VTcpClient() : VTcpBase() {}

VTcpClient::~VTcpClient() {}

void VTcpClient::on_connection(VConnect* req, int status) {
  STD_L_ZERO_ERROR_SHOW(status, "connection");

  VTcp* tcp = reinterpret_cast<VTcp*>(req->getData());

#ifdef _DEBUG
  char ip[INET6_ADDRSTRLEN] = {0};
  int port;

  uv_ip4_name(reinterpret_cast<sockaddr_in*>(&addr), ip, sizeof(ip));
  port = ntohs(reinterpret_cast<sockaddr_in*>(&addr)->sin_port);
  fprintf(stderr, "connection ip:%s port:%d \n", ip, port);
#endif  // endif

  if (status < 0) {
#ifdef _DEBUG
    fprintf(stderr, "on_connection error %s\n", uv_err_name(status));
#endif  // endif
  } else {
    tcp->readStart(
        std::bind(&VTcpClient::alloc_buffer, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&VTcpClient::echo_read, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
  }

  this->connectiond(status);
  delete req;
}

int VTcpClient::connect(const char* addripv4, int port) {
  int ret;

  VConnect* req = new VConnect();
  req->setData(this->getVTcp());
  uv_ip4_addr(addripv4, port, &addr);

  ret = this->getVTcp()->connect(
      req, reinterpret_cast<sockaddr*>(&addr),
      std::bind(&VTcpClient::on_connection, this, std::placeholders::_1,
                std::placeholders::_2));
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "connect");
  return ret;
}

void VTcpClient::writeData(const VBuf& data) {
  VTcpBase::writeData(this->getVTcp(), data);
}

void VTcpClient::writeNewData(const VBuf& data) {
  VTcpBase::writeNewData(this->getVTcp(), data);
}

void VTcpClient::setConnectiondCb(std::function<void(int)> connectiond_cb) {
  this->tcp_client_connectiond_cb = connectiond_cb;
}

void VTcpClient::connectiond(int status) {
#ifdef _DEBUG
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  this->getVTcp()->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    printf("client connected from IP: %s, Port: %d\n", peer_ip,
           ntohs(addrs.sin_port));
  } else {
    fprintf(stderr, "Failed to get peer IP address\n");
  }
#endif
#endif
#endif  // _DEBUG
  if (tcp_client_connectiond_cb)
    this->tcp_client_connectiond_cb(status);
}
