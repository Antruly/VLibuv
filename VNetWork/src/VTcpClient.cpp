﻿#include "VTcpClient.h"
#include <cassert>
#include <VTimer.h>

VTcpClient::VTcpClient()
    : addr(),
      alloc_buffers(),
      tcp(nullptr),
      loop(nullptr),
      buffer_cache(nullptr),
      idle_run(false),
      tcp_run(false),
      own_loop(true),
	  read_start(false),
	  status(VTCP_WORKER_STATUS_NONE){
  this->loop = new VLoop();
  this->tcp = new VTcp(loop);
  this->idle = new VIdle(loop);
  this->buffer_cache = new VBuf();
  this->initCallBackup();
  // buffer_cache->resize(65536);
}

// 新的构造函数，接受外部VLoop指针
VTcpClient::VTcpClient(VLoop* externalLoop)
    : addr(),
      alloc_buffers(),
      tcp(nullptr),
      buffer_cache(nullptr),
      loop(externalLoop),
      idle_run(false),
      tcp_run(false),
      own_loop(false),
	  read_start(false), 
	  status(VTCP_WORKER_STATUS_NONE){
  if (loop) {
    this->tcp = new VTcp(loop);
    this->idle = new VIdle(loop);

    this->buffer_cache = new VBuf();
    // buffer_cache->resize(65536);
  }
  this->initCallBackup();
}

VTcpClient::~VTcpClient() {
  this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);

  if (own_loop && loop) {
	 /* loop->walk(
		  [this](VHandle* handle, void* data) {
		  if (handle == this->tcp)
		  {
			  if (this->tcp_run) {
				  this->close();
			  }
			  else{
				  loop->stop();
			  }
		  }
		  else{
			  handle->close();
		  }
	  },
		  nullptr);

	  if (!loop->isActive()) {
		  loop->run();
	  }*/

    // 如果是自管理的VLoop，需要在这里释放资源
    if (!this->loop->isClosing() && this->loop->isActive()) {
      this->loop->close();
    }
    delete this->loop;
	this->loop = nullptr;
  }

  if (this->idle != nullptr) {
    if (this->idle_run) {
      this->idleClose();
    }
    delete this->idle;
	this->idle = nullptr;
  }
  if (this->tcp != nullptr) {
    if (this->tcp_run) {
      this->close();
	  this->tcp_run = false;
    }
	delete this->tcp;
	this->tcp = nullptr;
  }

 
  if (alloc_buffers.size() > 0) {
    // 遍历并删除VBuf*指针
    for (auto it = alloc_buffers.begin(); it != alloc_buffers.end(); ++it) {
      // 释放VBuf*指针指向的内存
      delete it->first;
    }
    alloc_buffers.clear();
  }

  if (this->buffer_cache != nullptr) {
    buffer_cache->clear();
    delete buffer_cache;
  }

}
void VTcpClient::on_idle(VIdle* vidle) {
  if ((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSING) ==
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSING) {
    if (this->idle_run) {
      this->idleClose();
    }
  }
  if (tcp_idle_cb)
    tcp_idle_cb();
}

void VTcpClient::on_close(VHandle* client) {
  this->read_start = false;
  this->tcp_run = false;
  this->async_close = true;
  this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);
  if (tcp_close_cb)
    tcp_close_cb(this);
}

void VTcpClient::echo_write(VWrite* req, int status) {
  this->tcp_run = true;
  this->removStatus(VTCP_WORKER_STATUS_PROCESSING);
  if (status) {
    Log->logDebug("Write error %s\n", uv_strerror(status));
  }
  if (tcp_write_cb)
    tcp_write_cb(this, req->getSrcBuf(), status);

  if (req->getBuf() != nullptr)
    delete req->getBuf();
  delete req;

  this->async_write = true;
}

void VTcpClient::alloc_buffer(VHandle* handle,
                              size_t suggested_size,
                              VBuf* buf) {
  if (tcp_alloc_buffer_cb)
    tcp_alloc_buffer_cb(this, suggested_size);

  if (buffer_cache != nullptr) {
    buffer_cache->resize(suggested_size);
    buf->setData(buffer_cache->getConstData(), suggested_size, false);
  }
  // alloc_buffers.insert(std::make_pair(buf, buffer_num++));
}

