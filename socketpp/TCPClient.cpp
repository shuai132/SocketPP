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

ssize_t TCPClient::send(const Message &message) {
    if (!_connected) {
        LOGE("not connected!");
        return SendResult::NotConnected;
    }

    LOGD("TCPClient::send: target=%d, len=%ld", message.target.fd, message.rawMsg.length());

    if (_sendInterceptor) {
        if (_sendInterceptor(message)) return SendResult::Intercepted;
    }

    onSend(message);

    LOGD("try to send to stream:%d", _streamPeer.fd);
    const auto &rawMsg = message.rawMsg;
    ssize_t ret = _streamPeer.send(rawMsg.data(), rawMsg.length());

    if (ret == -1) {
        LOGE("send failed!");
    } else {
        LOGD("send success! len:%ld", ret);
    }

    return ret;
}

ssize_t TCPClient::send(const std::string &str) {
    return send(Message(str));
}

void TCPClient::post(const Message &message) {
    if (!_connected)
        return;

    _msgQueueMutex.lock();
    _msgQueue.push(message);
    _msgQueueMutex.unlock();
    _msgQueueCondition.notify_one();
}

void TCPClient::post(const std::string &str) {
    post(Message(str));
}

void TCPClient::flush() {
    std::lock_guard<std::mutex> lock(_msgQueueMutex);

    while (!_msgQueue.empty()) {
        send(_msgQueue.front());
        _msgQueue.pop();
    }
}

void TCPClient::disconnect() {
    onDisconnected(_streamPeer.fd);
}

void TCPClient::startSendThread() {
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

void TCPClient::setSendInterceptor(const MessageInterceptor &interceptor) {
    this->_sendInterceptor = interceptor;
}

void TCPClient::setSendHandle(const MessageHandle &handle) {
    this->_sendHandle = handle;
}

void TCPClient::setRecvHandle(const MessageHandle &handle) {
    this->_recvHandle = handle;
}

void TCPClient::setConnHandle(const StreamHandle &handle) {
    _connHandle = handle;
}

void TCPClient::setDiscHandle(const StreamHandle &handle) {
    _discHandle = handle;
}

void TCPClient::onSend(const Message &message) {
    if (_sendHandle)
        _sendHandle(message);
}

void TCPClient::onStart(int efd) {
    SocketClient::onStart(efd);
    _connected = true;
}

void TCPClient::onConnected(int fd) {
    _streamPeer = fd;

    if (_connHandle)
        _connHandle(_streamPeer);
}

void TCPClient::onDisconnected(int fd) {
    if (_discHandle)
        _discHandle(fd);
}

void TCPClient::onReceive(const Message &message) {
    if (_recvHandle)
        _recvHandle(message);
}

void TCPClient::onReceive(int fd, const byte *buf, size_t len) {
    TCPClient::onReceive(Message::create(fd, buf, len));
}

}   // namespace SocketPP
