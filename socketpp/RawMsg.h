//
// Created by liushuai on 2019/7/9.
//

#pragma once

#include <cstddef>
#include <string>

#include "type.h"

struct RawMsg {
private:
    size_t _len = 0;
    byte *_data = nullptr;

public:
    inline void initMsg(const byte *data = nullptr, size_t len = 0) {
        if (data == nullptr || len == 0) {
            _len  = 0;
            _data = nullptr;
            return;
        }

        _len  = len;
        _data = new byte[len];
        memcpy(_data, data, len);
    }

    inline void moveMsg(RawMsg& msg) {
        _len  = msg._len;
        _data = msg._data;
        msg._data = nullptr;
    }

    explicit RawMsg(const byte *data = nullptr, size_t len = 0) {
        initMsg(data, len);
    }

    explicit RawMsg(const std::string& msg) {
        initMsg((byte*)msg.data(), msg.length());
    }

    RawMsg(const RawMsg& msg) {
        initMsg(msg._data, msg._len);
    }

    RawMsg(RawMsg&& msg) noexcept {
        moveMsg(msg);
    }

    RawMsg& operator=(const RawMsg& msg) {
        if (&msg == this)
            return *this;

        delete[] _data;
        initMsg(msg._data, msg._len);
        return *this;
    }

    RawMsg& operator=(RawMsg&& msg) noexcept {
        if (&msg == this)
            return *this;

        delete[] _data;
        moveMsg(msg);
        return *this;
    }

    byte *data() const {
        return _data;
    }

    size_t length() const {
        return _len;
    }

    std::string toString() const {
        return std::string((char *) _data, 0, _len);
    }

    ~RawMsg() {
        delete[] _data;
    }
};
