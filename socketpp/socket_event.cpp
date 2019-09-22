//
// Created by liushuai on 19-7-11.
//

#include "socket_event.h"
#include "Socket.h"
#include "log.h"

using namespace SocketPP;

void sk_on_global_error() {
    LOGE("sk_on_global_error");
}

void sk_on_error(int efd) {
    LOGE("sk_on_error:efd=%d", efd);
}

void sk_on_start(int efd, void* userdata) {
    LOGD("sk_on_start:efd=%d", efd);
    auto socket = static_cast<Socket*>(userdata);
    socket->onStart(efd);
}

void sk_on_close(int efd) {
    LOGD("sk_on_close:efd=%d", efd);
    Socket::getSocket(efd)->onClose();
}

/**
 * after connected
 * @param efd
 * @param fd
 */
void sk_on_connected(int efd, int fd) {
    LOGD("sk_on_connected:efd=%d, fd=%d", efd, fd);
    Socket::getSocket(efd)->onConnected(fd);
}

/**
 * after fd closed by os
 * @param efd
 * @param fd
 */
void sk_on_disconnected(int efd, int fd) {
    LOGD("sk_on_disconnected:efd=%d, fd=%d", efd, fd);
    Socket::getSocket(efd)->onDisconnected(fd);
}

void sk_on_read_data(int efd, int fd, byte* buf, int len) {
    LOGD("sk_on_read_data:efd=%d, fd=%d, len=%d", efd, fd, len);
    Socket::getSocket(efd)->onReceive(fd, buf, len);
}

void sk_on_read_error(int efd, int fd) {
    LOGE("sk_on_read_error:efd=%d, fd=%d", efd, fd);
}
