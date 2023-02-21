//
//  EthernetInfo.cpp
//  common
//
//  Created by baowei on 07/10/2017.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "net/EthernetInfo.h"
#include "data/StringArray.h"
#include "data/Convert.h"
#include <cassert>

namespace Net {
    Timeout::Timeout() : send(0, 0, 3), receive(0, 0, 3), open(0, 0, 3), close(0, 0, 3) {
    }

    Timeout &Timeout::operator=(const Timeout &value) {
        this->copyFrom(&value);
        return *this;
    }

    bool Timeout::operator==(const Timeout &value) const {
        return this->send == value.send &&
               this->receive == value.receive &&
               this->open == value.open &&
               this->close == value.close;
    }

    bool Timeout::operator!=(const Timeout &value) const {
        return !operator==(value);
    }

    String Timeout::toString() const {
        return String::convert("open:{%s};close:{%s};send:{%s};receive:{%s}",
                               open.toString().c_str(), close.toString().c_str(),
                               send.toString().c_str(), receive.toString().c_str());
    }

    void Timeout::copyFrom(const Timeout *value) {
        this->send = value->send;
        this->receive = value->receive;
        this->open = value->open;
        this->close = value->close;
    }

    void Timeout::read(Stream *stream) {
        this->send.read(stream);
        this->receive.read(stream);
        this->open.read(stream);
        this->close.read(stream);
    }

    void Timeout::write(Stream *stream) const {
        this->send.write(stream);
        this->receive.write(stream);
        this->open.write(stream);
        this->close.write(stream);
    }

    void Timeout::read(XmlTextReader &reader) {
        TimeSpan::parse(reader.getAttribute("send"), send);
        TimeSpan::parse(reader.getAttribute("receive"), receive);
        TimeSpan::parse(reader.getAttribute("open"), open);
        TimeSpan::parse(reader.getAttribute("close"), close);
    }

    void Timeout::write(XmlTextWriter &writer) const {
        writer.writeAttribute("send", send);
        writer.writeAttribute("receive", receive);
        writer.writeAttribute("open", open);
        writer.writeAttribute("close", close);
    }

    void Timeout::read(JsonTextReader &reader) {
        TimeSpan::parse(reader.getAttribute("send"), send);
        TimeSpan::parse(reader.getAttribute("receive"), receive);
        TimeSpan::parse(reader.getAttribute("open"), open);
        TimeSpan::parse(reader.getAttribute("close"), close);
    }

    void Timeout::write(JsonTextWriter &writer) const {
        writer.writeAttribute("send", send);
        writer.writeAttribute("receive", receive);
        writer.writeAttribute("open", open);
        writer.writeAttribute("close", close);
    }

    void Timeout::write(JsonNode &node) const {
        node.add(JsonNode("send", send));
        node.add(JsonNode("receive", receive));
        node.add(JsonNode("open", open));
        node.add(JsonNode("close", close));
    }

    bool Timeout::parse(const String &str, Timeout &timeout) {
        // open:{00:00:10};close:{00:02:00};send:{00:00:05};receive:{00:00:05}
        Timeout temp;
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        for (uint32_t i = 0; i < texts.count(); i++) {
            StringArray values;
            Convert::splitItems(texts[i], values, ':');
            if (values.count() == 2) {
                String name = String::trim(values[0], ' ', '{', '}');
                String value = String::trim(values[1], ' ', '{', '}');

                if (String::equals(name, "open", true)) {
                    TimeSpan::parse(value, temp.open);
                } else if (String::equals(name, "close", true)) {
                    TimeSpan::parse(value, temp.close);
                } else if (String::equals(name, "send", true)) {
                    TimeSpan::parse(value, temp.send);
                } else if (String::equals(name, "receive", true)) {
                    TimeSpan::parse(value, temp.receive);
                }
            } else {
                return false;
            }
        }
        if (texts.count() > 0) {
            timeout.copyFrom(&temp);
            return true;
        }
        return false;
    }

    TcpInfo::TcpInfo() {
        address = String::Empty;
        port = 0;
        sendBufferSize = 0; // default size
        receiveBufferSize = 0; // default size
        noDelay = false;
        block = false;
    }

    TcpInfo::~TcpInfo() = default;

    String TcpInfo::toString() const {
        return String::convert("address: %s, port: %d", address.c_str(), port);
    }

    TcpInfo &TcpInfo::operator=(const TcpInfo &value) {
        this->copyFrom(&value);
        return *this;
    }

    bool TcpInfo::operator==(const TcpInfo &value) const {
        return this->address == value.address &&
               this->port == value.port &&
               this->sendBufferSize == value.sendBufferSize &&
               this->receiveBufferSize == value.receiveBufferSize &&
               this->noDelay == value.noDelay &&
               this->timeout == value.timeout &&
               this->block == value.block;
    }

    bool TcpInfo::operator!=(const TcpInfo &value) const {
        return !operator==(value);
    }

