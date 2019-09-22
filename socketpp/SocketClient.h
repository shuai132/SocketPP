//
// Created by liushuai on 19-7-13.
//

#pragma once

#include <string>

#include "Socket.h"

namespace SocketPP {

class SocketClient : public Socket {
public:
    int loop() override;

public:
    explicit SocketClient(std::string ip, int port);

    inline void setIP(std::string ip) { ip_ = std::move(ip); };

    inline std::string getIP() const { return ip_; }

protected:
    std::string ip_;
};

}
