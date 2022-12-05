//
//  EthernetInfo.h
//  common
//
//  Created by baowei on 07/10/2017.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef EthernetInfo_h
#define EthernetInfo_h

#include "data/ValueType.h"
#include "xml/XmlTextReader.h"
#include "xml/XmlTextWriter.h"
#include "json/JsonTextReader.h"
#include "json/JsonTextWriter.h"
#include "driver/channels/TcpChannelContext.h"

using namespace Xml;
using namespace Drivers;

namespace Net
{
    class Timeout
    {
    public:
        TimeSpan send;
        TimeSpan receive;
        TimeSpan open;
        TimeSpan close;
        
        Timeout();
        
        void operator=(const Timeout& value);
        bool operator==(const Timeout& value) const;
        bool operator!=(const Timeout& value) const;
        
        const String toString() const;
        
        void copyFrom(const Timeout* value);
        
        void read(Stream* stream);
        void write(Stream* stream) const;
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        void read(JsonTextReader& reader);
        void write(JsonTextWriter& writer) const;
        void write(JsonNode& node) const;
        
    public:
        static bool parse(const String& str, Timeout& timeout);
    };
    class TcpInfo
    {
    public:
        String address;
        int port;
        
        int sendBufferSize;
        int receiveBufferSize;
        bool noDelay;
        bool block;
        Timeout timeout;
        
        TcpInfo();
        virtual ~TcpInfo();
        
        virtual const String toString() const;
        
        void operator=(const TcpInfo& value);
        bool operator==(const TcpInfo& value) const;
        bool operator!=(const TcpInfo& value) const;
        
        virtual void copyFrom(const TcpInfo* value);
        
        virtual bool isEmpty() const;
        
        virtual void read(Stream* stream);
        virtual void write(Stream* stream) const;
        virtual void read(XmlTextReader& reader);
        virtual void write(XmlTextWriter& writer) const;
        virtual void read(JsonTextReader& reader);
        virtual void write(JsonTextWriter& writer) const;
        virtual void write(JsonNode& node) const;
        
    };
    class TcpServerInfo : public TcpInfo
    {
    public:
        int maxConnections;
        TcpChannelContext::Types receiverType;
        TcpChannelContext::Types senderType;
        
        TcpServerInfo();
        
        const String toString() const override;
        
        void operator=(const TcpServerInfo& value);
        bool operator==(const TcpServerInfo& value) const;
        bool operator!=(const TcpServerInfo& value) const;
        
        void copyFrom(const TcpInfo* value) override;
        
        bool isEmpty() const override;
        
        void read(Stream* stream) override;
        void write(Stream* stream) const override;
        void read(XmlTextReader& reader) override;
        void write(XmlTextWriter& writer) const override;
        void read(JsonTextReader& reader) override;
        void write(JsonTextWriter& writer) const override;
        void write(JsonNode& node) const override;
    };
    
    class UdpInfo
    {
    public:
        String address;
        int port;
        
        int sendBufferSize;
        
        UdpInfo();
        
        const String toString() const;
        
        void operator=(const UdpInfo& value);
        bool operator==(const UdpInfo& value) const;
        bool operator!=(const UdpInfo& value) const;
        
        void copyFrom(const UdpInfo* value);
        
        bool isEmpty() const;
        
        void read(Stream* stream);
        void write(Stream* stream) const;
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        void read(JsonTextReader& reader);
        void write(JsonTextWriter& writer) const;
        void write(JsonNode& node) const;
    };
    class UdpServerInfo
    {
    public:
        String address;
        int port;
        
        int receiveBufferSize;
        
        UdpServerInfo();
        
        const String toString() const;
        
        void operator=(const UdpServerInfo& value);
        bool operator==(const UdpServerInfo& value) const;
        bool operator!=(const UdpServerInfo& value) const;
        
        void copyFrom(const UdpServerInfo* value);
        
        bool isEmpty() const;
        
        void read(Stream* stream);
        void write(Stream* stream) const;
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        void read(JsonTextReader& reader);
        void write(JsonTextWriter& writer) const;
        void write(JsonNode& node) const;
    };
}

#endif /* EthernetInfo_h */
