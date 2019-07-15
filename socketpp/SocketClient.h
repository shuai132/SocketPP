//
// Created by liushuai on 19-7-13.
//

#pragma once

#include <string>

#include "Socket.h"

class SocketClient : public Socket {
public:
    virtual int loop() override;

public:
    explicit SocketClient(std::string ip, int port);

    inline void setIP(std::string ip) { _ip = std::move(ip); };

    inline std::string getIP() const { return _ip; }

protected:
    std::string _ip;
};
