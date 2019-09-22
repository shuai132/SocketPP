//
// Created by liushuai on 19-7-11.
//

#include <string>

#include "socket_platform.h"
#include "epoll/epoll.h"

int sk_server_start_loop(int port, void* userdata) {
    return ep_server_start_loop(std::to_string(port).c_str(), userdata);
}

int sk_connect_and_loop(const char* host, int port, void* userdata) {
    return ep_connect_and_loop(host, port, userdata);
}

ssize_t sk_write_fd(int fd, const byte* data, size_t length) {
    return ::write(fd, data, length);
}
