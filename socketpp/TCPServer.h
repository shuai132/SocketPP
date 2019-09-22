//
// Created by liushuai on 2019/7/10.
//

#pragma once

#include <cstring>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>

#include "SocketServer.h"
#include "Message.h"

namespace SocketPP {

class TCPServer : public SocketServer {
public:
    using MessageInterceptor = std::function<bool(const Message&)>;
    using MessageHandle = std::function<void(const Message&)>;
    using StreamHandle = std::function<void(const TCPStream&)>;

    enum SendResult {
        StreamNotFound = -3,
        SocketNotInited = -2,
        SendError = -1,
        Intercepted = 0,
    };

public:
    explicit TCPServer(int port = 6000);

    ~TCPServer() override;

    /**
     * send message immediately with thread safe
     * @param message
     * @return >0 : the length has send
     *          0 : if be intercept
     *         -1 : send error(may stream closed)
     *         -2 : not inited
     *         -3 : stream not found
     */
    ssize_t send(const Message& message);

    // post message to queue will be send automatic later
    void post(const Message& message);

    // flush all posted messages
    void flush();

    void disconnectAllStreams();

    const std::vector<TCPStream>& getStreams();

    /**
     * @param interceptor return true: intercept it, false or not.
     */
    void setSendInterceptor(const MessageInterceptor& interceptor);

    void setSendHandle(const MessageHandle& handle);

    void setRecvHandle(const MessageHandle& handle);

    void setConnHandle(const StreamHandle& handle);

    void setDiscHandle(const StreamHandle& handle);

    void onReceive(const Message& message);

public:
    void onStart(int efd) override;

    void onConnected(int fd) override;

    void onDisconnected(int fd) override;

    void onReceive(int fd, const byte* buf, size_t len) override;

private:
    void startSendThread();

    void onSend(const TCPStream& stream, const Message& message);

private:
    bool started_ = false;
    bool stopped_ = false;

    std::thread* sendThread_ = nullptr;

    std::vector<TCPStream> connectedStreams_;
    std::mutex streamMutex_;

    std::queue<Message> msgQueue_;
    std::mutex msgQueueMutex_;
    std::condition_variable msgQueueCondition_;

    MessageInterceptor sendInterceptor_ = nullptr;
    MessageHandle sendHandle_ = nullptr;
    MessageHandle recvHandle_ = nullptr;

    StreamHandle connHandle_ = nullptr;
    StreamHandle discHandle_ = nullptr;
};

}   // namespace SocketPP
