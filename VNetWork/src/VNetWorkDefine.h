﻿#pragma once
// 最大 单个报文缓存长度
#define VNETWORK_MAX_ONE_DATA_CACHE_LENGTH 65536

// IPv4 地址长度，包括 NULL 终止符
#define VNETWORK_IP_ADDRESS_LENGTH 16

// IPv6 地址长度，包括 NULL 终止符
#define VNETWORK_IPV6_ADDRESS_LENGTH 46

// 主机名最大长度
#define VNETWORK_MAX_HOSTNAME_LENGTH 255

// IPv4 地址最大长度
#define VNETWORK_MAX_IPv4_LENGTH 15

// IPv6 地址最大长度
#define VNETWORK_MAX_IPv6_LENGTH 45

// TCP/UDP 端口号最大值
#define VNETWORK_MAX_PORT_NUMBER 65535

// 缓冲区大小
#define VNETWORK_MAX_BUFFER_SIZE 1024

// 监听套接字连接请求的最大排队数量
#define VNETWORK_MAX_CONNECTIONS_BACKLOG 10

// 网络接口名称最大长度
#define VNETWORK_MAX_INTERFACE_NAME_LENGTH 256

// 网络接口描述最大长度
#define VNETWORK_MAX_INTERFACE_DESCRIPTION_LENGTH 1024

// HTTP 默认端口号
#define VNETWORK_DEFAULT_TCP_PORT 80

// UDP 默认端口号
#define VNETWORK_DEFAULT_UDP_PORT 53

// HTTP 默认端口号
#define VNETWORK_DEFAULT_HTTP_PORT 80

// HTTPS 默认端口号
#define VNETWORK_DEFAULT_HTTPS_PORT 443

// FTP 默认端口号
#define VNETWORK_DEFAULT_FTP_PORT 21

// SSH 默认端口号
#define VNETWORK_DEFAULT_SSH_PORT 22

// SMTP 默认端口号
#define VNETWORK_DEFAULT_SMTP_PORT 25

// POP3 默认端口号
#define VNETWORK_DEFAULT_POP3_PORT 110

// IMAP 默认端口号
#define VNETWORK_DEFAULT_IMAP_PORT 143

// SFTP 默认端口号
#define VNETWORK_DEFAULT_SFTP_PORT 115

// Telnet 默认端口号
#define VNETWORK_DEFAULT_TELNET_PORT 23

// NTP 默认端口号
#define VNETWORK_DEFAULT_NTP_PORT 123

// LDAP 默认端口号
#define VNETWORK_DEFAULT_LDAP_PORT 389

// LDAPS 默认端口号
#define VNETWORK_DEFAULT_LDAPS_PORT 636


enum VTCP_WORKER_STATUS {
  VTCP_WORKER_STATUS_NONE = 0x00,  // 无状态

  // libuv提供的连接状态
  VTCP_WORKER_STATUS_CONNECTING = 0x01,    // 连接中
  VTCP_WORKER_STATUS_CONNECTED = 0x02,     // 已连接
  VTCP_WORKER_STATUS_DISCONNECTED = 0x04,  // 已断开连接

  VTCP_WORKER_STATUS_LISTENING = 0x08,  // 监听中
  VTCP_WORKER_STATUS_CLOSED = 0x10,     // 已关闭

  // 读写状态
  VTCP_WORKER_STATUS_READING = 0x20,  // 可读取数据
  VTCP_WORKER_STATUS_WRITING = 0x40,  // 可写入数据

  // 数据处理状态
  VTCP_WORKER_STATUS_PROCESSING = 0x80,  // 正在处理数据

  // 断线重连状态
  VTCP_WORKER_STATUS_RECONNECTING = 0x100,  // 重新连接中

  VTCP_WORKER_STATUS_CLOSING = 0x200,  // 正在关闭中

  // libuv提供的错误状态
  VTCP_WORKER_STATUS_ERROR_NONE = 0x1000000,           // 无错误
  VTCP_WORKER_STATUS_ERROR_UNKNOWN = 0x2000000,     // 未知错误
  VTCP_WORKER_STATUS_ERROR_EADDRNOTAVAIL = 0x4000000,  // 地址不可用
  VTCP_WORKER_STATUS_ERROR_ECONNRESET = 0x8000000,     // 连接被重置

};
