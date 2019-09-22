//
// Created by liushuai on 2019/7/10.
//

#include <algorithm>

#include "TCPServer.h"
#include "TCPStream.h"
#include "log.h"

namespace SocketPP {

TCPServer::TCPServer(int port)
        : SocketServer(port) {
    startSendThread();
}

TCPServer::~TCPServer() {
    stopped_ = true;
    msgQueueCondition_.notify_one();
    sendThread_->join();
    delete sendThread_;
}

ssize_t TCPServer::send(const Message& message) {
    if (!started_) {
        LOGE("not inited!");
        return SendResult::SocketNotInited;
    }

    std::lock_guard<std::mutex> lockStream(streamMutex_);
    LOGD("TCPServer::send: target=%d, len=%ld", message.target.fd, message.rawMsg.length());

    if (sendInterceptor_) {
        if (sendInterceptor_(message)) return SendResult::Intercepted;
    }

    const auto& iter = std::find(connectedStreams_.cbegin(), connectedStreams_.cend(), message.target);
    if (iter == connectedStreams_.cend()) {
        LOGE("StreamNotFound");
        return StreamNotFound;
    }

    const auto& stream = *iter;
    onSend(stream, message);

    LOGD("try to send to stream:%d", stream.fd);
    const auto& rawMsg = message.rawMsg;
    ssize_t ret = stream.send(rawMsg.data(), rawMsg.length());

    if (ret == -1) {
        LOGE("send failed!");
    } else {
        LOGD("send success! len:%ld", ret);
    }

    return ret;
}

void TCPServer::post(const Message& message) {
    if (!started_)
        return;

    msgQueueMutex_.lock();
    msgQueue_.push(message);
    msgQueueMutex_.unlock();
    msgQueueCondition_.notify_one();
}

void TCPServer::flush() {
    std::lock_guard<std::mutex> lock(msgQueueMutex_);

    while (!msgQueue_.empty()) {
        send(msgQueue_.front());
        msgQueue_.pop();
    }
}

void TCPServer::disconnectAllStreams() {
    std::lock_guard<std::mutex> lockStream(streamMutex_);

    for (auto stream:connectedStreams_) {
        onDisconnected(stream.fd);
    }
    connectedStreams_.clear();
}

const std::vector<TCPStream>& TCPServer::getStreams() {
    return connectedStreams_;
}

void TCPServer::startSendThread() {
    sendThread_ = new std::thread([this] {
        LOGD("sendThread running...");
        Message msg;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(msgQueueMutex_);
                LOGD("msgQueue_.size=%ld, msgQueueCondition_ will %s", msgQueue_.size(),
                     msgQueue_.empty() ? "waiting..." : "not wait!");
                msgQueueCondition_.wait(lock, [this] { return !msgQueue_.empty() || stopped_; });
                LOGD("msgQueueCondition_ wake! msgQueue_.size=%ld", msgQueue_.size());

                if (stopped_) return;

                msg = msgQueue_.front();
                msgQueue_.pop();
            }

            send(msg);
        }
    });
}

void TCPServer::setSendInterceptor(const MessageInterceptor& interceptor) {
    this->sendInterceptor_ = interceptor;
}

void TCPServer::setSendHandle(const MessageHandle& handle) {
    this->sendHandle_ = handle;
}

void TCPServer::setRecvHandle(const MessageHandle& handle) {
    this->recvHandle_ = handle;
}

void TCPServer::setConnHandle(const StreamHandle& handle) {
    connHandle_ = handle;
}

void TCPServer::setDiscHandle(const StreamHandle& handle) {
    discHandle_ = handle;
}

void TCPServer::onSend(const TCPStream&, const Message& message) {
    if (sendHandle_)
        sendHandle_(message);
}

void TCPServer::onStart(int efd) {
    SocketServer::onStart(efd);
    started_ = true;
}

void TCPServer::onConnected(int fd) {
    TCPStream stream(fd);

    LOGD("get an accept:%d", stream.fd);
    streamMutex_.lock();
    connectedStreams_.push_back(stream);
    streamMutex_.unlock();

    LOGD("connectedStreams_.size()=%ld", connectedStreams_.size());

    if (connHandle_)
        connHandle_(stream);
}

void TCPServer::onDisconnected(int fd) {
    std::lock_guard<std::mutex> lockStream(streamMutex_);
    auto iter = std::find_if(connectedStreams_.cbegin(), connectedStreams_.cend(),
                             [&](TCPStream stream) { return stream.fd == fd; });

    if (iter == connectedStreams_.cend()) {
        LOGE("fd:%d is not in connectedStreams_!", fd);
        return;
    }

    connectedStreams_.erase(iter);

    LOGD("connectedStreams_.size()=%ld", connectedStreams_.size());

    if (discHandle_)
        discHandle_(fd);
}

void TCPServer::onReceive(const Message& message) {
    if (recvHandle_)
        recvHandle_(message);
}

void TCPServer::onReceive(int fd, const byte* buf, size_t len) {
    TCPServer::onReceive(Message::create(fd, buf, len));
}

}   // namespace SocketPP
