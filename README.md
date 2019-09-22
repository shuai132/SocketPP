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

using namespace SocketPP;

int main() {
    const int port = 6000;
    TCPServer server(port);
    server.setRecvHandle([&] (const Message& message) {
        server.send(message);
    });
    return server.loop();
}
```
and then use nc tools, or run client example:
```bash
nc localhost 6000
```
or
```cpp
#include "SocketPP.h"

using namespace SocketPP;

int main() {
    const int port = 6000;
    TCPClient client("127.0.0.1", port);
    client.setConnHandle([&] (const TCPStream& stream) {
        client.send("hello");
    });
    client.setRecvHandle([&] (const Message& message) {
        printf("on receive: msg:%s", message.rawMsg.toString().c_str());
    });
    return client.loop();
}
```
