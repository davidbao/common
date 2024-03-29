//
//  Sender.cpp
//  common
//
//  Created by baowei on 2016/12/31.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "net/Sender.h"
#include "data/ByteArray.h"
#include "diag/Stopwatch.h"

using namespace Diag;

namespace Net {
    Sender::Sender() = default;

    Sender::~Sender() = default;

    void Sender::getEscapeBuffer(const uint8_t *buffer, off_t offset, size_t count, ByteArray &dst,
                                 const EscapeOption *escape) {
        ByteArray toEscapeBuffer(escape->toEscapeBuffer, escape->toEscapeLength);
        ByteArray escapeBuffer(escape->escapeBuffer, escape->escapeLength);
        size_t length = escape->length > 0 ? escape->length : count + escape->length;
        dst = ByteArray(buffer, count);
        dst = dst.replace(offset + escape->offset, length, escapeBuffer, toEscapeBuffer);
    }

    ssize_t Sender::send(const uint8_t *buffer, off_t offset, size_t count, const EscapeOption *escape) {
#ifdef DEBUG
        Stopwatch sw("Sender::send", 2000);
#endif

        ssize_t sendLen = 0;

        if (buffer == nullptr) {
            return 0;
        }
        if (offset < 0) {
            return 0;
        }
        if (count < 0) {
            return 0;
        }

        if (connected()) {
            if (escape == nullptr) {
                sendLen = send(buffer, offset, count);
            } else {
                ByteArray dst;
                getEscapeBuffer(buffer, offset, count, dst, escape);
                sendLen = send(dst.data(), offset, dst.count());
            }
            return sendLen;
        } else {
            return 0;
        }
    }

    ssize_t Sender::send(const ByteArray &buffer, const EscapeOption *escape) {
        return send(buffer.data(), 0, buffer.count(), escape);
    }

    ssize_t Sender::send(const String &str) {
        ByteArray buffer((const uint8_t *) str.c_str(), str.length());
        return send(buffer);
    }
}
