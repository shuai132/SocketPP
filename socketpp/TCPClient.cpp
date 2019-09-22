//
// Created by liushuai on 2019/7/13.
//

#include <algorithm>

#include "TCPClient.h"
#include "TCPStream.h"
#include "log.h"

namespace SocketPP {

TCPClient::TCPClient(const std::string& ip, int port)
        : SocketClient(ip, port) {
    startSendThread();
}

ssize_t TCPClient::send(const Message& message) {
    if (!connected_) {
        LOGE("not connected!");
        return SendResult::NotConnected;
    }

    LOGD("TCPClient::send: target=%d, len=%ld", message.target.fd, message.rawMsg.length());

    if (sendInterceptor_) {
        if (sendInterceptor_(message)) return SendResult::Intercepted;
    }

    onSend(message);

    LOGD("try to send to stream:%d", streamPeer_.fd);
    const auto& rawMsg = message.rawMsg;
    ssize_t ret = streamPeer_.send(rawMsg.data(), rawMsg.length());

    if (ret == -1) {
        LOGE("send failed!");
    } else {
        LOGD("send success! len:%ld", ret);
    }

    return ret;
}

ssize_t TCPClient::send(const std::string& str) {
    return send(Message(str));
}

void TCPClient::post(const Message& message) {
    if (!connected_)
        return;

    msgQueueMutex_.lock();
    msgQueue_.push(message);
    msgQueueMutex_.unlock();
    msgQueueCondition_.notify_one();
}

void TCPClient::post(const std::string& str) {
    post(Message(str));
}

void TCPClient::flush() {
    std::lock_guard<std::mutex> lock(msgQueueMutex_);

    while (!msgQueue_.empty()) {
        send(msgQueue_.front());
        msgQueue_.pop();
    }
}

void TCPClient::disconnect() {
    onDisconnected(streamPeer_.fd);
}

void TCPClient::startSendThread() {
    new std::thread([this] {
        LOGD("sendThread running...");
        Message msg;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(msgQueueMutex_);
                LOGD("msgQueue_.size=%ld, msgQueueCondition_ will %s", msgQueue_.size(),
                     msgQueue_.empty() ? "waiting..." : "not wait!");
                msgQueueCondition_.wait(lock, [this] { return !msgQueue_.empty(); });
                LOGD("msgQueueCondition_ wake! msgQueue_.size=%ld", msgQueue_.size());

                msg = msgQueue_.front();
                msgQueue_.pop();
            }

            send(msg);
        }
    });
}

void TCPClient::setSendInterceptor(const MessageInterceptor& interceptor) {
    this->sendInterceptor_ = interceptor;
}

void TCPClient::setSendHandle(const MessageHandle& handle) {
    this->sendHandle_ = handle;
}

void TCPClient::setRecvHandle(const MessageHandle& handle) {
    this->recvHandle_ = handle;
}

void TCPClient::setConnHandle(const StreamHandle& handle) {
    connHandle_ = handle;
}

void TCPClient::setDiscHandle(const StreamHandle& handle) {
    discHandle_ = handle;
}

void TCPClient::onSend(const Message& message) {
    if (sendHandle_)
        sendHandle_(message);
}

void TCPClient::onStart(int efd) {
    SocketClient::onStart(efd);
    connected_ = true;
}

void TCPClient::onConnected(int fd) {
    streamPeer_ = fd;

    if (connHandle_)
        connHandle_(streamPeer_);
}

void TCPClient::onDisconnected(int fd) {
    if (discHandle_)
        discHandle_(fd);
}

void TCPClient::onReceive(const Message& message) {
    if (recvHandle_)
        recvHandle_(message);
}

void TCPClient::onReceive(int fd, const byte* buf, size_t len) {
    TCPClient::onReceive(Message::create(fd, buf, len));
}

}   // namespace SocketPP
