//
// Created by liushuai on 2019/7/13.
//

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "epoll.h"
#include "socket_event.h"
#include "log.h"


#define BUF_SIZE 512

#define RET_OK  0
#define RET_ERR -1

void set_non_block(int fd)
{
    int flag = fcntl ( fd, F_GETFL, 0 );
    fcntl ( fd, F_SETFL, flag | O_NONBLOCK );
}

int ep_connect_and_loop(const char *host, int port, void *userdata)
{
    int ret = RET_OK;

    in_port_t i16_port = port;
    if(0 >= i16_port)
    {
        LOGE("port number is wrong:%d", i16_port);
        sk_on_global_error();
        return RET_ERR;
    }

    int efd;
    efd = epoll_create(10);
    if(efd == -1)
    {
        LOGE("epoll_create error!");
        sk_on_global_error();
        return RET_ERR;
    }

    sk_on_start(efd, userdata);

    int sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == sk)
    {
        LOGE("open socket failed!");
        sk_on_global_error();
        return RET_ERR;
    }

    struct sockaddr_in sa = {0};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(i16_port);

    struct sockaddr_in *psa = &sa;

    ret = inet_pton(AF_INET, host, &psa->sin_addr.s_addr);
    if(0 == ret)
    {
        LOGE("inet_pton failed, invalid address!");
        close(sk);
        return RET_ERR;
    }
    else if(ret < 0)
    {
        LOGE("inet_pton failed");
        close(sk);
        sk_on_global_error();
        return RET_ERR;
    }

    if(connect(sk, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
        LOGE("connect failed");
        close(sk);
        sk_on_global_error();
        return RET_ERR;
    } else {
        sk_on_connected(efd, sk);
    }

    set_non_block(sk);

    struct epoll_event event;
    struct epoll_event events[10];

    event.events = EPOLLOUT | EPOLLIN | EPOLLET;
    event.data.fd = sk;

    epoll_ctl(efd, EPOLL_CTL_ADD, sk, &event);

    while(1)
    {
        int cnt = epoll_wait(efd, events, 10, -1);
        for (int i = 0; i < cnt; i++)
        {
            if(events[i].events & EPOLLIN)
            {
                char buf[BUF_SIZE];
                int fd = events[i].data.fd;
                int len = read(fd, buf, BUF_SIZE);
                if (len == -1) {
                    if (errno == EAGAIN) {
                        break;
                    }

                    sk_on_read_error(efd, fd);

                } else if (len == 0) {
                    close(events[i].data.fd);
                    sk_on_disconnected(efd, fd);
                    break;
                }

                sk_on_read_data(efd, fd, (byte*)buf, len);
            }
        }
    }

    close(sk);
    close(efd);

    return RET_OK;
}
