//
//  Sender.h
//  common
//
//  Created by baowei on 2016/12/31.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Sender_h
#define Sender_h

#include "data/String.h"
#include "data/ByteArray.h"
#include "net/EscapeOption.h"

using namespace Data;

namespace Net {
    class Sender {
    public:
        Sender();

        virtual ~Sender();

        virtual bool connected() = 0;

        virtual ssize_t send(const uint8_t *buffer, off_t offset, size_t count) = 0;

        ssize_t send(const uint8_t *buffer, off_t offset, size_t count, const EscapeOption *escape);

        ssize_t send(const ByteArray &buffer, const EscapeOption *escape = nullptr);

    private:
        static void getEscapeBuffer(const uint8_t *buffer, off_t offset, size_t count,
                                    ByteArray &dst, const EscapeOption *escape);
    };
}

#endif // Sender_H
