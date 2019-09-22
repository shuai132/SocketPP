//
// Created by liushuai on 2019/7/10.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

void sk_on_global_error();

void sk_on_error(int efd);

void sk_on_start(int efd, void* userdata);

void sk_on_close(int efd);

void sk_on_connected(int efd, int fd);

void sk_on_disconnected(int efd, int fd);

void sk_on_read_data(int efd, int fd, byte* buf, int len);

void sk_on_read_error(int efd, int fd);

#ifdef __cplusplus
}
#endif
