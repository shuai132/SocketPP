//
// Created by liushuai on 2019/7/10.
//

#include "TCPStream.h"

bool TCPStream::operator==(const TCPStream &right) const {
    return this->fd == right.fd;
}

ssize_t TCPStream::send(const byte *data, size_t length) {
    return ep_write_data(fd, data, length);
}
