#include "VTcpService.h"

VTcpService::VTcpService() : addr() {
  loop = new VLoop();
  tcpServer = new VTcp(loop);
}

VTcpService::~VTcpService() {
  delete tcpServer;
  delete loop;
}

void VTcpService::on_close(VHandle* client) {
	if (this->tcpService_close_cb)
		this->tcpService_close_cb(reinterpret_cast<VTcp*>(client));
	delete client;
}

void VTcpService::echo_write(VWrite* req, int status) {
  if (status) {
#ifdef _DEBUG
    fprintf(stderr, "Write error %s\n", uv_strerror(status));
#endif
  }
  if (this->tcpService_write_cb)
  this->tcpService_write_cb(reinterpret_cast<VTcp*>(req->getData()), req->getBuf2());
  delete req;
  
}

void VTcpService::echo_write_clone(VWrite* req, int status)
{
	if (status) {
#ifdef _DEBUG
		fprintf(stderr, "Write clone error %s\n", uv_strerror(status));
#endif
	}
	if (this->tcpService_write_cb)
	this->tcpService_write_cb(reinterpret_cast<VTcp*>(req->getData()), req->getBuf2());
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
	bf.setData(buf->getData(), nread);
	this->readData(reinterpret_cast<VTcp*>(client), bf);
	bf.resize(0);

  } else if (nread < 0) {
    if (nread != UV_EOF)
#ifdef _DEBUG
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
#endif // endif
	client->close(std::bind(&VTcpService::on_close, this, std::placeholders::_1));
	const_cast<VBuf*>(buf)->clean();
  }
 
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

void VTcpService::close(std::function<void(VHandle*)> closeCallback){
	tcpServer->close(closeCallback);
};

void VTcpService::close(){
	tcpServer->close();
}

void VTcpService::closeClient(VTcp* client)
{
	client->close(std::bind(&VTcpService::on_close, this, std::placeholders::_1));
}

void VTcpService::setCloseCb(std::function<void(VTcp*)> close_cb)
{
	this->tcpService_close_cb = close_cb;
}

void VTcpService::setWriteCb(std::function<void(VTcp*, const VBuf*)> write_cb)
{
	this->tcpService_write_cb = write_cb;
}

void VTcpService::setReadCb(std::function<void(VTcp*, const VBuf*)> read_cb)
{
	this->tcpService_read_cb = read_cb;
}

void VTcpService::setNewClientCb(std::function<void(VTcp*)> new_connection_cb)
{
	this->tcpService_new_connection_cb = new_connection_cb;
}

void VTcpService::newConnection(VTcp* client) {

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
#endif // _DEBUG
  if (tcpService_new_connection_cb)
  this->tcpService_new_connection_cb(client);
}

void VTcpService::writeData(VTcp* client, const VBuf& data) {
  VWrite* req = new VWrite();
  req->setBuf(&data);
  req->setBuf2(&data);
  req->setData(client);
  client->write(req, &data, 1,
                std::bind(&VTcpService::echo_write, this, std::placeholders::_1,
                          std::placeholders::_2));
}

void VTcpService::writeCloneData(VTcp* client, const VBuf& data) {
	VWrite* req = new VWrite();
	const VBuf* bf = new VBuf(data);
	req->setBuf(bf);
	req->setBuf2(&data);
	req->setData(client);
	client->write(req, bf, 1,
		std::bind(&VTcpService::echo_write_clone, this, std::placeholders::_1,
		std::placeholders::_2));
}
void VTcpService::readData(VTcp* client, const VBuf& data) {

#ifdef _DEBUG
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  client->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    VBuf newData(data);
    newData.resize(data.size() + 1);
    printf("IP: %s, Port: %d reaData \n", peer_ip, ntohs(addrs.sin_port));
  } else {
    fprintf(stderr, "Failed to get peer IP address\n");
  }
#endif
#endif
#endif // _DEBUG
  if (this->tcpService_read_cb)
  this->tcpService_read_cb(client, &data);
}
