//
// Created by liushuai on 2019/7/10.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include "type.h"

void ep_on_error();

void ep_on_start();

void ep_on_read_error(int fd);

void ep_on_connected(int fd);

void ep_on_disconnected(int fd);

void ep_on_read_data(int fd, byte *buf, int cnt);

#ifdef __cplusplus
}
#endif
