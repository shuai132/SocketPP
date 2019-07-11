//
// Created by liushuai on 2019/7/10.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include "type.h"

void ep_on_global_error();

void ep_on_error(int efd);

void ep_on_start(int efd, void *userdata);

void ep_on_read_error(int efd, int fd);

void ep_on_connected(int efd, int fd);

void ep_on_disconnected(int efd, int fd);

void ep_on_read_data(int efd, int fd, byte *buf, int cnt);

#ifdef __cplusplus
}
#endif
