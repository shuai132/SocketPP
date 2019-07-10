//
// Created by liushuai on 2019/7/10.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include "type.h"

/******** 控制 ********/
/**
 * 开始事件循环
 * @param port
 * @return
 */
int ep_start_loop(const char *port);

void ep_end_loop(void);

/**
 * Write N bytes of BUF to FD.  Return the number written, or -1
 * @param fd    要发送的文件描述符
 * @param buf   发送缓冲区
 * @param len   缓冲区大小
 * @return      >=0 成功发送的长度
 *              -1  出现错误
 */
ssize_t ep_write_data(int fd, const byte *buf, size_t len);

void ep_close_fd(int fd);

#ifdef __cplusplus
}
#endif
