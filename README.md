# Socket++

[![Build Status](https://www.travis-ci.org/shuai132/SocketPP.svg?branch=master)](https://www.travis-ci.org/shuai132/SocketPP)

a lightweight C++ TCP socket library, powered by epoll on Linux platform and kqueue on macOS/Unix.

## Features:
* High performance and high concurrenc benefit from epoll/kqueue
* Support send queue with thread safe
* Automatic memory management and ensure performance by c++ move semantics
* Multi-instance support
* Multiplatform support, Linux/macOS and most Unix-like OS.

## Requirements:
* C++11

## Build:
```bash
mkdir build && cd build && cmake .. && make
```

## Usage:
* simple echo server
```cpp
#include "SocketPP.h"

int main() {
    const int port = 6000;
    SocketPP::TCPServer server(port);
    server.setRecvHandle([&] (const Message &message) {
        server.send(message);
    });
    return server.loop();
}
```
