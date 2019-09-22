#include "SocketPP.h"

using namespace SocketPP;

int main() {
    const int port = 6000;
    LOGI("echo server port:%d", port);

    TCPServer server(port);

    server.setConnHandle([] (const TCPStream& stream) {
        LOGI("on connected: fd=%d", stream.fd);
    });

    server.setDiscHandle([] (const TCPStream& stream) {
        LOGI("on disconnected: fd=%d", stream.fd);
    });

    server.setRecvHandle([&] (const Message& message) {
        LOGI("on receive: fd=%d, msg:%s", message.target.fd, message.rawMsg.toString().c_str());
        server.send(message);
    });

    return server.loop();
}
