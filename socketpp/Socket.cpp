//
// Created by liushuai on 19-7-11.
//

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
    auto iter = std::find_if(_efdSocketMap.cbegin(), _efdSocketMap.cend(), [&](std::pair<int, Socket*> epollSocket) {
        return epollSocket.second == this;
    });
    if (iter != _efdSocketMap.cend()) {
        _efdSocketMap.erase(iter);
    } else {
        LOGE("no efd in socket:%p", this);
    }
}

void Socket::setPort(int port) {
    _port = port;
}

int Socket::getPort() const {
    return _port;
}

int Socket::loop() {
    LOGD("socket=%p, port=%d", this, _port);
    return sk_start_loop(_port, this);
}

Socket *Socket::getSocket(int efd) {
    return _efdSocketMap.at(efd);
}

ssize_t Socket::write(int fd, const byte *data, size_t length) {
    return sk_write_fd(fd, data, length);
}
