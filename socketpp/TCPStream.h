//
// Created by liushuai on 2019/7/10.
//

#pragma once

#include <unistd.h>

#include "type.h"

namespace SocketPP {

class TCPStream {
public:
    int fd;

    // not explicit
    TCPStream(int fd = 0) : fd(fd) {}

    bool operator==(const TCPStream& right) const;

    ssize_t send(const byte* data, size_t length) const;
};

}
