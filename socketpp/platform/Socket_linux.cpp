//
// Created by liushuai on 19-7-11.
//

#include <string>
#include <unistd.h>

#include "socket_platform.h"
#include "epoll.h"

int sk_start_loop(int port, void *userdata) {
    return ep_start_loop(std::to_string(port).c_str(), userdata);
}

ssize_t sk_write_fd(int fd, const byte *data, size_t length) {
    return ::write(fd, data, length);
}
