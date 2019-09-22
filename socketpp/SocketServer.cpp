//
// Created by liushuai on 19-7-13.
//

#include "SocketServer.h"
#include "socket_platform.h"

namespace SocketPP {

SocketServer::SocketServer(int port)
        : Socket(port) {
}

int SocketServer::loop() {
    LOGD("socket=%p, port=%d", this, port_);
    return sk_server_start_loop(port_, this);
}

}