void VTcpClient::echo_read(VStream* client, ssize_t nread, const VBuf* buf) {
  this->tcp_run = true;
  /* auto it = alloc_buffers.find(const_cast<VBuf*>(buf));
   if (it != alloc_buffers.end()) {
     alloc_buffers.erase(it);
   }*/
  if (nread > 0) {
    VBuf bf;
    bf.setData(buf->getData(), nread, false);
    this->readData(reinterpret_cast<VTcp*>(client), bf);
    bf.setData(nullptr, 0, false);
    const_cast<VBuf*>(buf)->setZero();
    const_cast<VBuf*>(buf)->setData(nullptr, 0, false);
    // bf.clean();

  } else if (nread < 0) {

    if (nread != UV_EOF) {
      Log->logDebug("Read error %sn", uv_err_name(nread));
    } else {
      Log->logDebugWarn("client disconnectn");
    }

    client->close(
        std::bind(&VTcpClient::on_close, this, std::placeholders::_1));
    // const_cast<VBuf*>(buf)->clean();
    const_cast<VBuf*>(buf)->setZero();
    const_cast<VBuf*>(buf)->setData(nullptr, 0, false);
    if (client == tcp) {
      this->removStatus(static_cast<VTCP_WORKER_STATUS>(
          VTCP_WORKER_STATUS_READING | VTCP_WORKER_STATUS_WRITING |
          VTCP_WORKER_STATUS_CONNECTED));
      this->setStatus(VTCP_WORKER_STATUS_ERROR_ECONNRESET);
    }
  }
}
VTcp* VTcpClient::getVTcp() {
  return this->tcp;
}
VLoop* VTcpClient::getVLoop() {
  return this->loop;
}
sockaddr_storage VTcpClient::getLocalAddrs(std::string& ip, int& port) {
  struct sockaddr_storage local_addr;
  memset(&local_addr, 0, sizeof(local_addr));
  int local_addr_len = sizeof(local_addr);
  if (tcp == nullptr) {
    return local_addr;
  }

  tcp->getsockname((struct sockaddr*)&local_addr, &local_addr_len);

  char local_ip[INET6_ADDRSTRLEN];
  int local_port;

  if (local_addr.ss_family == AF_INET) {
    uv_ip4_name((struct sockaddr_in*)&local_addr, local_ip, sizeof(local_ip));
    ip = local_ip;

    local_port = ntohs(((struct sockaddr_in*)&local_addr)->sin_port);
    port = local_port;
  } else if (local_addr.ss_family == AF_INET6) {
    uv_ip6_name((struct sockaddr_in6*)&local_addr, local_ip, sizeof(local_ip));
    ip = local_ip;
    local_port = ntohs(((struct sockaddr_in6*)&local_addr)->sin6_port);
    port = local_port;
  }
  return local_addr;
}
sockaddr_storage VTcpClient::getPeerAddrs(std::string& ip, int& port) {
  struct sockaddr_storage peer_addr;
  memset(&peer_addr, 0, sizeof(peer_addr));
  int peer_addr_len = sizeof(peer_addr);
  if (tcp == nullptr) {
    return peer_addr;
  }
  tcp->getpeername((struct sockaddr*)&peer_addr, &peer_addr_len);

  char peer_ip[INET6_ADDRSTRLEN];
  int peer_port;

  if (peer_addr.ss_family == AF_INET) {
    uv_ip4_name((struct sockaddr_in*)&peer_addr, peer_ip, sizeof(peer_ip));
    ip = peer_ip;
    peer_port = ntohs(((struct sockaddr_in*)&peer_addr)->sin_port);
    port = peer_port;
  } else if (peer_addr.ss_family == AF_INET6) {
    uv_ip6_name((struct sockaddr_in6*)&peer_addr, peer_ip, sizeof(peer_ip));
    peer_port = ntohs(((struct sockaddr_in6*)&peer_addr)->sin6_port);
  }
  return peer_addr;
}
int VTcpClient::setData(void *pdata) {
  vdata = pdata;
  return 0;
}
void *VTcpClient::getData() { return vdata; }
VIdle* VTcpClient::getVIdle() {
  return this->idle;
}
int VTcpClient::run(uv_run_mode md) { return this->loop->run(md); }
void VTcpClient::initCallBackup() {
 

}
void VTcpClient::stop() { this->loop->stop(); }
void VTcpClient::idleStart() {
  this->idle_run = true;
  this->idle->start(
      std::bind(&VTcpClient::on_idle, this, std::placeholders::_1));
}

void VTcpClient::idleClose() {
  if (!this->idle->isClosing()) {
    this->idle_run = false;
    this->idle->stop();
  }
}

