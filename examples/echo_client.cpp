#include "SocketPP.h"

using namespace SocketPP;

int main() {
    const int port = 6000;
    TCPClient client("127.0.0.1", port);
    client.setConnHandle([&] (const TCPStream& stream) {
        client.send("hello\n");
    });
    client.setRecvHandle([&] (const Message& message) {
        LOGI("on receive: msg:%s", message.rawMsg.toString().c_str());
    });
    return client.loop();
}
