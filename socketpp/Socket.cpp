//
// Created by liushuai on 19-7-11.
//

#include <algorithm>

#include "Socket.h"
#include "socket_platform.h"

std::map<int, Socket*> Socket::_efdSocketMap;

Socket::Socket(int port)
        : _port(port) {
}

Socket::~Socket() {
    onClose();
}

void Socket::onStart(int efd) {
    _efdSocketMap[efd] = this;
}

void Socket::onClose() {
    if (_efdSocketMap.empty()) return;

    const auto &iter = std::find_if(_efdSocketMap.cbegin(), _efdSocketMap.cend(), [&](const std::pair<int, Socket*> &pair) {
        return pair.second == this;
    });
    if (iter != _efdSocketMap.cend()) {
        _efdSocketMap.erase(iter);
    } else {
        LOGE("no efd in socket:%p", this);
    }
}

Socket *Socket::getSocket(int efd) {
    return _efdSocketMap.at(efd);
}

ssize_t Socket::write(int fd, const byte *data, size_t length) {
    return sk_write_fd(fd, data, length);
}