void VTcpClient::setIdleCb(std::function<void()> idle_cb) {
  tcp_idle_cb = idle_cb;
}
void VTcpClient::setCloseCb(std::function<void(VTcpClient*)> close_cb) {
  this->tcp_close_cb = close_cb;
}

void VTcpClient::setWriteCb(
    std::function<void(VTcpClient*, const VBuf*, int)> write_cb) {
  tcp_write_cb = write_cb;
}

void VTcpClient::setReadCb(
    std::function<void(VTcpClient*, const VBuf*)> read_cb) {
  tcp_read_cb = read_cb;
}

void VTcpClient::close() {
  this->async_close = false;
  if (this->tcp != nullptr && this->tcp->isClosing()) {
    this->setStatus(VTCP_WORKER_STATUS_CLOSING, true);
    if (this->loop != nullptr && loop->isActive()) {
      loop->stop();
    }
    this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);
    this->async_close = true;
    if (tcp_close_cb)
      tcp_close_cb(this);
    return;
  } else if (this->tcp != nullptr && !this->tcp->isClosing()) {
    this->setStatus(VTCP_WORKER_STATUS_CLOSING, true);
    this->tcp->close(
        std::bind(&VTcpClient::on_close, this, std::placeholders::_1));
    while (true) {
      if (!this->loop->isActive())
        this->run(uv_run_mode::UV_RUN_NOWAIT);

      if (((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) >
           0) ||
          this->getStatus() >
              VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_ERROR_NONE) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return;
  } else {
    this->setStatus(VTCP_WORKER_STATUS_CLOSING, true);
    if (this->loop != nullptr && loop->isActive()) {
      loop->stop();
    }
    this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);
    this->async_close = true;
    if (tcp_close_cb)
      tcp_close_cb(this);
  }
}

VTCP_WORKER_STATUS VTcpClient::getStatus() {
  return this->status;
}

void VTcpClient::setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce) {
  if (isOnce) {
    this->status = vstatus;
  } else {
    this->status = static_cast<VTCP_WORKER_STATUS>(
        static_cast<int>(this->status) | static_cast<int>(vstatus));
  }
}

void VTcpClient::removStatus(VTCP_WORKER_STATUS vstatus) {
  this->status = static_cast<VTCP_WORKER_STATUS>(
      static_cast<int>(this->status) & ~static_cast<int>(vstatus));
}

void VTcpClient::readData(VTcp* client, const VBuf& data) {
#ifdef _DEBUG
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  client->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    VBuf newData;
    newData.clone(data);
    newData.resize(data.size() + 1);
    Log->logDebug("IP: %s, Port: %d reaData \n", peer_ip, ntohs(addrs.sin_port));
    newData.clear();
  } else {
    Log->logDebugError("Failed to get peer IP address\n");
  }
#endif
#endif
#endif  // _DEBUG
  this->setStatus(VTCP_WORKER_STATUS_READING);
  if (tcp_read_cb)
    tcp_read_cb(this, &data);
  this->removStatus(VTCP_WORKER_STATUS_READING);
}

