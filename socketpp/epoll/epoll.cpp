//
// Created by liushuai on 2019/7/10.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include "epoll.h"
#include "socket_event.h"
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
        sk_on_global_error();
        return -1;
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        LOGE("can not get socket descriptor!");
        sk_on_global_error();
        return -1;
    }

    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        LOGE("bind error!");
        sk_on_global_error();
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}


static int make_socket_non_block(int sfd)
{
    int flags, res;

    flags = fcntl(sfd, F_GETFL);
    if (flags == -1) {
        LOGE("cannot get socket flags!");
        sk_on_global_error();
        return -1;
    }

    flags |= O_NONBLOCK;
    res = fcntl(sfd, F_SETFL, flags);
    if (res == -1) {
        LOGE("cannot set socket flags!");
        sk_on_global_error();
        return -1;
    }

    return 0;
}

static int loop_once(int epoll, int sfd, struct epoll_event &event)
{
    struct epoll_event events[MAX_EVENTS];
    int cnt = epoll_wait(epoll, events, MAX_EVENTS, -1);

    for (int i = 0; i < cnt; i++) {
        if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN)) {
            LOGE("socket fd error!");
            close(events[i].data.fd);
            continue;

        } else if (events[i].data.fd == sfd) {
            while (1) {
                struct sockaddr client_addr;
                socklen_t addrlen = sizeof(struct sockaddr);

                int sd = accept(sfd, &client_addr, &addrlen);
                if (sd == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    } else {
                        LOGE("cannot accept new socket!");
                        continue;
                    }
                }

                int res = make_socket_non_block(sd);
                if (res == -1) {
                    LOGE("cannot set flags!");
                    sk_on_error(epoll);
                    return -1;
                }

                event.data.fd = sd;
                event.events  = EPOLLET | EPOLLIN;
                res = epoll_ctl(epoll, EPOLL_CTL_ADD, sd, &event);
                if (res == -1) {
                    LOGE("cannot add to epoll!");
                    sk_on_error(epoll);
                    return -1;
                }

                sk_on_connected(epoll, sd);
            }
        } else {
            ssize_t len;
            byte buf[BUF_SIZE];

            while (1) {
                int fd = events[i].data.fd;
                len = read(fd, buf, BUF_SIZE);
                if (len == -1) {
                    if (errno == EAGAIN) {
                        break;
                    }

                    sk_on_read_error(epoll, fd);

                } else if (len == 0) {
                    close(events[i].data.fd);
                    sk_on_disconnected(epoll, fd);
                    break;
                }

                sk_on_read_data(epoll, fd, buf, len);
            }
        }
    }
    return 0;
}

int ep_server_start_loop(const char *port, void *userdata)
{
    init_settings();

    int sfd, res, epoll;
    struct epoll_event event;

    sfd = create_and_bind(port);
    if (sfd == -1) {
        LOGE("cannot create socket!");
        sk_on_global_error();
        return -1;
    }

    res = make_socket_non_block(sfd);
    if (res == -1) {
        LOGE("connot set flags!");
        sk_on_global_error();
        return -1;
    }

    res = listen(sfd, SOMAXCONN);
    if (res == -1) {
        LOGE("cannot listen!");
        sk_on_global_error();
        return -1;
    }

    epoll = epoll_create(1);
    if (epoll == -1) {
        LOGE("cannot create epoll!");
        sk_on_global_error();
        return -1;
    }

    event.events  = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = sfd;
    res = epoll_ctl(epoll, EPOLL_CTL_ADD, sfd, &event);
    if (res == -1) {
        LOGE("can not add event to epoll!");
        sk_on_global_error();
        return -1;
    }

    sk_on_start(epoll, userdata);

    for (;;) {
        loop_once(epoll, sfd, event);
    }

    close(epoll);
    sk_on_close(epoll);

    return 0;
}
