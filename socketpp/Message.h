//
// Created by liushuai on 2019/7/9.
//

#pragma once

#include <cstddef>
#include <cassert>

#include "RawMsg.h"
#include "TCPStream.h"

namespace SocketPP {

class Message {
public:
    Message() = default;

    Message(TCPStream target, const std::string& msg) : target(target), rawMsg(msg) {}

    Message(const std::string& msg) : target(0), rawMsg(msg) {}

    Message(TCPStream target, RawMsg rawMsg) : target(target), rawMsg(std::move(rawMsg)) {}

    /**
     * @param target
     * @param data
     * @param len
     * @return
     */
    static Message create(TCPStream target, const byte* data, size_t len) {
        return {target, RawMsg(data, len)};
    }

public:
    TCPStream target;

    RawMsg rawMsg;
};

}
