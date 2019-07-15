//
// Created by liushuai on 19-7-13.
//

#pragma once

#include "Socket.h"
#include "type.h"
#include "log.h"

class SocketServer : public Socket {
public:
    virtual int loop() override;

public:
    explicit SocketServer(int port);
};
