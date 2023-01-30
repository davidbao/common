//
//  IOPort.h
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#ifndef IOPort_h
#define IOPort_h

#include "data/String.h"
#include "net/Sender.h"
#include "net/Receiver.h"

using namespace Data;
using namespace Net;

namespace IO {
    class IOPort : public Sender, public Receiver {
    public:
        using Sender::send;
        using Receiver::receive;

        explicit IOPort();

        ~IOPort() override;

        bool connected() override;

        size_t available() override;

        bool useReceiveTimeout() const override;

        ssize_t send(const uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count) override;

        virtual bool open();

        virtual const String &name() const = 0;

        void close();

        bool isOpen() const;

    protected:
#ifdef WIN32
        typedef void* Handle;
        static constexpr Handle invalidHandle = ((Handle)-1);
#else
        typedef int Handle;
        static const Handle invalidHandle = -1;
#endif
        Handle _handle;
    };
}
#endif // IOPort_h