    void TcpInfo::copyFrom(const TcpInfo *value) {
        this->address = value->address;
        this->port = value->port;
        this->sendBufferSize = value->sendBufferSize;
        this->receiveBufferSize = value->receiveBufferSize;
        this->noDelay = value->noDelay;
        this->timeout.copyFrom(&value->timeout);
        this->block = value->block;
    }

    bool TcpInfo::isEmpty() const {
        return this->address.isNullOrEmpty() || this->port == 0;
    }

    void TcpInfo::read(Stream *stream) {
        this->address.read(stream);
        this->port = stream->readInt32();
        this->sendBufferSize = stream->readInt32();
        this->receiveBufferSize = stream->readInt32();
        this->noDelay = stream->readBoolean();
        this->timeout.read(stream);
        this->block = stream->readBoolean();
    }

    void TcpInfo::write(Stream *stream) const {
        this->address.write(stream);
        stream->writeInt32(this->port);
        stream->writeInt32(this->sendBufferSize);
        stream->writeInt32(this->receiveBufferSize);
        stream->writeBoolean(this->noDelay);
        this->timeout.write(stream);
        stream->writeBoolean(this->block);
    }

    void TcpInfo::read(XmlTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("sendBufferSize"), sendBufferSize);
        Int32::parse(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(reader.getAttribute("noDelay"), noDelay);
        Boolean::parse(reader.getAttribute("block"), block);
        Timeout::parse(reader.getAttribute("timeout"), timeout);
    }

    void TcpInfo::write(XmlTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("sendBufferSize", sendBufferSize);
        writer.writeAttribute("receiveBufferSize", receiveBufferSize);
        writer.writeAttribute("noDelay", noDelay);
        writer.writeAttribute("block", block);
        writer.writeAttribute("timeout", timeout.toString());
    }

    void TcpInfo::read(JsonTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("sendBufferSize"), sendBufferSize);
        Int32::parse(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(reader.getAttribute("noDelay"), noDelay);
        Boolean::parse(reader.getAttribute("block"), block);
        Timeout::parse(reader.getAttribute("timeout"), timeout);
    }

    void TcpInfo::write(JsonTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("sendBufferSize", sendBufferSize);
        writer.writeAttribute("receiveBufferSize", receiveBufferSize);
        writer.writeAttribute("noDelay", noDelay);
        writer.writeAttribute("block", block);
        writer.writeAttribute("timeout", timeout.toString());
    }

    void TcpInfo::write(JsonNode &node) const {
        node.add(JsonNode("address", address));
        node.add(JsonNode("port", port));
        node.add(JsonNode("sendBufferSize", sendBufferSize));
        node.add(JsonNode("receiveBufferSize", receiveBufferSize));
        node.add(JsonNode("noDelay", noDelay));
        node.add(JsonNode("block", block));
    }

    TcpServerInfo::TcpServerInfo() : TcpInfo() {
        maxConnections = 1024;
        receiverType = TcpChannelContext::Types::MultiPlexing;
        senderType = TcpChannelContext::Types::MultiPlexing;
        timeout.close = TimeSpan::fromSeconds(30);
    }

    String TcpServerInfo::toString() const {
        return String::convert("address: %s, port: %d, maxConnections: %d", address.c_str(), port, maxConnections);
    }

    TcpServerInfo &TcpServerInfo::operator=(const TcpServerInfo &value) {
        this->copyFrom(&value);
        return *this;
    }

    bool TcpServerInfo::operator==(const TcpServerInfo &value) const {
        return TcpInfo::operator==(value) &&
               this->maxConnections == value.maxConnections &&
               this->receiverType == value.receiverType &&
               this->senderType == value.senderType;
    }

    bool TcpServerInfo::operator!=(const TcpServerInfo &value) const {
        return !operator==(value);
    }

    void TcpServerInfo::copyFrom(const TcpInfo *value) {
        TcpInfo::copyFrom(value);

        auto info = dynamic_cast<const TcpServerInfo *>(value);
        assert(info);
        this->maxConnections = info->maxConnections;
        this->receiverType = info->receiverType;
        this->senderType = info->senderType;
    }

    bool TcpServerInfo::isEmpty() const {
        return this->port == 0;
    }

    void TcpServerInfo::read(Stream *stream) {
        TcpInfo::read(stream);

        this->maxConnections = stream->readInt32();
        this->receiverType = (TcpChannelContext::Types) stream->readInt32();
        this->senderType = (TcpChannelContext::Types) stream->readInt32();
    }

    void TcpServerInfo::write(Stream *stream) const {
        TcpInfo::write(stream);

        stream->writeInt32(this->maxConnections);
        stream->writeInt32(this->receiverType);
        stream->writeInt32(this->senderType);
    }

