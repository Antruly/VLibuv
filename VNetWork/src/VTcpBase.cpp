#include "VTcpBase.h"

VTcpBase::VTcpBase() : addr() {
  this->loop = new VLoop();
  this->tcp = new VTcp(loop);
  this->idle = new VIdle(loop);
}

VTcpBase::~VTcpBase() {
  delete this->idle;
  delete this->tcp;
  delete this->loop;
}
void VTcpBase::on_idle(VIdle* vidle) {
  if (tcp_idle_cb)
    tcp_idle_cb();
}

void VTcpBase::on_close(VHandle* client) {
  reinterpret_cast<VTcp*>(client)->init(loop);
  if (tcp_close_cb)
    tcp_close_cb(reinterpret_cast<VTcp*>(client));
}

void VTcpBase::echo_write(VWrite* req, int status) {
  if (status) {
#ifdef _DEBUG
    fprintf(stderr, "Write error %s\n", uv_strerror(status));
#endif
  }
  if (tcp_write_cb)
    tcp_write_cb(reinterpret_cast<VTcp*>(req->getData()), req->getSrcBuf(),
                 status);

  if (req->getBuf() != nullptr)
    delete req->getBuf();
  delete req;
}

void VTcpBase::alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf) {
  if (tcp_alloc_buffer_cb)
    tcp_alloc_buffer_cb(reinterpret_cast<VTcp*>(handle), suggested_size);
  buf->resize(suggested_size);
}

void VTcpBase::echo_read(VStream* client, ssize_t nread, const VBuf* buf) {
  if (nread > 0) {
    VBuf bf;
    bf.setData(buf->getData(), nread);
    this->readData(reinterpret_cast<VTcp*>(client), bf);
    bf.resize(0);

  } else if (nread < 0) {
    if (nread != UV_EOF)
#ifdef _DEBUG
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
#endif  // endif
    client->close(std::bind(&VTcpBase::on_close, this, std::placeholders::_1));
    const_cast<VBuf*>(buf)->clean();
  }
}
VTcp* VTcpBase::getVTcp() {
  return this->tcp;
}
VLoop* VTcpBase::getVLoop() {
  return this->loop;
}
VIdle* VTcpBase::getVIdle() {
  return this->idle;
}
int VTcpBase::run(uv_run_mode md) {
  return this->loop->exec(md);
}
void VTcpBase::idleStart() {
  this->idle->start(std::bind(&VTcpBase::on_idle, this, std::placeholders::_1));
}

void VTcpBase::idleClose() {
  this->idle->close();
}

void VTcpBase::writeData(VTcp* client, const VBuf& data) {
  VWrite* req = new VWrite();
  req->setBuf(nullptr);
  req->setSrcBuf(&data);
  req->setData(this->getVTcp());
  client->write(req, &data, 1,
                std::bind(&VTcpBase::echo_write, this, std::placeholders::_1,
                          std::placeholders::_2));
}

void VTcpBase::writeNewData(VTcp* client, const VBuf& data) {
  VWrite* req = new VWrite();
  const VBuf* bf = new VBuf(data);
  req->setBuf(bf);
  req->setSrcBuf(&data);
  req->setData(this->getVTcp());
  client->write(req, bf, 1,
                std::bind(&VTcpBase::echo_write, this, std::placeholders::_1,
                          std::placeholders::_2));
}

void VTcpBase::setIdleCb(std::function<void()> idle_cb) {
  tcp_idle_cb = idle_cb;
}
void VTcpBase::setCloseCb(std::function<void(VTcp*)> close_cb) {
  this->tcp_close_cb = close_cb;
}

void VTcpBase::setWriteCb(
    std::function<void(VTcp*, const VBuf*, int)> write_cb) {
  tcp_write_cb = write_cb;
}

void VTcpBase::setReadCb(std::function<void(VTcp*, const VBuf*)> read_cb) {
  tcp_read_cb = read_cb;
}

void VTcpBase::close() {
  this->tcp->close(std::bind(&VTcpBase::on_close, this, std::placeholders::_1));
}


VTCP_WORKER_STATUS VTcpBase::getStatus() {
  return this->status;
}

void VTcpBase::setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce) {
  if (isOnce) {
    this->status = vstatus;
  } else {
    this->status = static_cast<VTCP_WORKER_STATUS>(
        static_cast<int>(this->status) | static_cast<int>(vstatus));
  }
}

void VTcpBase::readData(VTcp* client, const VBuf& data) {
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
#endif  // _DEBUG

  if (tcp_read_cb)
    tcp_read_cb(client, &data);
}