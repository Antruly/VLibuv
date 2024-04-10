#pragma once

#include <VBuf.h>
#include <VTcpClient.h>
#include <VTcpServer.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>
#include "VObject.h"
#include "VWebDefine.h"

/**
 * @brief VOpenSsl类用于提供基于OpenSSL的安全套接字功能。
 */
class VOpenSsl : public VObject {
 public:
  /**
   * @brief 构造函数，用于客户端模式。
   * @param tcpClient VTcpClient指针，用于底层网络通信。
   * @param method SSL_METHOD指针，指定SSL/TLS协议方法。
   */
  VOpenSsl(VTcpClient* tcpClient, const SSL_METHOD* method);

  /**
   * @brief 构造函数，用于服务器模式。
   * @param tcpServer VTcpServer指针，用于底层网络通信。
   * @param method SSL_METHOD指针，指定SSL/TLS协议方法。
   */
  VOpenSsl(VTcpServer* tcpServer, const SSL_METHOD* method);

  /**
   * @brief 构造函数，用于独立模式，不依赖网络通信。
   * @param method SSL_METHOD指针，指定SSL/TLS协议方法。
   */
  VOpenSsl(const SSL_METHOD* method);

  /**
   * @brief 析构函数，释放资源。
   */
  ~VOpenSsl();

  /**
   * @brief 初始化客户端SSL连接。
   * @param tcpClient VTcpClient指针，用于底层网络通信。
   * @return 初始化是否成功。
   */
  bool initClient(VTcpClient* tcpClient);

  /**
   * @brief 初始化服务器SSL连接。
   * @param tcpServer VTcpServer指针，用于底层网络通信。
   * @return 初始化是否成功。
   */
  bool initServer(VTcpServer* tcpServer);

  /**
   * @brief 初始化回调函数。
   */
  void initCallback();

  /**
   * @brief 检查SSL连接是否初始化完成。
   * @return 是否完成初始化。
   */
  bool sslIsInitFinished();

  /**
   * @brief 建立SSL连接。
   * @return 连接是否成功建立。
   */
  bool sslConnect();

  /**
   * @brief 打包数据为SSL加密数据。
   * @param srcData 源数据。
   * @param outData 加密后数据。
   * @return 打包是否成功。
   */
  bool sslPackData(const VBuf& srcData, VBuf& outData);

  /**
   * @brief 解析SSL加密数据。
   * @param srcData 加密数据。
   * @param outData 解析后数据。
   * @return 解析是否成功。
   */
  bool sslParserData(const VBuf& srcData, VBuf& outData);

  /**
   * @brief 获取待写入SSL加密数据的缓存。
   * @param data 待写入数据。
   */
  void getWriteBioData(VBuf& data);

  /**
   * @brief 获取待读取SSL解密数据的缓存。
   * @param data 待读取数据。
   */
  void getReadSslData(VBuf& data);

  /**
   * @brief 检查是否有可读数据。
   * @return 是否有可读数据。
   */
  bool sslHaveRead() const;

  /**
   * @brief 检查是否有可写数据。
   * @return 是否有可写数据。
   */
  bool sslHaveWrite() const;

  const SSL_METHOD* getSslMethod() const;

  /**
   * @brief 获取错误消息。
   * @return 错误消息。
   */
  std::string getErrorMassage();

  /**
   * @brief 设置SSL连接回调函数。
   * @param connectiond_cb 连接回调函数。
   */
  void setSslConnectiondCb(std::function<void(VOpenSsl*, int)> connectiond_cb);

  /**
   * @brief 设置SSL写入回调函数。
   * @param write_cb 写入回调函数。
   */
  void setSslWriteCb(std::function<void(VOpenSsl*, const VBuf*, int)> write_cb);

  /**
   * @brief 设置SSL读取回调函数。
   * @param read_cb 读取回调函数。
   */
  void setSslReadCb(std::function<void(VOpenSsl*, const VBuf*)> read_cb);

 protected:
  /**
   * @brief 获取SSL错误码。
   * @param ret 返回值。
   * @return 错误码。
   */
  int sslGetError(int ret);

  /**
   * @brief 调用返回事件。
   * @param ret 返回值。
   */
  int invokeRetEvent(int ret);

  /**
   * @brief 从TCP客户端读取数据。
   * @param maxTimeout 最大超时时间。
   * @return 读取数据大小。
   */
  size_t tcpClientReadData(uint64_t maxTimeout = 30000);

 protected:
  std::function<void(VOpenSsl*, int)>
      ssl_connectiond_cb_; /**< SSL连接回调函数。 */

  std::function<void(VOpenSsl*, const VBuf*, int)>
      ssl_write_cb_; /**< SSL写入回调函数。 */

  std::function<void(VOpenSsl*, const VBuf*)>
      ssl_read_cb_; /**< SSL读取回调函数。 */

 private:
  const SSL_METHOD* ssl_method_ = nullptr;
  VTcpClient* tcp_client_ = nullptr; /**< TCP客户端指针。 */
  VTcpServer* tcp_server_ = nullptr; /**< TCP服务器指针。 */
  SSL* ssl_ = nullptr;               /**< SSL指针。 */
  SSL_CTX* ssl_ctx_ = nullptr;       /**< SSL上下文指针。 */
  BIO* read_bio_ = nullptr;          /**< 读取BIO。 */
  BIO* write_bio_ = nullptr;         /**< 写入BIO。 */
  char* cache_read_buf_ = nullptr;   /**< 读取缓存。 */
  char* cache_write_buf_ = nullptr;  /**< 写入缓存。 */
  std::string error_massage_;        /**< 错误消息。 */
  VBuf read_data_cache_;             /**< 读取数据缓存。 */
  VBuf write_data_cache_;            /**< 写入数据缓存。 */
  VBuf read_data_;                   /**< 读取数据。 */
  bool ssl_have_read_ = false;       /**< 是否有可读数据标志。 */
  bool ssl_have_write_ = false;      /**< 是否有可写数据标志。 */
};
