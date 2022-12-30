//
//  UdpClient.h
//  common
//
//  Created by baowei on 2014/5/15.
//  Copyright © 2014 com. All rights reserved.
//

#ifndef UdpClient_h
#define UdpClient_h

#include "data/ValueType.h"
#include "data/PList.h"

using namespace Data;

namespace Net {
    class UdpClient {
    public:
        UdpClient();

        virtual ~UdpClient();

        bool open(const String &host, uint16_t port);

        bool open(uint16_t port);

        void close();

        ssize_t write(const String &host, uint16_t port, const uint8_t *data, size_t count);

        ssize_t write(uint16_t port, const uint8_t *data, size_t count);

        ssize_t write(uint16_t port, const ByteArray &buffer);

        int sendBufferSize() const;

        void setSendBufferSize(int bufferSize);

        bool setBlocking(bool blocking = false);

    private:
        bool isBroadcast(const String &host) const;

    public:
        static const int MaxSendBufferSize = 65535;

    private:
        int _socket;
    };

    typedef PList<UdpClient> UdpClients;

}

#endif // UdpClient_h
