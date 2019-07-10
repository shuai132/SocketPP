//
// Created by liushuai on 2019/7/10.
//

#include "epoll_event.h"
#include "ISocket.h"
#include "log.h"

static ISocket *_socket = nullptr;

void socket_set_delegate(ISocket *socket) {
    _socket = socket;
}

void socket_del_delegate() {
    _socket = nullptr;
}

void ep_on_error() {
    LOGD("ep_on_error");
}

void ep_on_start() {
    LOGD("ep_on_start");
    _socket->onStart();
}

void ep_on_read_error(int fd) {
    LOGD("ep_on_read_error:fd=%d", fd);
}

void ep_on_connected(int fd) {
    LOGD("ep_on_connected:fd=%d", fd);
    _socket->onConnected(fd);
}

void ep_on_disconnected(int fd) {
    LOGD("ep_on_disconnected:fd=%d", fd);
    _socket->onDisconnected(fd);
}

void ep_on_read_data(int fd, byte *buf, int cnt) {
    LOGD("ep_on_read_data:fd=%d, cnt=%d", fd, cnt);
    _socket->onReceive(fd, buf, cnt);
}
