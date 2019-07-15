//
// Created by liushuai on 19-7-11.
//

#pragma once

#include <map>

#include "type.h"
#include "log.h"

class Socket {
public:
    virtual void onConnected(int fd) = 0;

    virtual void onDisconnected(int fd) = 0;

    virtual void onReceive(int fd, const byte *buf, size_t len) = 0;

    virtual int loop() = 0;

public:
    inline void setPort(int port) { _port = port; };

    inline int getPort() const { return _port; }

public:
    virtual void onStart(int efd);

    virtual void onClose();

protected:
    explicit Socket(int port = 6000);

    virtual ~Socket();

protected:
    int _port;

public:
    /**
     * get mapped socket by efd
     * exception: throw std::out_of_range exception if there is no socket for efd
     * @param efd
     * @return
     */
    static Socket* getSocket(int efd);

    static ssize_t write(int fd, const byte *data, size_t length);

private:
    static std::map<int, Socket*> _efdSocketMap;
};
