#pragma once
#include "VHttpServer.h"

VHttpServer::VHttpServer(size_t numThreads, size_t maxThreads,
                         size_t minIdleThreads, size_t maxIdleThreads,
                         size_t maxTaskQueueSize) {
  this->tcp_server_ = new VTcpServer(numThreads, maxThreads, minIdleThreads,
                                     maxIdleThreads, maxTaskQueueSize);

  this->tcp_server_->setCloseCb(std::bind(&VHttpServer::on_server_tcp_close,
                                          this, std::placeholders::_1));

  this->tcp_server_->setNewClientCb(
      std::bind(&VHttpServer::on_new_tcp_client, this, std::placeholders::_1));
}

VHttpServer::VHttpServer(VTcpServer *tcpServer) {
  this->tcp_server_ = tcpServer;

   this->tcp_server_->setCloseCb(std::bind(&VHttpServer::on_server_tcp_close,
                                          this, std::placeholders::_1));

  this->tcp_server_->setNewClientCb(
       std::bind(&VHttpServer::on_new_tcp_client, this, std::placeholders::_1));
}

VHttpServer::~VHttpServer() {

  if (this->tcp_server_ != nullptr) {
    delete this->tcp_server_;
    this->tcp_server_ = nullptr;
  }
}

int VHttpServer::run(uv_run_mode md) { return this->tcp_server_->run(md); }

void VHttpServer::close() { this->tcp_server_->close(); }

int VHttpServer::listenIpv4(const char *addripv4, int port, int flags) {
  return this->tcp_server_->listenIpv4(addripv4, port, flags);
}

VTcpServer *VHttpServer::getVTcpServer() { return tcp_server_; }

void VHttpServer::setCloseCb(std::function<void(VTcpServer *)> close_cb) {
  this->http_server_close_cb = close_cb;
}

void VHttpServer::setNewHttpClientCb(
    std::function<void(VHttpClient *)> new_http_client_cb) {
  this->http_server_new_http_client_cb = new_http_client_cb;
}

void VHttpServer::on_server_tcp_close(VTcp *tcp) { this->httpServerClose(this->tcp_server_); }

void VHttpServer::on_new_tcp_client(VTcpClient *client) {
  VHttpClient *httpClient = new VHttpClient(client, true);
  // 预留先判断是否https
  if (false) {
    httpClient->initCallback(true);
  } else {
    httpClient->initCallback(false);
  }
 
  this->newHttpClient(httpClient);
}



void VHttpServer::httpServerClose(VTcpServer *server) {
  this->http_server_close_cb(server);
}

void VHttpServer::newHttpClient(VHttpClient *client) {
  this->http_server_new_http_client_cb(client);
}