//
// Created by liushuai on 2019/7/9.
//

#pragma once

#include <cstdint>
#include <cassert>

#include "RawMsg.h"
#include "TCPStream.h"

class Message {
public:
    TCPStream target;

    RawMsg *rawMsg = nullptr;

    Message() : target(0), rawMsg(nullptr) {}

    Message(TCPStream target, std::string msg) : target(target), rawMsg(new RawMsg(msg)) {}

    /**
     * @param target
     * @param data
     * @param len
     * @return
     */
    static Message create(TCPStream target, const byte *data, size_t len) {
        return {target, new RawMsg(data, len)};
    }

    void destroy() {
        delete rawMsg;
        rawMsg = nullptr;
    }

private:
    Message(TCPStream target, RawMsg *rawMsg)
            : target(target), rawMsg(rawMsg) {
    }
};
