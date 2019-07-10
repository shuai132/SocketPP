//
// Created by liushuai on 19-7-11.
//

#pragma once

#include "type.h"

class ISocket;
void socket_set_delegate(ISocket *socket);
void socket_del_delegate();

class ISocket {
public:
    virtual void onStart() = 0;
    virtual void onReceive(int fd, const byte *buf, size_t len) = 0;
    virtual void onConnected(int fd) = 0;
    virtual void onDisconnected(int fd) = 0;
    virtual void onClose() = 0;

    ISocket() {
        socket_set_delegate(this);
    }
    virtual ~ISocket() {
        socket_del_delegate();
    }
};