void VTcpClient::on_connection(VConnect* req, int status) {
  // STD_L_ZERO_ERROR_SHOW(status, "connection");
  this->removStatus(VTCP_WORKER_STATUS_CONNECTING);

  VTcp* tcp = reinterpret_cast<VTcp*>(req->getData());

#ifdef _DEBUG
  char ip[INET6_ADDRSTRLEN] = {0};
  int port;

  uv_ip4_name(reinterpret_cast<sockaddr_in*>(&addr), ip, sizeof(ip));
  port = ntohs(reinterpret_cast<sockaddr_in*>(&addr)->sin_port);
  Log->logDebug("connection ip:%s port:%d \n", ip, port);
#endif  // endif

  if (status < 0) {
    this->setStatus(VTCP_WORKER_STATUS_ERROR_UNKNOWN, true);

    Log->logDebugError("on_connection error %s\n", uv_err_name(status));
  } else {
    this->setStatus(VTCP_WORKER_STATUS_CONNECTED);
    tcp->readStart(
        std::bind(&VTcpClient::alloc_buffer, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&VTcpClient::echo_read, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    this->setStatus(VTCP_WORKER_STATUS_WRITING);
  }

  this->connectiond(status);
  delete req;

    this->async_connect = true;
}

int VTcpClient::connect(const char* addripv4, int port) {
  this->removStatus(static_cast<VTCP_WORKER_STATUS>(
	  VTCP_WORKER_STATUS_DISCONNECTED | VTCP_WORKER_STATUS_CLOSED | VTCP_WORKER_STATUS_CLOSING |
      VTCP_WORKER_STATUS_NONE));
  this->async_connect = false;
  this->setStatus(VTCP_WORKER_STATUS_CONNECTING, true);
  int ret;
  tcp->init(loop);
  VConnect* req = new VConnect();
  req->setData(tcp);
  uv_ip4_addr(addripv4, port, &addr);
  this->tcp_run = true;
  ret = tcp->connect(req, reinterpret_cast<sockaddr*>(&addr),
                     std::bind(&VTcpClient::on_connection, this,
                               std::placeholders::_1, std::placeholders::_2));
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "connect");
  return ret;
}

int VTcpClient::writeData(const VBuf& data) {
  this->async_write = false;
  if ((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) ==
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) {
    if (tcp_write_cb)
      tcp_write_cb(this, &data, -1);
    return -1;
  }
  this->setStatus(VTCP_WORKER_STATUS_PROCESSING);

  VWrite* req = new VWrite();
  req->setBuf(nullptr);
  req->setSrcBuf(&data);
  req->setData(tcp);
  return tcp->write(req, &data, 1,
                    std::bind(&VTcpClient::echo_write, this,
                              std::placeholders::_1, std::placeholders::_2));
}

int VTcpClient::writeNewData(const VBuf& data) {
  this->async_write = false;
  if ((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) ==
      VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) {
    if (tcp_write_cb)
      tcp_write_cb(this, &data, -1);
    return -1;
  }
  this->setStatus(VTCP_WORKER_STATUS_PROCESSING);
  VWrite* req = new VWrite();
  const VBuf* bf = new VBuf(data);
  req->setBuf(bf);
  req->setSrcBuf(&data);
  req->setData(tcp);
  return tcp->write(req, bf, 1,
             std::bind(&VTcpClient::echo_write, this, std::placeholders::_1,
                       std::placeholders::_2));
}

void VTcpClient::setConnectiondCb(std::function<void(int)> connectiond_cb) {
  this->tcp_client_connectiond_cb = connectiond_cb;
}

int VTcpClient::clientReadStart() {
    if (!read_start) {
    read_start = true;
    return tcp->readStart(
        std::bind(&VTcpClient::alloc_buffer, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        std::bind(&VTcpClient::echo_read, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
  }
  
}

void VTcpClient::waitConnectFinish() {
  if ((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTING) == 0) {
    return;
  }

  while (true) {
     if (!this->loop->isActive())
       this->run(uv_run_mode::UV_RUN_NOWAIT);

     if (((this->getStatus() &
           VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTING) == 0) ||
         this->getStatus() > VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_ERROR_NONE) {
     break;
     }

     if (this->async_connect) {
       this->async_connect = false;
       break;
     }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void VTcpClient::waitCloseFinish() {
  if ((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) > 0) {
    return;
  }

   while (true) {
    if (!this->loop->isActive())
      this->run(uv_run_mode::UV_RUN_NOWAIT);

    if (((this->getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CLOSED) >
         0) ||
        this->getStatus() > VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_ERROR_NONE) {
      break;
    }

    if (this->async_close) {
      this->async_close = false;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}



void VTcpClient::waitWriteFinish() {
  if ((this->getStatus() & (VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED |
       VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_PROCESSING)) == 0) {
    return;
  }

  while (true) {
    if (!this->loop->isActive())
      this->run(uv_run_mode::UV_RUN_NOWAIT);

    if (((this->getStatus() &
          (VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED |
           VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_PROCESSING)) == 0) ||
        this->getStatus() > VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_ERROR_NONE) {
      break;
    }

    if (this->async_write) {
      this->async_write = false;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void VTcpClient::connectiond(int status) {
#ifdef _DEBUG
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  sockaddr_in addrs;
  int peer_ip_len = sizeof(addrs);
  tcp->getpeername((sockaddr*)&addrs, &peer_ip_len);
  char peer_ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(addrs.sin_addr), peer_ip, INET_ADDRSTRLEN)) {
    Log->logDebug("Client connected from IP: %s, Port: %d", peer_ip,
                          ntohs(addrs.sin_port));
  } else {
    Log->logError("Failed to get peer IP address");
  }

#endif
#endif
#endif  // _DEBUG
  if (tcp_client_connectiond_cb)
    this->tcp_client_connectiond_cb(status);

}
