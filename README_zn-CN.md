<img alt="Static Badge" src="https://img.shields.io/badge/VLibuv-master?style=flat&label=MIT-License&labelColor=rgb(23%2C33%2C73)&color=rgb(33%2C193%2C33)">  <img alt="Static Badge" src="https://img.shields.io/badge/Release-Latest?logo=gitlab&label=Latest&color=rgb(235%2C55%2C55)">

<a href="https://github.com/Antruly/VLibuv/blob/master/README_zn-CN.md" class="github-button" data-color-scheme="no-preference: dark; light: dark; dark: dark;" data-icon="octicon-star" data-size="large" data-show-count="true" aria-label="Star ntkme/github-buttons on GitHub">中文简体</a>
 
<a href="https://github.com/Antruly/VLibuv/blob/master/README.md" class="github-button" data-color-scheme="no-preference: dark; light: dark; dark: dark;" data-icon="octicon-star" data-size="large" data-show-count="true" aria-label="Star ntkme/github-buttons on GitHub">English</a>

# VLibuv

## 前言

现有C++对于事件驱动的底层库并不多，或者是说，很多都是高级封装，如Qt，Qt库过于庞大，且基本上有自己的一套事件循环机制，很多后台项目和高性能服务等项目使用起来并不是很方便，由此使用偏向底层的事件驱动库尤为重要。
libuv是一个很不错的事件驱动库，它支持跨平台，且是现有Node.js的底层支柱框架。由于libuv是C语言编写开发的，主要是面向过程实现的，从而导致C++项目使用起来并不是很方便，由此，VLibuv项目最初就是为了解决这一困扰问题而诞生的。
此项目开发发布至v1.0.0版本开始，也发现许多现有libuv的C++封装库，不过基本上都是只实现了一些常用基本功能，且丢失了原有的libuv的使用逻辑，从而需要重新学习其使用方法。而VLibuv的优势在于，完全延续libuv的使用方法，仅扩展C++面向对象的封装接口（实现逻辑感兴趣的可以查看源码）。
此项目作为开源项目，也希望可以解决更多的开发者现有面临的问题。项目的初步阶段，目前作者主流适配windows平台，可直接源码编译，Linux平台下，由于unix目录下源文件适配的平台较多，采用直接链接libuv静态库文件方式（详细构建方法后面有），至于其他不同平台的适配性，还需要大家一起不断完善，我相信此项目可以让libuv官方刮目相看！

## 概述

VLibuv 是一个基于 libuv 的 C++ 封装，旨在简化异步、事件驱动应用程序的开发。它提供了一个方便的面向对象编程接口，使得异步编程对于 C++ 开发者更加易用。

## 主要特性

- **C++ 封装：** 通过面向对象的接口简化异步编程和事件处理。
- **对 `uv_handle_t` 和 `uv_req_t` 的扩展：** 通过 C++ 继承简化管理和操作。
- **对 `uv_buf_t` 的扩展：** 通过扩展方法增强了缓冲区操作的灵活性。
- **兼容性：** 与所有版本的 libuv 1.0 系列保持兼容，及时集成新功能和改进。
- **使用风格：** 保持原有libuv的所有基本用法，仅扩展C++使用的便捷性，无需再为原生libuv的C语言的全局回调函数困扰，解决无法直接用在C++面向对象的尴尬情况，解放C++程序员面对libuv的棘手问题。
- **实用工具：** 扩展很多基本的工具类，如base64、字符串操作、线程池、内存池、自定义类型指针、字节流类型、日志类，等等，且工具功能细化丰富，完全满足一个正常C++客户端或是服务端的开发工作。
- **所有对象继承于VObject（类似C#面向对象思路）：** 对于 libuv 自己的 handle 类型和 req 类型，保持了对应子类型的伪继承关系，使用更高级的C++类型继承，类型转换也更加清晰便捷。

## 新特性

### VNetwork
VNetwork 是一个提供网络功能的新模块，包括处理 TCP 连接的 VTcpServer 和 VTcpClient 等类。
扩展了基本的网络操作功能，比如快速建立socket链接，发送消息等，也可以搭建一个socket服务，监听客户端连接情况，并分配单独的一个线程处理对应客户端消息，并运用了动态线程池，可预设参数，高效管理。
目前在普通i5级别cpu电脑下，iperf 压测1000线程满负荷运行，cpu占用不到10%，且各个客户端性能分配平均和稳定，没有出现高延时无响应情况（项目里有test-VTcpServer,编译之后可自行测试）

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

