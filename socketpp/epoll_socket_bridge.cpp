//
// Created by liushuai on 2019/7/10.
//

#include <string>
#include <map>
#include <algorithm>

#include "ISocket.h"
#include "epoll.h"
#include "epoll_event.h"
#include "log.h"

static std::map<int, ISocket*> epollSocketMap;

/**
 * get mapped socket by epoll fd
 * exception: throw std::out_of_range exception if there is no socket for efd
 * @param efd
 * @return
 */
static inline ISocket* get_socket(int efd) {
    return epollSocketMap.at(efd);
}

int socket_start_loop(ISocket *socket, int port) {
    LOGD("socket_start_loop:socket=%p, port=%d", socket, port);
    return ep_start_loop(std::to_string(port).c_str(), socket);
}

void socket_on_destroy(ISocket *socket) {
    LOGD("socket_on_destroy:socket=%p", socket);
    auto iter = std::find_if(epollSocketMap.cbegin(), epollSocketMap.cend(), [&](std::pair<int, ISocket*> epollSocket) {
        return epollSocket.second == socket;
    });
    if (iter != epollSocketMap.cend()) {
        epollSocketMap.erase(iter);
    }
}

void ep_on_global_error() {
    LOGE("ep_on_global_error");
}

void ep_on_error(int efd) {
    LOGE("ep_on_error:efd=%d", efd);
}

void ep_on_start(int efd, void *userdata) {
    auto socket = static_cast<ISocket *>(userdata);
    epollSocketMap[efd] = socket;
    socket->onStart();
}

void ep_on_read_error(int efd, int fd) {
    LOGE("ep_on_read_error:efd=%d, fd=%d", efd, fd);
}

void ep_on_connected(int efd, int fd) {
    LOGD("ep_on_connected:efd=%d, fd=%d", efd, fd);
    get_socket(efd)->onConnected(fd);
}

void ep_on_disconnected(int efd, int fd) {
    LOGD("ep_on_disconnected:efd=%d, fd=%d", efd, fd);
    get_socket(efd)->onDisconnected(fd);
}

void ep_on_read_data(int efd, int fd, byte *buf, int cnt) {
    LOGD("ep_on_read_data:efd=%d, fd=%d, cnt=%d", efd, fd, cnt);
    get_socket(efd)->onReceive(fd, buf, cnt);
}
