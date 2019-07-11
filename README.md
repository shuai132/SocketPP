# Socket++

[![Build Status](https://www.travis-ci.org/shuai132/SocketPP.svg?branch=master)](https://www.travis-ci.org/shuai132/SocketPP)

a lightweight C++ TCP socket library powered by epoll on Linux platform.

## Features:
* High performance and high concurrenc benefit from epoll
* Support send queue with thread safe
* Automatic memory management
* Multi-instance support

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
    SocketPP socket(port);
    socket.setRecvHandle([&socket] (Message message) {
        socket.send(message);
    });
    return socket.loop();
}
```
