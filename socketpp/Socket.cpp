//
// Created by liushuai on 19-7-11.
//

#include <algorithm>

#include "Socket.h"
#include "socket_platform.h"

namespace SocketPP {

std::map<int, Socket*> Socket::efdSocketMap_;

Socket::Socket(int port)
        : port_(port) {
}

Socket::~Socket() {
    onClose();
}

void Socket::onStart(int efd) {
    efdSocketMap_[efd] = this;
}

void Socket::onClose() {
    if (efdSocketMap_.empty()) return;

    const auto& iter = std::find_if(efdSocketMap_.cbegin(), efdSocketMap_.cend(),
                                    [&](const std::pair<int, Socket*>& pair) {
                                        return pair.second == this;
                                    });
    if (iter != efdSocketMap_.cend()) {
        efdSocketMap_.erase(iter);
    } else {
        LOGE("no efd in socket:%p", this);
    }
}

Socket* Socket::getSocket(int efd) {
    return efdSocketMap_.at(efd);
}

ssize_t Socket::write(int fd, const byte* data, size_t length) {
    return sk_write_fd(fd, data, length);
}

}