    void TcpServerInfo::read(XmlTextReader &reader) {
        Int32::parse(reader.getAttribute("maxConnections"), maxConnections);
        receiverType = TcpChannelContext::parseTypeStr(reader.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(reader.getAttribute("senderType"));

        TcpInfo::read(reader);
    }

    void TcpServerInfo::write(XmlTextWriter &writer) const {
        writer.writeAttribute("maxConnections", maxConnections);
        writer.writeAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        writer.writeAttribute("senderType", TcpChannelContext::toTypeStr(senderType));

        TcpInfo::write(writer);
    }

    void TcpServerInfo::read(JsonTextReader &reader) {
        Int32::parse(reader.getAttribute("maxConnections"), maxConnections);
        receiverType = TcpChannelContext::parseTypeStr(reader.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(reader.getAttribute("senderType"));

        TcpInfo::read(reader);
    }

    void TcpServerInfo::write(JsonTextWriter &writer) const {
        writer.writeAttribute("maxConnections", maxConnections);
        writer.writeAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        writer.writeAttribute("senderType", TcpChannelContext::toTypeStr(senderType));

        TcpInfo::write(writer);
    }

    void TcpServerInfo::write(JsonNode &node) const {
        node.add(JsonNode("maxConnections", maxConnections));
        node.add(JsonNode("receiverType", TcpChannelContext::toTypeStr(receiverType)));
        node.add(JsonNode("senderType", TcpChannelContext::toTypeStr(senderType)));

        TcpInfo::write(node);
    }

    UdpInfo::UdpInfo() {
        address = String::Empty;
        port = 0;
        sendBufferSize = 0; // default size
    }

    String UdpInfo::toString() const {
        return String::convert("address: %s, port: %d", address.c_str(), port);
    }

    UdpInfo &UdpInfo::operator=(const UdpInfo &value) {
        this->copyFrom(&value);
        return *this;
    }

    bool UdpInfo::operator==(const UdpInfo &value) const {
        return this->address == value.address &&
               this->port == value.port &&
               this->sendBufferSize == value.sendBufferSize;
    }

    bool UdpInfo::operator!=(const UdpInfo &value) const {
        return !operator==(value);
    }

    void UdpInfo::copyFrom(const UdpInfo *value) {
        this->address = value->address;
        this->port = value->port;
        this->sendBufferSize = value->sendBufferSize;
    }

    bool UdpInfo::isEmpty() const {
        return this->port == 0;
    }

    void UdpInfo::read(Stream *stream) {
        this->address.read(stream);
        this->port = stream->readInt32();
        this->sendBufferSize = stream->readInt32();
    }

    void UdpInfo::write(Stream *stream) const {
        this->address.write(stream);
        stream->writeInt32(this->port);
        stream->writeInt32(this->sendBufferSize);
    }

    void UdpInfo::read(XmlTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("sendBufferSize"), sendBufferSize);
    }

    void UdpInfo::write(XmlTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("sendBufferSize", sendBufferSize);
    }

    void UdpInfo::read(JsonTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("sendBufferSize"), sendBufferSize);
    }

    void UdpInfo::write(JsonTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("sendBufferSize", sendBufferSize);
    }

    void UdpInfo::write(JsonNode &node) const {
        node.add(JsonNode("address", address));
        node.add(JsonNode("port", port));
        node.add(JsonNode("sendBufferSize", sendBufferSize));
    }

    UdpServerInfo::UdpServerInfo() {
        address = String::Empty;
        port = 0;
        receiveBufferSize = 0; // default size
    }

    String UdpServerInfo::toString() const {
        return String::convert("address: %s, port: %d", address.c_str(), port);
    }

    UdpServerInfo &UdpServerInfo::operator=(const UdpServerInfo &value) {
        this->copyFrom(&value);
        return *this;
    }

    bool UdpServerInfo::operator==(const UdpServerInfo &value) const {
        return this->address == value.address &&
               this->port == value.port &&
               this->receiveBufferSize == value.receiveBufferSize;
    }

    bool UdpServerInfo::operator!=(const UdpServerInfo &value) const {
        return !operator==(value);
    }

    bool UdpServerInfo::isEmpty() const {
        return this->port == 0;
    }

    void UdpServerInfo::copyFrom(const UdpServerInfo *value) {
        this->address = value->address;
        this->port = value->port;
        this->receiveBufferSize = value->receiveBufferSize;
    }

    void UdpServerInfo::read(Stream *stream) {
        this->address.read(stream);
        this->port = stream->readInt32();
        this->receiveBufferSize = stream->readInt32();
    }

    void UdpServerInfo::write(Stream *stream) const {
        this->address.write(stream);
        stream->writeInt32(this->port);
        stream->writeInt32(this->receiveBufferSize);
    }

    void UdpServerInfo::read(XmlTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
    }

    void UdpServerInfo::write(XmlTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("receiveBufferSize", receiveBufferSize);
    }

    void UdpServerInfo::read(JsonTextReader &reader) {
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
    }

    void UdpServerInfo::write(JsonTextWriter &writer) const {
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("receiveBufferSize", receiveBufferSize);
    }

    void UdpServerInfo::write(JsonNode &node) const {
        node.add(JsonNode("address", address));
        node.add(JsonNode("port", port));
        node.add(JsonNode("receiveBufferSize", receiveBufferSize));
    }
}
