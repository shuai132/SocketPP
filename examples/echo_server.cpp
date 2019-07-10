#include "SocketPP.h"
#include "log.h"

int main() {
    const int port = 6000;
    LOGI("echo server port:%d", port);

    SocketPP socket(port);

    socket.setConnHandle([] (TCPStream stream) {
        LOGI("on connected: fd=%d", stream.fd);
    });

    socket.setDiscHandle([] (TCPStream stream) {
        LOGI("on disconnected: fd=%d", stream.fd);
    });

    socket.setRecvHandle([&socket] (Message message) {
        LOGI("on receive: fd=%d, msg:%s", message.target.fd, message.rawMsg->toString().c_str());
        socket.send(message);
    });

    return socket.loop();
}
