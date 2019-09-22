//
// Created by liushuai on 19-7-13.
//

#include "SocketClient.h"
#include "socket_platform.h"

namespace SocketPP {

SocketClient::SocketClient(std::string ip, int port)
        : Socket(port), ip_(std::move(ip)) {
}

int SocketClient::loop() {
    LOGD("socket=%p, port=%d", this, port_);
    return sk_connect_and_loop(ip_.c_str(), port_, this);
}

}
