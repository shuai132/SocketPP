//
// Created by liushuai on 19-7-11.
//

#pragma once

#include <cstddef>

#include "type.h"

class ISocket;
int socket_start_loop(ISocket *socket, int port);
void socket_on_destroy(ISocket *socket);

class ISocket {
public:
    virtual void onStart() = 0;
    virtual void onReceive(int fd, const byte *buf, size_t len) = 0;
    virtual void onConnected(int fd) = 0;
    virtual void onDisconnected(int fd) = 0;
    virtual void onClose() = 0;

public:
    explicit ISocket(int port)
        : _port(port) {
    }

    virtual ~ISocket() {
        socket_on_destroy(this);
    }

    int getPort() {
        return _port;
    }

    int loop() {
        return socket_start_loop(this, _port);
    }

private:
    int _port;
};
