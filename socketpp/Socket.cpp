//
// Created by liushuai on 19-7-11.
//

#include "Socket.h"
#include "socket_platform.h"

std::map<int, Socket*> Socket::fdMap;

Socket::Socket(int port)
        : _port(port) {
}

Socket::~Socket() {
    LOGD("socket=%p", this);
    auto iter = std::find_if(fdMap.cbegin(), fdMap.cend(), [&](std::pair<int, Socket*> epollSocket) {
        return epollSocket.second == this;
    });
    if (iter != fdMap.cend()) {
        fdMap.erase(iter);
    }
}

void Socket::onStart(int efd) {
    fdMap[efd] = this;
}

void Socket::onClose() {
}

int Socket::getPort() const {
    return _port;
}

int Socket::loop() {
    LOGD("socket=%p, port=%d", this, _port);
    return sk_start_loop(_port, this);
}

Socket *Socket::getSocket(int efd) {
    return fdMap.at(efd);
}

ssize_t Socket::write(int fd, const byte *data, size_t length) {
    return sk_write_fd(fd, data, length);
}
