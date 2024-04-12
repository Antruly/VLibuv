<button onclick="window.location.href = 'README_zn-CN.md';">中文</button>
<button onclick="window.location.href = 'README.md';">English</button>

# VLibuv

## 概述

VLibuv 是一个基于 libuv 的 C++ 封装，旨在简化异步、事件驱动应用程序的开发。它提供了一个方便的面向对象编程接口，使得异步编程对于 C++ 开发者更加易用。

## 主要特性

- **C++ 封装：** 通过面向对象的接口简化异步编程和事件处理。
- **对 `uv_handle_t` 和 `uv_req_t` 的扩展：** 通过 C++ 继承简化管理和操作。
- **对 `uv_buf_t` 的扩展：** 通过扩展方法增强了缓冲区操作的灵活性。
- **兼容性：** 与所有版本的 libuv 1.0 系列保持兼容，及时集成新功能和改进。

## 新特性

### VNetwork
VNetwork 是一个提供网络功能的新模块，包括处理 TCP 连接的 VTcpServer 和 VTcpClient 等类。

### VWeb
VWeb 是另一个专注于 Web 相关功能的新模块，包括 HTTP 客户端和服务器实现的类，如 VHttpClient、VHttpServer、VHttpRequest、VHttpResponse 和 VHttpParser。此外，它通过 VWebSocketParser 还提供了对 WebSocket 协议的支持。

## 构建和安装

### 先决条件

- [CMake](https://cmake.org/)
- [Git](https://git-scm.com/)
- [libuv](https://github.com/libuv/libuv)
- [zlib](https://github.com/madler/zlib)
- [http-parser](https://github.com/nodejs/http-parser)
- [websocket-parser](https://github.com/php-ion/websocket-parser)
- [OpenSSL](https://github.com/openssl/openssl)

### 构建 libuv（仅限 Linux）

#### Cmake:
```bash
    unbuntu: sudo apt-get install camke
    centos: sudo yum install camke
```
构建:
```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    sudo cmake --install .
```

#### Autogen:
```bash
    unbuntu: sudo apt-get install autoconf automake libtool
    centos: sudo yum install autoconf automake libtool
```
构建:
```bash
    ./autogen.sh
    ./configure --prefix=/usr/local/libuv CC=gcc CFLAGS=-static
    make -j4
    sudo make install
```

### 构建 OpenSSL

#### 1. 下载并安装 perl

- Windows/Linux/macOs: [ActiveState Perl](https://www.activestate.com/products/perl/)
- Windows: [Strawberry Perl](http://strawberryperl.com/)

#### 2. 构建

Linux:
```bash
    ./Configure linux-x86_64 no-shared --prefix=/usr/local/openssl
    make depend
    make -j4
    sudo make install
```

Windows:
```bash
    perl Configure VC-WIN64A no-shared --prefix=c:\openssl
    set CL=/MP
    nmake
    nmake install
```

### Instructions

#### 1. 克隆先决条件存储库:

```bash
    git clone https://github.com/libuv/libuv libuv && \
    git clone https://github.com/madler/zlib zlib && \
    git clone https://github.com/nodejs/http-parser http-parser && \
    git clone https://github.com/php-ion/websocket-parser websocket-parser && \
    git clone https://github.com/openssl/openssl openssl
```

#### 2. 在 VLibuv 的根目录中创建一个构建目录并进入其中:

```bash
    mkdir build && cd build
```

#### 3. 运行 CMake 配置项目:
linux:
```bash
    cmake -DLIBUV_DIR=/usr/local/libuv \
      -DOPENSSL_DIR=/usr/local/openssl\
      -DHTTP_PARSER_DIR=../http-parser \
      -DWEBSOCKET_PARSER_DIR=../websocket-parser \
      -DZLIB_DIR=../zlib \
      -DVLIBUV_BUILD_TESTS=OFF \
      ..
```
windows:
```bash
    cmake -DLIBUV_DIR=../libuv \
      -DOPENSSL_DIR=c:/openssl \
      -DHTTP_PARSER_DIR=../http-parser \
      -DWEBSOCKET_PARSER_DIR=../websocket-parser \
      -DZLIB_DIR=../zlib \
      -DVLIBUV_BUILD_TESTS=OFF \
      ..
```
#### 4. 使用您喜欢的构建工具（make、Visual Studio 等）构建项目:

```bash
    cmake --build .
```

#### 5. 安装项目:

```bash
    cmake --install .
```

## 示例用法

```cpp
#include "VTimer.h"

// 定时器回调函数
void timerFunc(VTimer* vtimer) {
    // 添加定时器触发时要执行的逻辑
    // 例如，输出一条消息
    std::cout << "定时器触发！" << std::endl;
}

int main() {
    // 创建一个事件循环对象
    VLoop vloop;

    // 创建一个定时器对象并将其与事件循环关联
    VTimer vtimer(&vloop);

    // 以 timerFunc 作为回调函数，间隔为 1000 毫秒，重复次数为 0 表示无限重复，启动定时器
    vtimer.start(timerFunc, 1000, 0);

    // 进入事件循环并等待事件发生
    return vloop.run();
}

