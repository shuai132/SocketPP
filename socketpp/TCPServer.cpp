//
// Created by liushuai on 2019/7/10.
//

#include <algorithm>

#include "TCPServer.h"
#include "TCPStream.h"
#include "epoll.h"
#include "log.h"

namespace SocketPP {

TCPServer::TCPServer(int port)
        : Socket(port) {
    startSendThread();
}

ssize_t TCPServer::send(const Message &message) {
    if (!_inited) {
        LOGE("not inited!");
        return SendResult::SocketNotInited;
    }

    std::lock_guard<std::mutex> lockStream(_streamMutex);
    LOGD("TCPServer::send: target=%d, len=%ld", message.target.fd, message.rawMsg.length());

    if (_sendInterceptor) {
        if (_sendInterceptor(message)) return SendResult::Intercepted;
    }

    auto iter = std::find(_connectedStreams.cbegin(), _connectedStreams.cend(), message.target);
    if (iter == _connectedStreams.cend()) {
        LOGE("StreamNotFound");
        return StreamNotFound;
    }

    auto stream = *iter;
    onSend(stream, message);

    LOGD("try to send to stream:%d", stream.fd);
    auto& rawMsg = message.rawMsg;
    ssize_t ret = stream.send(rawMsg.data(), rawMsg.length());

    if (ret == -1) {
        LOGE("send failed!");
    } else {
        LOGD("send success! len:%ld", ret);
    }

    return ret;
}

void TCPServer::post(const Message &message) {
    if (!_inited)
        return;

    _msgQueueMutex.lock();
    _msgQueue.push(message);
    _msgQueueMutex.unlock();
    _msgQueueCondition.notify_one();
}

void TCPServer::flush() {
    std::lock_guard<std::mutex> lock(_msgQueueMutex);

    while (!_msgQueue.empty()) {
        send(_msgQueue.front());
        _msgQueue.pop();
    }
}

void TCPServer::disconnectAllStreams() {
    std::lock_guard<std::mutex> lockStream(_streamMutex);

    for(auto stream:_connectedStreams) {
        onDisconnected(stream.fd);
    }
    _connectedStreams.clear();
}

const std::vector<TCPStream>& TCPServer::getStreams() {
    return _connectedStreams;
}

void TCPServer::startSendThread() {
    new std::thread([this]{
        LOGD("sendThread running...");
        Message msg;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(_msgQueueMutex);
                LOGD("_msgQueue.size=%ld, _msgQueueCondition will %s", _msgQueue.size(),
                     _msgQueue.empty() ? "waiting..." : "not wait!");
                _msgQueueCondition.wait(lock, [this] { return !_msgQueue.empty(); });
                LOGD("_msgQueueCondition wake! _msgQueue.size=%ld", _msgQueue.size());

                msg = _msgQueue.front();
                _msgQueue.pop();
            }

            send(msg);
        }
    });
}

void TCPServer::setSendInterceptor(const MessageInterceptor &interceptor) {
    this->_sendInterceptor = interceptor;
}

void TCPServer::setSendHandle(const MessageHandle &handle) {
    this->_sendHandle = handle;
}

void TCPServer::setRecvHandle(const MessageHandle &handle) {
    this->_recvHandle = handle;
}

void TCPServer::setConnHandle(const StreamHandle &handle) {
    _connHandle = handle;
}

void TCPServer::setDiscHandle(const StreamHandle &handle) {
    _discHandle = handle;
}

void TCPServer::onSend(TCPStream stream, const Message &message) {
    if (_sendHandle)
        _sendHandle(message);
}

void TCPServer::onStart(int efd) {
    Socket::onStart(efd);
    _inited = true;
}

void TCPServer::onConnected(int fd) {
    TCPStream stream(fd);

    LOGD("get an accept:%d", stream.fd);
    _streamMutex.lock();
    _connectedStreams.push_back(stream);
    _streamMutex.unlock();

    LOGD("_connectedStreams.size()=%ld", _connectedStreams.size());

    if (_connHandle)
        _connHandle(stream);
}

void TCPServer::onDisconnected(int fd) {
    std::lock_guard<std::mutex> lockStream(_streamMutex);
    auto iter = std::find_if(_connectedStreams.cbegin(), _connectedStreams.cend(), [&](TCPStream stream){ return stream.fd == fd; });

    if (iter == _connectedStreams.cend()) {
        LOGE("fd:%d is not in _connectedStreams!", fd);
        return;
    }

    _connectedStreams.erase(iter);

    LOGD("_connectedStreams.size()=%ld", _connectedStreams.size());

    if (_discHandle)
        _discHandle(fd);
}

void TCPServer::onReceive(const Message &message) {
    if (_recvHandle)
        _recvHandle(message);
}

void TCPServer::onReceive(int fd, const byte *buf, size_t len) {
    TCPServer::onReceive(Message::create(fd, buf, len));
}

}   // namespace SocketPP
