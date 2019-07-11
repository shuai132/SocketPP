//
// Created by liushuai on 19-7-11.
//

#include "socket_platform.h"
#include "kqueue.h"

int sk_start_loop(int port, void *userdata) {
    return kq_start_loop(port, userdata);
}

ssize_t sk_write_fd(int fd, const byte *data, size_t length) {
    return ::write(fd, data, length);
}
