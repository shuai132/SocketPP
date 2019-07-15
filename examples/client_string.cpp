#include "SocketPP.h"
#include "log.h"

int main() {
    const int port = 6000;
    LOGI("client will connect port:%d", port);

    SocketPP::TCPClient client("127.0.0.1", port);

    client.setConnHandle([&] (const TCPStream &stream) {
        LOGI("on connected: fd=%d", stream.fd);
        client.send("hello");
    });

    client.setDiscHandle([] (const TCPStream &stream) {
        LOGI("on disconnected: fd=%d", stream.fd);
    });

    client.setRecvHandle([&] (const Message &message) {
        LOGI("on receive: fd=%d, msg:%s", message.target.fd, message.rawMsg.toString().c_str());
    });

    return client.loop();
}
