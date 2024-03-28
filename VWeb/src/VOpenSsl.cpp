#include "VOpenSsl.h"
#include <cassert>

VOpenSsl::VOpenSsl(VTcpClient* tcpClient, const SSL_METHOD* method)
    : VOpenSsl::VOpenSsl(method) {
  initClient(tcpClient);
}

VOpenSsl::VOpenSsl(VTcpServer* tcpServer, const SSL_METHOD* method)
    : VOpenSsl::VOpenSsl(method) {
  initServer(tcpServer);
}

VOpenSsl::VOpenSsl(const SSL_METHOD* method)
    : tcp_client_(nullptr), tcp_server_(nullptr) {
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  try {
    ssl_ctx_ = SSL_CTX_new(method);
    if (!ssl_ctx_) {
      throw std::runtime_error("Failed to create SSL context");
    }

    ssl_ = SSL_new(ssl_ctx_);
    if (!ssl_) {
      SSL_CTX_free(ssl_ctx_);
      ssl_ctx_ = nullptr;
      throw std::runtime_error("Failed to create SSL");
    }

    read_bio_ = BIO_new(BIO_s_mem());
    write_bio_ = BIO_new(BIO_s_mem());
    if (!read_bio_ || !write_bio_) {
      if (read_bio_ != nullptr) {
        BIO_free(read_bio_);
        read_bio_ = nullptr;
      } else if (write_bio_ != nullptr) {
        BIO_free(write_bio_);
        write_bio_ = nullptr;
      }
      SSL_free(ssl_);
      ssl_ = nullptr;
      SSL_CTX_free(ssl_ctx_);
      ssl_ctx_ = nullptr;
      throw std::runtime_error("Failed to create BIOs");
    }
    SSL_set_bio(ssl_, read_bio_, write_bio_);

  } catch (const std::runtime_error& e) {
    // Handle initialization failure
  }

  cache_read_buf_ = (char*)VMemory::malloc(VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
  memset(cache_read_buf_, 0, VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
  cache_write_buf_ = (char*)VMemory::malloc(VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
  memset(cache_write_buf_, 0, VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
}

VOpenSsl::~VOpenSsl() {
  read_data_cache_.clear();
  write_data_cache_.clear();
  read_data_.clear();

  SSL_free(ssl_);
  ssl_ = nullptr;
  write_bio_ = nullptr;
  read_bio_ = nullptr;

  SSL_CTX_free(ssl_ctx_);
  ssl_ctx_ = nullptr;
  EVP_cleanup();

  VMemory::free(cache_read_buf_);
  cache_read_buf_ = nullptr;
  VMemory::free(cache_write_buf_);
  cache_write_buf_ = nullptr;
}

bool VOpenSsl::sslConnect() {
  read_data_cache_.clear();
  write_data_cache_.clear();
  this->initCallback();
  assert(ssl_ != nullptr);
  SSL_set_connect_state(ssl_);
  SSL_CTX_set_mode(ssl_ctx_, 3);
  int ret = SSL_connect(ssl_);
  if (ret == 1) {
    // Connection established
    if (ssl_connectiond_cb_)
      ssl_connectiond_cb_(0);  // Call connection success callback
    return true;
  } else {
    int err = this->sslGetError(ret);
    switch (err) {
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        this->invokeRetEvent(ret);
        break;
      default:
        // Handle other errors
        error_massage_ = "SSL connect error";
        if (ssl_connectiond_cb_)
          ssl_connectiond_cb_(-1);  // Call connection failure callback
        return false;
    }
  }

  do {
    this->invokeRetEvent(ret);
  } while (!this->sslIsInitFinished());

  if (ssl_connectiond_cb_)
    ssl_connectiond_cb_(0);

  ssl_have_write_ = true;
  return true;
}

std::string VOpenSsl::getErrorMassage() {
  return error_massage_;
}

void VOpenSsl::setSslConnectiondCb(std::function<void(int)> connectiond_cb) {
  ssl_connectiond_cb_ = connectiond_cb;
}

void VOpenSsl::setSslWriteCb(std::function<void(const VBuf*, int)> write_cb) {
  ssl_write_cb_ = write_cb;
}

void VOpenSsl::setSslReadCb(std::function<void(const VBuf*)> read_cb) {
  ssl_read_cb_ = read_cb;
}

bool VOpenSsl::initClient(VTcpClient* tcpClient) {
  tcp_client_ = tcpClient;
  this->ssl_connectiond_cb_ = nullptr;
  this->ssl_read_cb_ = nullptr;
  this->ssl_write_cb_ = nullptr;
  assert(ssl_ != nullptr);
  return true;
}

bool VOpenSsl::initServer(VTcpServer* tcpServer) {
  tcp_server_ = tcpServer;
  this->ssl_read_cb_ = nullptr;
  this->ssl_write_cb_ = nullptr;
  assert(ssl_ != nullptr);
  return true;
}

void VOpenSsl::initCallback() {
  if (tcp_client_ != nullptr) {
    tcp_client_->setReadCb([this](VTcpClient* tcpClient, const VBuf* buf) {
      read_data_ = *buf;
      VBuf readBuf;
      if (!this->sslParserData(*buf, readBuf)) {
        return;
      }
    });

    tcp_client_->setWriteCb(
        [this](VTcpClient* tcpClient, const VBuf* buf, int status) {
          if (status < 0) {
            error_massage_ = "ssl write error status:" + std::to_string(status);
          }
          if (this->ssl_write_cb_)
            this->ssl_write_cb_(buf, status);
        });

    tcp_client_->setCloseCb([this](VTcpClient* tcpClient) {
      // Handle close event
    });
  }
}

bool VOpenSsl::sslIsInitFinished() {
  assert(ssl_ != nullptr);
  return SSL_is_init_finished(ssl_);
}

bool VOpenSsl::sslPackData(const VBuf& srcData, VBuf& outData) {
  read_data_cache_.clear();
  write_data_cache_.clear();
  int ret = SSL_write(ssl_, srcData.getConstData(), srcData.size());
  if (ret > 0) {
    outData.clear();
    this->getWriteBioData(outData);
    return true;
  } else if (ret == 0) {
    error_massage_ = "sslPackData is error";
    return false;
  } else {
    this->invokeRetEvent(ret);
    return true;
  }
  return false;
}

bool VOpenSsl::sslParserData(const VBuf& srcData, VBuf& outData) {
  read_data_cache_.clear();
  write_data_cache_.clear();
  int ret = BIO_write(read_bio_, srcData.getConstData(), srcData.size());
  if (ret > 0) {
    outData.clear();
    this->getReadSslData(outData);
    return true;
  } else if (ret == 0) {
    error_massage_ = "sslPackData is error";
    return false;
  } else {
    this->invokeRetEvent(ret);
    return true;
  }
  return false;
}

void VOpenSsl::getWriteBioData(VBuf& data) {
  int readIndex = 0;
  VBuf dtBuf;
  do {
    readIndex =
        BIO_read(write_bio_, cache_write_buf_, VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
    if (readIndex > 0) {
      dtBuf.appandData(cache_write_buf_, readIndex);
    }
  } while (readIndex > 0);
  data.appand(dtBuf);

  if (tcp_client_ != nullptr) {
    tcp_client_->writeData(dtBuf);

    if (!tcp_client_->getVLoop()->isActive()) {
      tcp_client_->getVLoop()->run(uv_run_mode::UV_RUN_ONCE);
      this->tcpClientReadData();
    }
  } else if (tcp_server_ != nullptr) {
    // #server
    if (dtBuf.size() > 0) {
      if (ssl_write_cb_)
        ssl_write_cb_(&dtBuf, 0);
    }
  } else {
    if (dtBuf.size() > 0) {
      if (ssl_write_cb_)
        ssl_write_cb_(&dtBuf, 0);
    }
  }
  dtBuf.clear();
  ssl_have_write_ = false;
}

void VOpenSsl::getReadSslData(VBuf& data) {
  int readIndex = 0;
  VBuf dtBuf;
  do {
    readIndex = SSL_read(ssl_, cache_read_buf_, VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE);
    if (readIndex > 0) {
      dtBuf.appandData(cache_read_buf_, readIndex);
    }
  } while (readIndex > 0);

  data.appand(dtBuf);

  if (dtBuf.size() > 0) {
    if (ssl_read_cb_)
      ssl_read_cb_(&dtBuf);
  }

  dtBuf.clear();
  ssl_have_read_ = false;
}

int VOpenSsl::sslGetError(int ret) {
  return SSL_get_error(ssl_, ret);
}

void VOpenSsl::invokeRetEvent(int ret) {
  int err = sslGetError(ret);
  if (err == SSL_ERROR_WANT_READ) {
    ssl_have_read_ = true;
    read_data_cache_.clear();
    getReadSslData(read_data_cache_);

    if (read_data_cache_.size() == 0) {
      ssl_have_read_ = false;
      ssl_have_write_ = true;
      write_data_cache_.clear();
      getWriteBioData(write_data_cache_);
    }

  } else if (err == SSL_ERROR_WANT_WRITE) {
    ssl_have_write_ = true;
    write_data_cache_.clear();
    getWriteBioData(write_data_cache_);
  }
}

size_t VOpenSsl::tcpClientReadData(uint64_t maxTimeout) {
  read_data_.clear();
  tcp_client_->clientReadStart();

  size_t readIndex = 0;
  uint64_t timeout = 0;
  while (true) {
    if (timeout > maxTimeout) {
      break;
    }
    tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);
    if (read_data_.size() > 0) {
      readIndex += read_data_.size();
      read_data_.clear();
      timeout = 0;
      continue;
    } else if (read_data_.size() == 0 && readIndex != 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      tcp_client_->run(uv_run_mode::UV_RUN_NOWAIT);
      if (read_data_.size() > 0) {
        readIndex += read_data_.size();
        read_data_.clear();
        timeout = 0;
        continue;
      } else {
        break;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout += 10;
  }

  return readIndex;
}

bool VOpenSsl::sslHaveRead() const {
  return ssl_have_read_;
}

bool VOpenSsl::sslHaveWrite() const {
  return ssl_have_write_;
}

const SSL_METHOD* VOpenSsl::getSslMethod() const {
  return ssl_method_;
}
