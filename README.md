<img alt="Static Badge" src="https://img.shields.io/badge/VLibuv-master?style=flat&label=MIT-License&labelColor=rgb(23%2C33%2C73)&color=rgb(33%2C193%2C33)">  <img alt="Static Badge" src="https://img.shields.io/badge/Release1.5.0-Latest?logo=gitlab&label=Latest&color=rgb(235%2C55%2C55)">



<a href="https://github.com/Antruly/VLibuv/blob/master/README_zn-CN.md" class="github-button" data-color-scheme="no-preference: dark; light: dark; dark: dark;" data-icon="octicon-star" data-size="large" data-show-count="true" aria-label="Star ntkme/github-buttons on GitHub">中文简体</a>
 
<a href="https://github.com/Antruly/VLibuv/blob/master/README.md" class="github-button" data-color-scheme="no-preference: dark; light: dark; dark: dark;" data-icon="octicon-star" data-size="large" data-show-count="true" aria-label="Star ntkme/github-buttons on GitHub">English</a>

# VLibuv
<img alt="Static Badge" src="https://github.com/Antruly/VLibuv/blob/master/VLibuv.webp" width="300" height="300">
## Overview

VLibuv is a C++ wrapper based on libuv, designed to simplify the development of asynchronous, event-driven applications. It provides a convenient object-oriented programming interface, making asynchronous programming more accessible for C++ developers.

## Key Features

- **C++ Wrapper:** Simplifies asynchronous programming and event handling with an object-oriented interface.
- **Extension of `uv_handle_t` and `uv_req_t`:** Facilitates management and operations through C++ inheritance.
- **Extension of `uv_buf_t`:** Enhances flexibility in buffer operations with extended methods.
- **Compatibility:** Maintains compatibility with all versions of libuv 1.0 series, incorporating new features and improvements promptly.

## New Features

### VNetwork
VNetwork is a new module that provides networking functionalities. It includes classes such as VTcpServer and VTcpClient for handling TCP connections.

### VWeb
VWeb is another new module focused on web-related functionalities. It includes classes for HTTP client and server implementations, such as VHttpClient, VHttpServer, VHttpRequest, VHttpResponse, and VHttpParser. Additionally, it provides support for WebSocket protocol through VWebSocketParser.


## Building and Installing

### Prerequisites

- [CMake](https://cmake.org/)
- [Git](https://git-scm.com/)
- [libuv](https://github.com/libuv/libuv)
- [zlib](https://github.com/madler/zlib)
- [http-parser](https://github.com/nodejs/http-parser)
- [websocket-parser](https://github.com/php-ion/websocket-parser)
- [OpenSSL](https://github.com/openssl/openssl)


### Build libuv（only linux）

#### Cmake:
```bash
    unbuntu: sudo apt-get install camke
    centos: sudo yum install camke
```
build:
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
build:
```bash
    ./autogen.sh
    ./configure --prefix=/usr/local/libuv CC=gcc CFLAGS=-static
    make -j4
    sudo make install
```

### Build OpenSSL

#### 1. Download and install perl

- Windows/Linux/macOs: [ActiveState Perl](https://www.activestate.com/products/perl/)
- Windows: [Strawberry Perl](http://strawberryperl.com/)

#### 2. Build

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

#### 1. Clone Prerequisites repository:

```bash
    git clone https://github.com/libuv/libuv libuv && \
    git clone https://github.com/madler/zlib zlib && \
    git clone https://github.com/nodejs/http-parser http-parser && \
    git clone https://github.com/php-ion/websocket-parser websocket-parser && \
    git clone https://github.com/openssl/openssl openssl
```

#### 2. Create a build directory in the root of VLibuv and navigate into it:

    ```bash
    mkdir build && cd build
    ```

#### 3. Run CMake to configure the project:
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
#### 4. Build the project using your preferred build tool (make, Visual Studio, etc.):

```bash
    cmake --build .
```

#### 5. Install the project:

```bash
    cmake --install .
```

## Example Usage

```cpp
#include "VTimer.h"

// Timer callback function
void timerFunc(VTimer* vtimer) {
    // Add logic to be executed when the timer triggers
    // For example, output a message
    std::cout << "Timer triggered!" << std::endl;
}

int main() {
    // Create an event loop object
    VLoop vloop;

    // Create a timer object and associate it with the event loop
    VTimer vtimer(&vloop);

    // Start the timer with the callback function as timerFunc, interval of 1000 milliseconds, and repeat count of 0 for unlimited repeats
    vtimer.start(timerFunc, 1000, 0);

    // Enter the event loop and wait for events to occur
    return vloop.run();
}
