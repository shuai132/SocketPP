//
// Created by liushuai on 19-7-13.
//

#include "SocketClient.h"
#include "socket_platform.h"

SocketClient::SocketClient(std::string ip, int port)
        : Socket(port), _ip(std::move(ip)) {
}

int SocketClient::loop() {
    LOGD("socket=%p, port=%d", this, _port);
    return sk_connect_and_loop(_ip.c_str(), _port, this);
}
