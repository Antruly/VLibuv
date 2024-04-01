# VLibuv

## Overview

VLibuv is a C++ wrapper based on libuv, designed to simplify the development of asynchronous, event-driven applications. It provides a convenient object-oriented programming interface, making asynchronous programming more accessible for C++ developers.

## Key Features

- **C++ Wrapper:** Simplifies asynchronous programming and event handling with an object-oriented interface.
- **Extension of `uv_handle_t` and `uv_req_t`:** Facilitates management and operations through C++ inheritance.
- **Extension of `uv_buf_t`:** Enhances flexibility in buffer operations with extended methods.
- **Derived Types:** Introduces derived types, such as `VTcpServer`, for rapid establishment of a TCP service.
- **Compatibility:** Maintains compatibility with all versions of libuv 1.0 series, incorporating new features and improvements promptly.

## Building and Installing

### Prerequisites

- CMake
- Git
- [libuv](https://github.com/libuv/libuv)
- [zlib](https://github.com/madler/zlib)
- [http-parser](https://github.com/nodejs/http-parser)
- [websocket-parser](https://github.com/php-ion/websocket-parser)
- [OpenSSL](https://github.com/openssl/openssl)


### Build libuv

cmake:
    ```bash
    sudo apt-get install camke
    sudo yum install camke
    ```

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    sudo cmake --install .
    ```

autogen:
    ```bash
    sudo apt-get install autoconf automake libtool
    sudo yum install autoconf automake libtool
    ```

    ```bash
    ./autogen.sh
    ./configure --prefix=/usr/local/libuv CC=gcc CFLAGS=-static
    make -j4
    sudo make install
    ```

### Build OpenSSL

1. Download and install perl

- Windows/Linux/macOs [ActiveState Perl](https://www.activestate.com/products/perl/)
- Windows [Strawberry Perl](http://strawberryperl.com/)

2. Build

Linux:
    ```bash
    ./Configure linux-x86_64 no-shared --prefix=/usr/local/openssl
    make depend
    make -j4
    sudo make install
    ```

Windows:
    ```bash
    perl Configure VC-WIN64A no-shared --prefix=C:\openssl
    set CL=/MP
    nmake
    nmake install
    ```

### Instructions

1. Clone Prerequisites repository:

    ```bash
    git clone https://github.com/libuv/libuv libuv && \
    git clone https://github.com/madler/zlib zlib && \
    git clone https://github.com/nodejs/http-parser http-parser && \
    git clone https://github.com/php-ion/websocket-parser websocket-parser && \
    git clone https://github.com/openssl/openssl openssl
    ```

2. Create a build directory in the root of VLibuv and navigate into it:

    ```bash
    mkdir build && cd build
    ```

3. Run CMake to configure the project:

    ```bash
    cmake -DLIBUV_DIR=/usr/local/libuv \
      -DOPENSSL_DIR=/usr/local/openssl\
      -DHTTP_PARSER_DIR=../http-parser \
      -DWEBSOCKET_PARSER_DIR=../websocket-parser \
      -DZLIB_DIR=../zlib \
      ..
    ```

4. Build the project using your preferred build tool (make, Visual Studio, etc.):

    ```bash
    cmake --build .
    ```

5. Install the project:

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
