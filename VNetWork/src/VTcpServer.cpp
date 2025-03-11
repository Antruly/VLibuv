#include "VTcpServer.h"
#include <algorithm>

VTcpServer::VTcpServer(size_t numThreads,
                       size_t maxThreads,
                       size_t minIdleThreads,
                       size_t maxIdleThreads,
                       size_t maxTaskQueueSize)
    : VObject(),
      threadpool(nullptr),
      tcp(nullptr),
      loop(nullptr),
      idle_run(false)
{
  this->loop = new VLoop();
  this->tcp = new VTcp(loop);
  this->idle = new VIdle(loop);
  this->timer = new VTimer(loop);

  this->threadpool = new VThreadPool(
      numThreads, maxThreads, minIdleThreads, maxIdleThreads, maxTaskQueueSize);
   this->timer->start(
      std::bind(&VTcpServer::on_timer, this, std::placeholders::_1), 0, 500);
}

VTcpServer::~VTcpServer() {
  this->timer->stop();

  delete this->threadpool;
  
  delete this->timer;
  if (idle_run) {
    this->idleClose();
  }
  delete this->idle;
  delete this->tcp;
  delete this->loop;
}

VTcp* VTcpServer::getVTcp() {
  return this->tcp;
}
VLoop* VTcpServer::getVLoop() {
  return this->loop; }
int VTcpServer::setData(void *pdata) {
  vdata = pdata;
  return 0;
}
void *VTcpServer::getData() { return vdata; }
VIdle* VTcpServer::getVIdle() {
  return this->idle;
}
int VTcpServer::run(uv_run_mode md) {
  return this->loop->run(md);
}
void VTcpServer::idleStart() {
  this->idle_run = true;
  this->idle->start(std::bind(&VTcpServer::on_idle, this, std::placeholders::_1));
}

void VTcpServer::idleClose() {
  if (!this->idle->isClosing()) {
    this->idle_run = false;
    this->idle->stop();
  }
}
void VTcpServer::setIdleCb(std::function<void()> idle_cb) {
  tcp_idle_cb = idle_cb;
}
void VTcpServer::setCloseCb(std::function<void(VTcp*)> close_cb) {
  this->tcp_close_cb = close_cb;
}

void VTcpServer::close() {
  if (this->tcp != nullptr && !this->tcp->isClosing()) {
	  this->setStatus(VTCP_WORKER_STATUS_CLOSING, true);
	  this->tcp->close(
		  std::bind(&VTcpServer::on_close, this, std::placeholders::_1));
	  return;
  }
  else {
	  if (this->loop != nullptr && loop->isActive()) {
		  loop->stop();
	  }
  }
  this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);
  
}

void VTcpServer::stop() { this->loop->stop(); }

VTCP_WORKER_STATUS VTcpServer::getStatus() {
  return this->status;
}

VThreadPool* VTcpServer::getVThreadPool() {
  return threadpool;
}

void VTcpServer::setStatus(VTCP_WORKER_STATUS vstatus, bool isOnce) {
  if (isOnce) {
    this->status = vstatus;
  } else {
    this->status = static_cast<VTCP_WORKER_STATUS>(
        static_cast<int>(this->status) | static_cast<int>(vstatus));
  }
}

void VTcpServer::removStatus(VTCP_WORKER_STATUS vstatus) {
  this->status = static_cast<VTCP_WORKER_STATUS>(
      static_cast<int>(this->status) & ~static_cast<int>(vstatus));
}

void VTcpServer::on_timer(VTimer* vtimer) {

}

void VTcpServer::on_idle(VIdle* vidle) {
  if (tcp_idle_cb)
    tcp_idle_cb();
}

void VTcpServer::on_close(VHandle* client) {

  if (tcp_close_cb)
    tcp_close_cb(reinterpret_cast<VTcp *>(client));

  this->setStatus(VTCP_WORKER_STATUS_CLOSED, true);
  this->loop->stop();
}

void VTcpServer::on_new_connection(VStream* tcp, int status) {
  if (status != 0) {
    STD_L_ZERO_ERROR_SHOW(status, "Failed to establish new connection");
    return;
  }

  // 尝试创建客户端并接受连接
  if (new_client_cb)
  {
    VTcpClient* tcpClient = nullptr;
    try {
      tcpClient = new VTcpClient();
      VTcp* client = tcpClient->getVTcp();

      if (client == nullptr) {
        throw std::runtime_error("Failed to get VTcp from VTcpClient");
      }

      client->setData(tcpClient);
      if (tcp->accept(client) != 0)
        throw std::runtime_error("Failed to accept new client");

      tcpClient->setStatus(VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED);

      // 挂起队列回调
     bool isOk = threadpool->enqueue([this, tcpClient] {
        this->newClient(tcpClient);  // For multi-threaded client mode
          });
      if (!isOk) {
       Log->logWarn("threadpool enqueue is callback false, when use new thread!\n");
       std::thread newTD([this, tcpClient] {
         this->newClient(tcpClient); // For multi-threaded client mode
       });
       newTD.detach();
      }

    } catch (std::exception& e) {
     
      if (tcpClient != nullptr) {
        delete tcpClient;  // Safely delete if it was created
      }
      STD_L_ZERO_ERROR_SHOW(-1, e.what());
    }
  } else {
    return;
  }
}

int VTcpServer::listenIpv4(const char* addripv4, int port, int flags) {
#ifdef _WIN32
  // 获取当前线程句柄
  HANDLE hThread = GetCurrentThread();

  // 设置线程优先级为高
  BOOL success = SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
#ifdef _DEBUG
  if (success) {
    Log->logInfo("Thread priority set to above normal.");
  } else {
    Log->logError("Failed to set thread priority.");
    return 1;
  }
#endif
#else

  pthread_t thread;
  int policy;
  struct sched_param param;

  // 获取当前线程的调度策略和参数
  if (pthread_getschedparam(pthread_self(), &policy, &param) != 0) {
    Log->logError("pthread_getschedparam error");
  }

  if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
    Log->logError("pthread_setschedparam error");
  }

#endif

  this->removStatus(static_cast<VTCP_WORKER_STATUS>(VTCP_WORKER_STATUS_CLOSED |
                                                    VTCP_WORKER_STATUS_NONE));
  this->setStatus(VTCP_WORKER_STATUS_STARTING);
  int ret;
  ret = this->getVTcp()->bindIpv4(addripv4, port, flags);
  if (ret != 0)
  {
	  this->setStatus(VTCP_WORKER_STATUS_ERROR_UNKNOWN);
  }
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "bindIpv4");
  
  ret = this->getVTcp()->listen(
      std::bind(&VTcpServer::on_new_connection, this, std::placeholders::_1,
                std::placeholders::_2),
      128);
  if (ret != 0)
  {
	  this->setStatus(VTCP_WORKER_STATUS_ERROR_UNKNOWN);
  }
  STD_NO_ZERO_ERROR_SHOW_INT(ret, "listenIpv4");

  this->setStatus(VTCP_WORKER_STATUS_LISTENING);
  return ret;
}


void VTcpServer::closeClient(VTcpClient* tcpClient) {
  tcpClient->close();
}


void VTcpServer::setNewClientCb(
    std::function<void(VTcpClient*)> new_client_cb) {
  this->new_client_cb = new_client_cb;
}

void VTcpServer::newClient(VTcpClient* client) {
  this->new_client_cb(client);
}
