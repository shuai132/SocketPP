//
// Created by liushuai on 19-7-11.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <cstddef>
#include <unistd.h>

#include "type.h"

int sk_server_start_loop(int port, void* userdata);

int sk_connect_and_loop(const char* host, int port, void* userdata);

ssize_t sk_write_fd(int fd, const byte* data, size_t length);

#ifdef __cplusplus
}
#endif
