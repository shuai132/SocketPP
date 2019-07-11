//
// Created by liushuai on 2019/7/10.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "epoll.h"
#include "epoll_event.h"
#include "log.h"


#define MAX_EVENTS  64
static int BUF_SIZE;

static void init_settings(void) {
    BUF_SIZE = getpagesize();
}

static int create_and_bind(const char *port) {
    struct addrinfo hint, *result;
    int res, sfd;

    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags    = AI_PASSIVE;

    res = getaddrinfo(NULL, port, &hint, &result);
    if (res == -1) {
        LOGE("can not get socket descriptor!");
        ep_on_global_error();
        return -1;
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        LOGE("can not get socket descriptor!");
        ep_on_global_error();
        return -1;
    }

    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        LOGE("bind error!");
        ep_on_global_error();
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}


static int make_socket_non_binding(int sfd)
{
    int flags, res;

    flags = fcntl(sfd, F_GETFL);
    if (flags == -1) {
        LOGE("cannot get socket flags!");
        ep_on_global_error();;
        return -1;
    }

    flags |= O_NONBLOCK;
    res = fcntl(sfd, F_SETFL, flags);
    if (res == -1) {
        LOGE("cannot set socket flags!");
        ep_on_global_error();;
        return -1;
    }

    return 0;
}

int ep_start_loop(const char *port, void *userdata)
{
    init_settings();

    int sfd, res, epoll, cnt, i, sd;
    struct epoll_event event, events[MAX_EVENTS];

    sfd = create_and_bind(port);
    if (sfd == -1) {
        LOGE("cannot create socket!");
        ep_on_global_error();;
        return -1;
    }

    res = make_socket_non_binding(sfd);
    if (res == -1) {
        LOGE("connot set flags!");
        ep_on_global_error();;
        return -1;
    }

    res = listen(sfd, SOMAXCONN);
    if (res == -1) {
        LOGE("cannot listen!");
        ep_on_global_error();;
        return -1;
    }

    epoll = epoll_create(1);
    if (epoll == -1) {
        LOGE("cannot create epoll!");
        ep_on_global_error();;
        return -1;
    }

    event.events  = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = sfd;
    res = epoll_ctl(epoll, EPOLL_CTL_ADD, sfd, &event);
    if (res == -1) {
        LOGE("can not add event to epoll!");
        ep_on_global_error();;
        return -1;
    }

    while (1) {
        ep_on_start(epoll, userdata);
        cnt = epoll_wait(epoll, events, MAX_EVENTS, -1);
        for (i = 0; i < cnt; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN)) {
                LOGE("socket fd error!");
                close(events[i].data.fd);
                continue;

            } else if (events[i].data.fd == sfd) {
                while (1) {
                    struct sockaddr client_addr;
                    socklen_t addrlen = sizeof(struct sockaddr);

                    sd = accept(sfd, &client_addr, &addrlen);
                    if (sd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            LOGE("cannot accept new socket!");
                            continue;
                        }
                    }

                    res = make_socket_non_binding(sd);
                    if (res == -1) {
                        LOGE("cannot set flags!");
                        ep_on_error(epoll);
                        return -1;
                    }

                    event.data.fd = sd;
                    event.events  = EPOLLET | EPOLLIN;
                    res = epoll_ctl(epoll, EPOLL_CTL_ADD, sd, &event);
                    if (res == -1) {
                        LOGE("cannot add to epoll!");
                        ep_on_error(epoll);
                        return -1;
                    }

                    ep_on_connected(epoll, sd);
                }
            } else {
                ssize_t cnt;
                byte buf[BUF_SIZE];

                while (1) {
                    int fd = events[i].data.fd;
                    cnt = read(fd, buf, BUF_SIZE);
                    if (cnt == -1) {
                        if (errno == EAGAIN) {
                            break;
                        }

                        ep_on_read_error(epoll, fd);

                    } else if (cnt == 0) {
                        close(events[i].data.fd);
                        ep_on_disconnected(epoll, fd);
                        break;
                    }

                    ep_on_read_data(epoll, fd, buf, cnt);
                }
            }
        }
    }

    close(epoll);
    return 0;
}

void ep_end_loop(void) {
    // todo: close epoll
}

ssize_t ep_write_data(int fd, const byte *buf, size_t len) {
    return write(fd, buf, len);
}

void ep_close_fd(int fd) {
    close(fd);
}
