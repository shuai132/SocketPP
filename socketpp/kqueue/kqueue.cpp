//
// Created by shuai on 2019-07-11.
//

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include "kqueue.h"
#include "socket_event.h"
#include "log.h"


#define return_error_if(r, fmt, ...)        \
    do {                                    \
        if (r) {                            \
            LOGE(fmt, ##__VA_ARGS__);       \
            sk_on_global_error();           \
            return;                         \
        }                                   \
    } while(0)

#define return_err_int_if(r, fmt, ...)      \
    do {                                    \
        if (r) {                            \
            LOGE(fmt, ##__VA_ARGS__);       \
            sk_on_global_error();           \
            return r;                       \
        }                                   \
    } while(0)


static const int kReadEvent = 1;
static const int kWriteEvent = 2;


static void setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return_error_if(flags < 0, "fcntl failed");
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return_error_if(r < 0, "fcntl failed");
}

static void updateEvents(int efd, int fd, int events, bool modify) {
    struct kevent ev[2];
    int n = 0;

    if (events & kReadEvent) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *) (intptr_t) fd);
    } else if (modify) {
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void *) (intptr_t) fd);
    }

    if (events & kWriteEvent) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void *) (intptr_t) fd);
    } else if (modify) {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *) (intptr_t) fd);
    }

    LOGD("%s fd %d events read %d write %d\n", modify ? "mod" : "add", fd, events & kReadEvent, events & kWriteEvent);
    int r = kevent(efd, ev, n, NULL, 0, NULL);
    if (r) {
        LOGE("kevent failed ");
        sk_on_error(efd);
        return;
    }
}

static void handleAccept(int efd, int fd) {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd = accept(fd, (struct sockaddr *) &raddr, &rsz);
    if (cfd < 0) {
        LOGE("accept failed");
        sk_on_error(efd);
        return;
    }

    sk_on_connected(efd, cfd);

    sockaddr_in peer;
    socklen_t alen = sizeof(peer);
    int r = getpeername(cfd, (sockaddr *) &peer, &alen);
    if (r < 0) {
        LOGE("getpeername failed");
        sk_on_error(efd);
        return;
    }

    LOGD("accept a connection from %s\n", inet_ntoa(raddr.sin_addr));
    setNonBlock(cfd);
    updateEvents(efd, cfd, kReadEvent | kWriteEvent, false);
}

static void handleRead(int efd, int fd) {
    char buf[4096];
    int n = 0;
    while ((n = ::read(fd, buf, sizeof buf)) > 0) {
        LOGD("read %d bytes", n);
        sk_on_read_data(efd, fd, (byte*)buf, n);
    }

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;

    if (n < 0) {
        LOGE("read error");  //实际应用中，n<0应当检查各类错误，如EINTR
        sk_on_read_error(efd, fd);
        return;
    }

    LOGD("fd %d closed", fd);
    close(fd);
    sk_on_disconnected(efd, fd);
}

static void handleWrite(int efd, int fd) {
    //实际应用应当实现可写时写出数据，无数据可写才关闭可写事件
    updateEvents(efd, fd, kReadEvent, true);
}

static void loop_once(int efd, int lfd, int waitms, bool isServer) {
    struct timespec timeout;
    timeout.tv_sec = waitms / 1000;
    timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;
    const int kMaxEvents = 20;
    struct kevent activeEvs[kMaxEvents];
    int n = kevent(efd, NULL, 0, activeEvs, kMaxEvents, &timeout);

    for (int i = 0; i < n; i++) {
        int fd = (int) (intptr_t) activeEvs[i].udata;
        int events = activeEvs[i].filter;
        if (events == EVFILT_READ) {
            if (isServer) {
                if (fd == lfd) {
                    handleAccept(efd, fd);
                } else {
                    handleRead(efd, fd);
                }
            } else {
                handleRead(efd, fd);
            }
        } else if (events == EVFILT_WRITE) {
            handleWrite(efd, fd);
        } else {
            return_error_if(1, "unknown event");
        }
    }
}

int kq_server_start_loop(int port, void *userdata) {
    int epollfd = kqueue();
    return_err_int_if(epollfd < 0, "kqueue failed");

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    return_err_int_if(listenfd < 0, "socket failed");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int r = ::bind(listenfd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
    return_err_int_if(r != 0, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));

    r = listen(listenfd, 20);
    return_err_int_if(r != 0, "listen failed %d %s", errno, strerror(errno));

    LOGD("fd %d listening at %d", listenfd, port);
    setNonBlock(listenfd);
    updateEvents(epollfd, listenfd, kReadEvent, false);

    sk_on_start(epollfd, userdata);

    for (;;) {
        //实际应用应当注册信号处理函数，退出时清理资源
        loop_once(epollfd, listenfd, 10000, true);
    }

    close(epollfd);
    sk_on_close(epollfd);

    return 0;
}

int kq_connect_and_loop(const char *host, int port, void *userdata) {
    int kq = kqueue();
    return_err_int_if(kq < 0, "kqueue failed");

    int sckfd = socket(AF_INET, SOCK_STREAM, 0);
    return_err_int_if(sckfd < 0, "socket failed");

    updateEvents(kq, sckfd, kReadEvent, false);

    sk_on_start(kq, userdata);

    struct hostent *hp = gethostbyname(host);
    return_err_int_if(hp == NULL, "gethostbyname");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr *)hp->h_addr);
    memset(&(addr.sin_zero), 0, 8);

    ssize_t fd = connect(sckfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    return_err_int_if(fd < 0, "connect failed");

    setNonBlock(sckfd);
    updateEvents(kq, sckfd, kReadEvent, false);

    sk_on_connected(kq, sckfd);

    for (;;) {
        //实际应用应当注册信号处理函数，退出时清理资源
        loop_once(kq, sckfd, 10000, false);
    }

    close(kq);
    sk_on_close(kq);

    return 0;
}
