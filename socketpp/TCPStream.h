//
// Created by liushuai on 2019/7/10.
//

#pragma once

#include <unistd.h>

#include "type.h"

class TCPStream {
public:
    int fd = 0;

    // not explicit
    TCPStream(int fd) : fd(fd) {}

    bool operator==(const TCPStream &right) const;

    ssize_t send(const byte *data, size_t length);
};
