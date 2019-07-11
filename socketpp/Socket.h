//
// Created by liushuai on 19-7-11.
//

#pragma once

#include <map>
#include <algorithm>

#include "type.h"
#include "log.h"

class Socket {
public:
    virtual void onStart(int efd);
    virtual void onClose();

    virtual void onConnected(int fd) = 0;
    virtual void onDisconnected(int fd) = 0;
    virtual void onReceive(int fd, const byte *buf, size_t len) = 0;

public:
    explicit Socket(int port);

    virtual ~Socket() ;

    int getPort() const;

    int loop();

public:
    /**
     * get mapped socket by epoll fd
     * exception: throw std::out_of_range exception if there is no socket for efd
     * @param efd
     * @return
     */
    static Socket* getSocket(int efd);

    static ssize_t write(int fd, const byte *data, size_t length);

private:
    int _port;

    static std::map<int, Socket*> fdMap;
};
