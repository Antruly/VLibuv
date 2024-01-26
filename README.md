# VLibuv

## Project Overview

VLibuv is a C++ wrapper based on libuv, designed to provide a convenient object-oriented programming interface and simplify the development of asynchronous, event-driven applications. By extending and encapsulating libuv's `uv_handle_t` and `uv_req_t` types, VLibuv makes them more compatible with C++'s object-oriented programming style.

## Key Features

- **C++ Wrapper:** Simplifies asynchronous programming and event handling with a C++ wrapper for libuv's raw types.

- **Extension of `uv_handle_t` and `uv_req_t`:** Facilitates management and operations through C++ inheritance.

- **Extension of `uv_buf_t`:** Enhances flexibility in buffer operations with extended methods.

- **Derived Types:** Introduces derived types, such as `VTcpService`, for rapid establishment of a TCP service.

- **Compatibility:** Maintains compatibility with all versions of libuv 1.0 series, incorporating new features and improvements promptly.

## Building and Installing the Project with CMake
Follow these steps to build and install the project using CMake:

```camke
# Clone the libuv repository from https://github.com/libuv/libuv.
git clone https://github.com/libuv/libuv

# In the root directory of VLibuv, create a build directory
mkdir build && cd build

# Run CMake to configure the project
cmake ..

# Build the project using your preferred build tool (make, Visual Studio, etc.)
cmake --build .

# Install the project
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
    return vloop.exec();
}
```
