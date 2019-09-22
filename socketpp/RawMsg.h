//
// Created by liushuai on 2019/7/9.
//

#pragma once

#include <cstddef>
#include <string>

#include "type.h"

namespace SocketPP {

struct RawMsg {
private:
    size_t len_ = 0;
    byte* data_ = nullptr;

public:
    inline void initMsg(const byte* data = nullptr, size_t len = 0) {
        if (data == nullptr || len == 0) {
            len_ = 0;
            data_ = nullptr;
            return;
        }

        len_ = len;
        data_ = new byte[len];
        memcpy(data_, data, len);
    }

    inline RawMsg& moveMsg(RawMsg& msg) {
        if (&msg == this)
            return *this;

        len_ = msg.len_;

        delete[] data_;
        data_ = msg.data_;
        msg.data_ = nullptr;

        return *this;
    }

    explicit RawMsg(const byte* data = nullptr, size_t len = 0) {
        initMsg(data, len);
    }

    explicit RawMsg(const std::string& msg) {
        initMsg((byte*) msg.data(), msg.length());
    }

    RawMsg(const RawMsg& msg) {
        initMsg(msg.data_, msg.len_);
    }

    RawMsg(RawMsg&& msg) noexcept {
        moveMsg(msg);
    }

    RawMsg& operator=(const RawMsg& msg) {
        if (&msg == this)
            return *this;

        delete[] data_;
        initMsg(msg.data_, msg.len_);
        return *this;
    }

    RawMsg& operator=(RawMsg&& msg) noexcept {
        return moveMsg(msg);
    }

    byte* data() const {
        return data_;
    }

    size_t length() const {
        return len_;
    }

    std::string toString() const {
        return std::string((char*) data_, 0, len_);
    }

    ~RawMsg() {
        delete[] data_;
    }
};

}
