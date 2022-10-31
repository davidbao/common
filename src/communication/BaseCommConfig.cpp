#include "data/TimeSpan.h"
#include "system/Convert.h"
#include "IO/Directory.h"
#include "IO/File.h"
#include "communication/BaseCommConfig.h"

using namespace Common;

namespace Communication
{
    Broadcast::Broadcast() : interval(0, 0, 1), count(10), sendport(10041), receiveport(10042), enabled(false)
    {
    }
    Broadcast::Broadcast(const Broadcast& value) : Broadcast()
    {
        this->operator=(value);
    }
    void Broadcast::operator=(const Broadcast& value)
    {
        this->interval = value.interval;
        this->count = value.count;
        this->sendport = value.sendport;
        this->receiveport = value.receiveport;
        this->enabled = value.enabled;
    }
    bool Broadcast::operator==(const Broadcast& value) const
    {
        return this->interval == value.interval &&
            this->count == value.count &&
            this->sendport == value.sendport &&
            this->receiveport == value.receiveport &&
            this->enabled == value.enabled;
    }
    bool Broadcast::operator!=(const Broadcast& value) const
    {
        return !operator==(value);
    }
    void Broadcast::read(XmlTextReader& reader)
    {
        TimeSpan::parse(reader.getAttribute("interval"), interval);
        Int32::parse(reader.getAttribute("count"), count);
        Int32::parse(reader.getAttribute("sendport"), sendport);
        Int32::parse(reader.getAttribute("receiveport"), receiveport);
        Boolean::parse(reader.getAttribute("enabled"), enabled);
    }
    void Broadcast::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("interval", interval);
        writer.writeAttribute("count", count);
        writer.writeAttribute("sendport", sendport);
        writer.writeAttribute("receiveport", receiveport);
        writer.writeAttribute("enabled", enabled);
    }
    void Broadcast::read(XmlNode& node)
    {
        TimeSpan::parse(node.getAttribute("interval"), interval);
        Int32::parse(node.getAttribute("count"), count);
        Int32::parse(node.getAttribute("sendport"), sendport);
        Int32::parse(node.getAttribute("receiveport"), receiveport);
        Boolean::parse(node.getAttribute("enabled"), enabled);
    }
    void Broadcast::write(XmlNode& node) const
    {
        node.updateAttribute("interval", interval);
        node.updateAttribute("count", count);
        node.updateAttribute("sendport", sendport);
        node.updateAttribute("receiveport", receiveport);
        node.updateAttribute("enabled", enabled);
    }
    
    const Secure Secure::None(false);
    Secure::Secure(bool enabled) : enabled(enabled)
    {
    }
    Secure::Secure(const Secure& value)
    {
        this->operator=(value);
    }
    void Secure::operator=(const Secure& value)
    {
        this->certFile = value.certFile;
        this->keyFile = value.keyFile;
        this->cacertFile = value.cacertFile;
        this->enabled = value.enabled;
    }
    bool Secure::operator==(const Secure& value) const
    {
        return this->certFile == value.certFile &&
        this->keyFile == value.keyFile &&
        this->cacertFile == value.cacertFile &&
        this->enabled == value.enabled;
    }
    bool Secure::operator!=(const Secure& value) const
    {
        return !operator==(value);
    }
    void Secure::read(XmlTextReader& reader)
    {
        reader.getAttribute("certFile", certFile);
        reader.getAttribute("keyFile", keyFile);
        reader.getAttribute("cacertFile", cacertFile);
        reader.getAttribute("enabled", enabled);
    }
    void Secure::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("certFile", certFile);
        writer.writeAttribute("keyFile", keyFile);
        writer.writeAttribute("cacertFile", cacertFile);
        writer.writeAttribute("enabled", enabled);
    }
    void Secure::read(XmlNode& node)
    {
        node.getAttribute("certFile", certFile);
        node.getAttribute("keyFile", keyFile);
        node.getAttribute("cacertFile", cacertFile);
        node.getAttribute("enabled", enabled);
    }
    void Secure::write(XmlNode& node) const
    {
        node.updateAttribute("certFile", certFile);
        node.updateAttribute("keyFile", keyFile);
        node.updateAttribute("cacertFile", cacertFile);
        node.updateAttribute("enabled", enabled);
    }
    
    Client::Connection::Connection() : detectionInterval(0, 0, 3), resumeInterval(0, 0, 10), detectionCount(3), showError(false)
    {
    }
    Client::Connection::Connection(const Client::Connection& value) : Connection()
    {
        this->operator=(value);
    }
    void Client::Connection::operator=(const Connection& value)
    {
        this->detectionInterval = value.detectionInterval;
        this->resumeInterval = value.resumeInterval;
        this->detectionCount = value.detectionCount;
        this->showError = value.showError;
    }
    bool Client::Connection::operator==(const Connection& value) const
    {
        return this->detectionInterval == value.detectionInterval &&
            this->resumeInterval == value.resumeInterval &&
            this->detectionCount == value.detectionCount &&
            this->showError == value.showError;
    }
    bool Client::Connection::operator!=(const Connection& value) const
    {
        return !operator==(value);
    }
    void Client::Connection::read(XmlTextReader& reader)
    {
        TimeSpan::parse(reader.getAttribute("detectioninterval"), detectionInterval);
        TimeSpan::parse(reader.getAttribute("resumeinterval"), resumeInterval);
        Int32::parse(reader.getAttribute("detectioncount"), detectionCount);
        Boolean::parse(reader.getAttribute("showError"), showError);
    }
    void Client::Connection::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("detectioninterval", detectionInterval);
        writer.writeAttribute("resumeinterval", resumeInterval);
        writer.writeAttribute("detectioncount", detectionCount);
        writer.writeAttribute("showError", showError);
    }
    void Client::Connection::read(XmlNode& node)
    {
        TimeSpan::parse(node.getAttribute("detectioninterval"), detectionInterval);
        TimeSpan::parse(node.getAttribute("resumeinterval"), resumeInterval);
        Int32::parse(node.getAttribute("detectioncount"), detectionCount);
        Boolean::parse(node.getAttribute("showError"), showError);
    }
    void Client::Connection::write(XmlNode& node) const
    {
        node.updateAttribute("detectioninterval", detectionInterval);
        node.updateAttribute("resumeinterval", resumeInterval);
        node.updateAttribute("detectioncount", detectionCount);
        node.updateAttribute("showError", showError);
    }
    
    Client::Timeout::Timeout() : send(0, 0, 3), receive(0, 0, 3), open(0, 0, 3)
    {
    }
    Client::Timeout::Timeout(const Timeout& value)
    {
        this->operator=(value);
    }
    void Client::Timeout::operator=(const Timeout& value)
    {
        this->send = value.send;
        this->receive = value.receive;
        this->open = value.open;
    }
    bool Client::Timeout::operator==(const Timeout& value) const
    {
        return this->send == value.send &&
            this->receive == value.receive &&
            this->open == value.open;
    }
    bool Client::Timeout::operator!=(const Timeout& value) const
    {
        return !operator==(value);
    }
    void Client::Timeout::read(XmlTextReader& reader)
    {
        TimeSpan::parse(reader.getAttribute("open"), open);
        TimeSpan::parse(reader.getAttribute("receive"), receive);
        TimeSpan::parse(reader.getAttribute("send"), send);
    }
    void Client::Timeout::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("open", open);
        writer.writeAttribute("receive", receive);
        writer.writeAttribute("send", send);
    }
    void Client::Timeout::read(XmlNode& node)
    {
        TimeSpan::parse(node.getAttribute("open"), open);
        TimeSpan::parse(node.getAttribute("receive"), receive);
        TimeSpan::parse(node.getAttribute("send"), send);
    }
    void Client::Timeout::write(XmlNode& node) const
    {
        node.updateAttribute("open", open);
        node.updateAttribute("receive", receive);
        node.updateAttribute("send", send);
    }
    
    const Client Client::Empty = Client();
    Client::Client(bool enabled)
    {
        this->enabled = enabled;
        this->name = String::Empty;
        this->address = String::Empty;
        this->port = 0;
        this->packetLength = 64 * 1024;
        this->sendBufferSize = 512 * 1024;
        this->receiveBufferSize = 512 * 1024;
        this->noDelay = true;
        this->reuseAddress = false;
        this->blocking = false;
        this->receiverType = TcpChannelContext::Types::MultiPlexing;
        this->senderType = TcpChannelContext::Types::MultiPlexing;
    }
    Client::Client(const String& name, const String& address, int port) : Client(true)
    {
        this->name = name;
        this->address = address;
        this->port = port;
    }
    Client::Client(const Client& value) : Client()
    {
        this->operator=(value);
    }
    bool Client::isEmpty() const
    {
        return name.isNullOrEmpty() || !enabled;
    }
    
    void Client::copyFrom(const Client* value)
    {
        this->operator=(*value);
    }
    
    void Client::operator=(const Client& value)
    {
        this->name = value.name;
        this->address = value.address;
        this->broadcast = value.broadcast;
        this->port = value.port;
        this->connection = value.connection;
        this->timeout = value.timeout;
        this->secure = value.secure;
        this->packetLength = value.packetLength;
        this->sendBufferSize = value.sendBufferSize;
        this->receiveBufferSize = value.receiveBufferSize;
        this->noDelay = value.noDelay;
        this->reuseAddress = value.reuseAddress;
        this->blocking = value.blocking;
        this->receiverType = value.receiverType;
        this->senderType = value.senderType;
        this->enabled = value.enabled;
    }
    bool Client::operator==(const Client& value) const
    {
        return this->name == value.name &&
            this->address == value.address &&
            this->broadcast == value.broadcast &&
            this->port == value.port &&
            this->connection == value.connection &&
            this->timeout == value.timeout &&
            this->secure == value.secure &&
            this->packetLength == value.packetLength &&
            this->sendBufferSize == value.sendBufferSize &&
            this->receiveBufferSize == value.receiveBufferSize &&
            this->noDelay == value.noDelay &&
            this->reuseAddress == value.reuseAddress &&
            this->blocking == value.blocking &&
            this->receiverType == value.receiverType &&
            this->senderType == value.senderType &&
            this->enabled == value.enabled;
    }
    bool Client::operator!=(const Client& value) const
    {
        return !operator==(value);
    }
    Endpoint Client::endpoint() const
    {
        return Endpoint(address, port);
    }
    void Client::read(XmlTextReader& reader)
    {
        name = reader.getAttribute("name");
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        BaseCommConfig::parseBufferLength(reader.getAttribute("packetlength"), packetLength);
        BaseCommConfig::parseBufferLength(reader.getAttribute("sendBufferSize"), sendBufferSize);
        BaseCommConfig::parseBufferLength(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(reader.getAttribute("noDelay"), noDelay);
        Boolean::parse(reader.getAttribute("reuseAddress"), reuseAddress);
        Boolean::parse(reader.getAttribute("blocking"), blocking);
        receiverType = TcpChannelContext::parseTypeStr(reader.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(reader.getAttribute("senderType"));
        Boolean::parse(reader.getAttribute("enabled"), enabled);
        while (reader.read() &&
               !(reader.nodeType() == XmlNodeType::EndElement &&
                 reader.localName() == "client"))
        {
            if (reader.nodeType() == XmlNodeType::Element &&
                reader.localName() == "broadcast")
            {
                broadcast.read(reader);
            }
            else if (reader.nodeType() == XmlNodeType::Element &&
                     reader.localName() == "connection")
            {
                connection.read(reader);
            }
            else if (reader.nodeType() == XmlNodeType::Element &&
                     reader.localName() == "timeout")
            {
                timeout.read(reader);
            }
            else if (reader.nodeType() == XmlNodeType::Element &&
                     reader.localName() == "secure")
            {
                secure.read(reader);
            }
        }
    }
    void Client::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("name", name);
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("packetlength", BaseCommConfig::toBufferLengthStr(packetLength));
        writer.writeAttribute("sendBufferSize", BaseCommConfig::toBufferLengthStr(sendBufferSize));
        writer.writeAttribute("receiveBufferSize", BaseCommConfig::toBufferLengthStr(receiveBufferSize));
        writer.writeAttribute("noDelay", noDelay);
        writer.writeAttribute("reuseAddress", reuseAddress);
        writer.writeAttribute("blocking", blocking);
        writer.writeAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        writer.writeAttribute("senderType", TcpChannelContext::toTypeStr(senderType));
        writer.writeAttribute("enabled", enabled);
        
        writer.writeStartElement("broadcast");
        broadcast.write(writer);
        writer.writeEndElement();
        
        writer.writeStartElement("connection");
        connection.write(writer);
        writer.writeEndElement();
        
        writer.writeStartElement("timeout");
        timeout.write(writer);
        writer.writeEndElement();
        
        writer.writeStartElement("secure");
        secure.write(writer);
        writer.writeEndElement();
    }
    void Client::read(XmlNode& node)
    {
        name = node.getAttribute("name");
        address = node.getAttribute("address");
        Int32::parse(node.getAttribute("port"), port);
        BaseCommConfig::parseBufferLength(node.getAttribute("packetlength"), packetLength);
        BaseCommConfig::parseBufferLength(node.getAttribute("sendBufferSize"), sendBufferSize);
        BaseCommConfig::parseBufferLength(node.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(node.getAttribute("noDelay"), noDelay);
        Boolean::parse(node.getAttribute("reuseAddress"), reuseAddress);
        Boolean::parse(node.getAttribute("blocking"), blocking);
        receiverType = TcpChannelContext::parseTypeStr(node.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(node.getAttribute("senderType"));
        Boolean::parse(node.getAttribute("enabled"), enabled);
        
        XmlNode broadcastNode;
        if(node.getSubNode("broadcast", broadcastNode))
        {
            broadcast.read(broadcastNode);
        }
        XmlNode connectionNode;
        if(node.getSubNode("connection", connectionNode))
        {
            connection.read(connectionNode);
        }
        XmlNode timeoutNode;
        if(node.getSubNode("timeout", timeoutNode))
        {
            timeout.read(timeoutNode);
        }
        XmlNode secureNode;
        if(node.getSubNode("secure", secureNode))
        {
            secure.read(secureNode);
        }
    }
    void Client::write(XmlNode& node) const
    {
        node.updateAttribute("name", name);
        node.updateAttribute("address", address);
        node.updateAttribute("port", port);
        node.updateAttribute("packetlength", BaseCommConfig::toBufferLengthStr(packetLength));
        node.updateAttribute("sendBufferSize", BaseCommConfig::toBufferLengthStr(sendBufferSize));
        node.updateAttribute("receiveBufferSize", BaseCommConfig::toBufferLengthStr(receiveBufferSize));
        node.updateAttribute("noDelay", noDelay);
        node.updateAttribute("reuseAddress", reuseAddress);
        node.updateAttribute("blocking", blocking);
        node.updateAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        node.updateAttribute("senderType", TcpChannelContext::toTypeStr(senderType));
        node.updateAttribute("enabled", enabled);
        
        XmlNode broadcastNode;
        if(node.getOrNewSubNode("broadcast", broadcastNode))
        {
            broadcast.write(broadcastNode);
        }
        
        XmlNode connectionNode;
        if(node.getOrNewSubNode("connection", connectionNode))
        {
            connection.write(connectionNode);
        }
        
        XmlNode timeoutNode;
        if(node.getOrNewSubNode("timeout", timeoutNode))
        {
            timeout.write(timeoutNode);
        }
        
        XmlNode secureNode;
        if(node.getOrNewSubNode("secure", secureNode))
        {
            secure.write(secureNode);
        }
    }
    void Client::updateConfig(const StringMap& properties)
    {
        String str;
        const String prefix = "client";
        properties.at(String::format("%s.enabled", prefix.c_str()), str);
        Boolean::parse(str, enabled);
        properties.at(String::format("%s.host", prefix.c_str()), address);
        properties.at(String::format("%s.port", prefix.c_str()), str);
        Port::parse(str, port);
        properties.at(String::format("%s.sendBufferSize", prefix.c_str()), str);
        BaseCommConfig::parseBufferLength(str, sendBufferSize);
        properties.at(String::format("%s.receiveBufferSize", prefix.c_str()), str);
        BaseCommConfig::parseBufferLength(str, receiveBufferSize);
        
        properties.at(String::format("%s.packetLength", prefix.c_str()), str);
        UInt32::parse(str, packetLength);
        properties.at(String::format("%s.receiverType", prefix.c_str()), str);
        receiverType = TcpChannelContext::parseTypeStr(str);
        properties.at(String::format("%s.senderType", prefix.c_str()), str);
        senderType = TcpChannelContext::parseTypeStr(str);
        
        properties.at(String::format("%s.timeout.open", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.open);
        properties.at(String::format("%s.timeout.send", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.send);
        properties.at(String::format("%s.timeout.receive", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.receive);
        
        properties.at(String::format("%s.secure.enabled", prefix.c_str()), str);
        Boolean::parse(str, secure.enabled);
        properties.at(String::format("%s.secure.certFile", prefix.c_str()), secure.certFile);
        properties.at(String::format("%s.secure.keyFile", prefix.c_str()), secure.keyFile);
        properties.at(String::format("%s.secure.cacertFile", prefix.c_str()), secure.cacertFile);
        
        properties.at(String::format("%s.connection.detectionInterval", prefix.c_str()), str);
        TimeSpan::parse(str, connection.detectionInterval);
        properties.at(String::format("%s.connection.resumeInterval", prefix.c_str()), str);
        TimeSpan::parse(str, connection.resumeInterval);
        properties.at(String::format("%s.connection.detectionCount", prefix.c_str()), str);
        Int32::parse(str, connection.detectionCount);
        properties.at(String::format("%s.connection.showError", prefix.c_str()), str);
        Boolean::parse(str, connection.showError);
    }
    bool Client::asyncReceiver() const
    {
        return receiverType == TcpChannelContext::Types::Async;
    }
    bool Client::syncReceiver() const
    {
        return receiverType == TcpChannelContext::Types::Sync;
    }
    bool Client::multiPlexingReceiver() const
    {
        return receiverType == TcpChannelContext::Types::MultiPlexing;
    }

    bool Client::asyncSender() const
    {
        return senderType == TcpChannelContext::Types::Async;
    }
    bool Client::syncSender() const
    {
        return senderType == TcpChannelContext::Types::Sync;
    }
    bool Client::multiPlexingSender() const
    {
        return senderType == TcpChannelContext::Types::MultiPlexing;
    }
    
    Server::Timeout::Timeout() : send(0, 0, 3), receive(0, 0, 3), close(0, 0, 10)
    {
    }
    Server::Timeout::Timeout(const Timeout& value)
    {
        this->operator=(value);
    }    
    void Server::Timeout::operator=(const Timeout& value)
    {
        this->send = value.send;
        this->receive = value.receive;
        this->close = value.close;
    }
    bool Server::Timeout::operator==(const Timeout& value) const
    {
        return this->send == value.send &&
            this->receive == value.receive &&
            this->close == value.close;
    }
    bool Server::Timeout::operator!=(const Timeout& value) const
    {
        return !operator==(value);
    }
    void Server::Timeout::read(XmlTextReader& reader)
    {
        TimeSpan::parse(reader.getAttribute("close"), close);
        TimeSpan::parse(reader.getAttribute("receive"), receive);
        TimeSpan::parse(reader.getAttribute("send"), send);
    }
    void Server::Timeout::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("close", close);
        writer.writeAttribute("receive", receive);
        writer.writeAttribute("send", send);
    }
    void Server::Timeout::read(XmlNode& node)
    {
        TimeSpan::parse(node.getAttribute("close"), close);
        TimeSpan::parse(node.getAttribute("receive"), receive);
        TimeSpan::parse(node.getAttribute("send"), send);
    }
    void Server::Timeout::write(XmlNode& node) const
    {
        node.updateAttribute("close", close);
        node.updateAttribute("receive", receive);
        node.updateAttribute("send", send);
    }
    
    const Server Server::Empty = Server();
    Server::Server(bool enabled)
    {
        this->enabled = enabled;
        this->name = String::Empty;
        this->address = "any";
        this->port = 0;
        this->maxConnections = 5000;
        this->sendBufferSize = 512 * 1024;
        this->receiveBufferSize = 512 * 1024;
        this->noDelay = true;
        this->reuseAddress = false;
        this->receiverType = TcpChannelContext::Types::MultiPlexing;
        this->senderType = TcpChannelContext::Types::MultiPlexing;
    }
    Server::Server(const String& name, const String& address, int port) : Server(true)
    {
        this->name = name;
        this->address = address;
        this->port = port;
    }
    Server::Server(const Server& value) : Server()
    {
        this->operator=(value);
    }
    bool Server::isEmpty() const
    {
        return name.isNullOrEmpty() || !enabled;
    }
    void Server::copyFrom(const Server* value)
    {
        this->operator=(*value);
    }
    void Server::operator=(const Server& value)
    {
        this->name = value.name;
        this->address = value.address;
        this->broadcast = value.broadcast;
        this->port = value.port;
        this->maxConnections = value.maxConnections;
        this->timeout = value.timeout;
        this->secure = value.secure;
        this->sendBufferSize = value.sendBufferSize;
        this->receiveBufferSize = value.receiveBufferSize;
        this->noDelay = value.noDelay;
        this->reuseAddress = value.reuseAddress;
        this->enabled = value.enabled;
        this->receiverType = value.receiverType;
        this->senderType = value.senderType;
    }
    bool Server::operator==(const Server& value) const
    {
        return this->name == value.name &&
            this->address == value.address &&
            this->broadcast == value.broadcast &&
            this->port == value.port &&
            this->maxConnections == value.maxConnections &&
            this->timeout == value.timeout &&
            this->secure == value.secure &&
            this->sendBufferSize == value.sendBufferSize &&
            this->receiveBufferSize == value.receiveBufferSize &&
            this->noDelay == value.noDelay &&
            this->reuseAddress == value.reuseAddress &&
            this->enabled == value.enabled &&
            this->receiverType == value.receiverType &&
            this->senderType == value.senderType;
    }
    bool Server::operator!=(const Server& value) const
    {
        return !operator==(value);
    }
    Endpoint Server::endpoint() const
    {
        return Endpoint(address, port);
    }
    void Server::read(XmlTextReader& reader)
    {
        name = reader.getAttribute("name");
        address = reader.getAttribute("address");
        Int32::parse(reader.getAttribute("port"), port);
        Int32::parse(reader.getAttribute("maxConnections"), maxConnections);
        BaseCommConfig::parseBufferLength(reader.getAttribute("sendBufferSize"), sendBufferSize);
        BaseCommConfig::parseBufferLength(reader.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(reader.getAttribute("noDelay"), noDelay);
        Boolean::parse(reader.getAttribute("reuseAddress"), reuseAddress);
        receiverType = TcpChannelContext::parseTypeStr(reader.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(reader.getAttribute("senderType"));
        Boolean::parse(reader.getAttribute("enabled"), enabled);
        while (reader.read() &&
               !(reader.nodeType() == XmlNodeType::EndElement &&
                 reader.localName() == "server"))
        {
            if (reader.nodeType() == XmlNodeType::Element &&
                reader.localName() == "broadcast")
            {
                broadcast.read(reader);
            }
            else if (reader.nodeType() == XmlNodeType::Element &&
                     reader.localName() == "timeout")
            {
                timeout.read(reader);
            }
            else if (reader.nodeType() == XmlNodeType::Element &&
                     reader.localName() == "secure")
            {
                secure.read(reader);
            }
        }
    }
    void Server::write(XmlTextWriter& writer) const
    {
        writer.writeAttribute("name", name);
        writer.writeAttribute("address", address);
        writer.writeAttribute("port", port);
        writer.writeAttribute("maxConnections", maxConnections);
        writer.writeAttribute("sendBufferSize", BaseCommConfig::toBufferLengthStr(sendBufferSize));
        writer.writeAttribute("receiveBufferSize", BaseCommConfig::toBufferLengthStr(receiveBufferSize));
        writer.writeAttribute("noDelay", noDelay);
        writer.writeAttribute("reuseAddress", reuseAddress);
        writer.writeAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        writer.writeAttribute("senderType", TcpChannelContext::toTypeStr(senderType));
        writer.writeAttribute("enabled", enabled);
        
        writer.writeStartElement("broadcast");
        broadcast.write(writer);
        writer.writeEndElement();
        
        writer.writeStartElement("timeout");
        timeout.write(writer);
        writer.writeEndElement();
        
        writer.writeStartElement("secure");
        secure.write(writer);
        writer.writeEndElement();
    }
    void Server::read(XmlNode& node)
    {
        name = node.getAttribute("name");
        address = node.getAttribute("address");
        Int32::parse(node.getAttribute("port"), port);
        Int32::parse(node.getAttribute("maxConnections"), maxConnections);
        BaseCommConfig::parseBufferLength(node.getAttribute("sendBufferSize"), sendBufferSize);
        BaseCommConfig::parseBufferLength(node.getAttribute("receiveBufferSize"), receiveBufferSize);
        Boolean::parse(node.getAttribute("noDelay"), noDelay);
        Boolean::parse(node.getAttribute("reuseAddress"), reuseAddress);
        receiverType = TcpChannelContext::parseTypeStr(node.getAttribute("receiverType"));
        senderType = TcpChannelContext::parseTypeStr(node.getAttribute("senderType"));
        Boolean::parse(node.getAttribute("enabled"), enabled);
        
        XmlNode broadcastNode;
        if(node.getSubNode("broadcast", broadcastNode))
        {
            broadcast.read(broadcastNode);
        }
        XmlNode timeoutNode;
        if(node.getSubNode("timeout", timeoutNode))
        {
            timeout.read(timeoutNode);
        }
        XmlNode secureNode;
        if(node.getSubNode("secure", secureNode))
        {
            secure.read(secureNode);
        }
    }
    void Server::write(XmlNode& node) const
    {
        node.updateAttribute("name", name);
        node.updateAttribute("address", address);
        node.updateAttribute("port", port);
        node.updateAttribute("maxConnections", maxConnections);
        node.updateAttribute("sendBufferSize", BaseCommConfig::toBufferLengthStr(sendBufferSize));
        node.updateAttribute("receiveBufferSize", BaseCommConfig::toBufferLengthStr(receiveBufferSize));
        node.updateAttribute("noDelay", noDelay);
        node.updateAttribute("reuseAddress", reuseAddress);
        node.updateAttribute("receiverType", TcpChannelContext::toTypeStr(receiverType));
        node.updateAttribute("senderType", TcpChannelContext::toTypeStr(senderType));
        node.updateAttribute("enabled", enabled);
        
        XmlNode broadcastNode;
        if(node.getOrNewSubNode("broadcast", broadcastNode))
        {
            broadcast.write(broadcastNode);
        }
        
        XmlNode timeoutNode;
        if(node.getOrNewSubNode("timeout", timeoutNode))
        {
            timeout.write(timeoutNode);
        }
        
        XmlNode secureNode;
        if(node.getOrNewSubNode("secure", secureNode))
        {
            secure.write(secureNode);
        }
    }
    void Server::updateConfig(const StringMap& properties, const String& prefix)
    {
        String str;
        properties.at(String::format("%s.scheme", prefix.c_str()), name);
        if(properties.at(String::format("%s.enabled", prefix.c_str()), str))
            Boolean::parse(str, enabled);
        else
            enabled = true;
        properties.at(String::format("%s.host", prefix.c_str()), address);
        properties.at(String::format("%s.port", prefix.c_str()), str);
        Port::parse(str, port);
        properties.at(String::format("%s.maxConnections", prefix.c_str()), str);
        Int32::parse(str, maxConnections);
        properties.at(String::format("%s.sendBufferSize", prefix.c_str()), str);
        BaseCommConfig::parseBufferLength(str, sendBufferSize);
        properties.at(String::format("%s.receiveBufferSize", prefix.c_str()), str);
        BaseCommConfig::parseBufferLength(str, receiveBufferSize);
        properties.at(String::format("%s.receiverType", prefix.c_str()), str);
        receiverType = TcpChannelContext::parseTypeStr(str);
        properties.at(String::format("%s.senderType", prefix.c_str()), str);
        senderType = TcpChannelContext::parseTypeStr(str);
        properties.at(String::format("%s.timeout.close", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.close);
        properties.at(String::format("%s.timeout.send", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.send);
        properties.at(String::format("%s.timeout.receive", prefix.c_str()), str);
        TimeSpan::parse(str, timeout.receive);
        properties.at(String::format("%s.secure.enabled", prefix.c_str()), str);
        Boolean::parse(str, secure.enabled);
        properties.at(String::format("%s.secure.certFile", prefix.c_str()), secure.certFile);
        properties.at(String::format("%s.secure.keyFile", prefix.c_str()), secure.keyFile);
        properties.at(String::format("%s.secure.cacertFile", prefix.c_str()), secure.cacertFile);
    }
    bool Server::asyncReceiver() const
    {
        return receiverType == TcpChannelContext::Types::Async;
    }
    bool Server::syncReceiver() const
    {
        return receiverType == TcpChannelContext::Types::Sync;
    }
    bool Server::multiPlexingReceiver() const
    {
        return receiverType == TcpChannelContext::Types::MultiPlexing;
    }

    bool Server::asyncSender() const
    {
        return senderType == TcpChannelContext::Types::Async;
    }
    bool Server::syncSender() const
    {
        return senderType == TcpChannelContext::Types::Sync;
    }
    bool Server::multiPlexingSender() const
    {
        return senderType == TcpChannelContext::Types::MultiPlexing;
    }
    
    BaseCommConfig::BaseCommConfig(const ConfigFile& file) : Configuration(file)
    {
    }
    
    BaseCommConfig::~BaseCommConfig()
    {
    }
    const Server& BaseCommConfig::server() const
    {
        return _servers.count() > 0 ? *_servers[0] : Server::Empty;
    }
    const Client& BaseCommConfig::client() const
    {
        return _clients.count() > 0 ? *_clients[0] : Client::Empty;
    }
    const Client& BaseCommConfig::client(const String& name, bool useSSL) const
    {
        String tcpName = String::convert("%s_tcp", name.c_str());
        String sslName = String::convert("%s_ssl", name.c_str());
        String ssl2Name = String::convert("%ss", name.c_str());
        if(useSSL)
        {
            // ssl first.
            for (uint i=0; i<_clients.count(); i++)
            {
                Client* client = _clients[i];
                if(client->enabled && (client->name == sslName || client->name == ssl2Name))
                {
                    client->secure.enabled = true;
                    return *client;
                }
            }
        }
        for (uint i=0; i<_clients.count(); i++)
        {
            Client* client = _clients[i];
            if(client->enabled && (client->name == name || client->name == tcpName))
            {
                client->secure.enabled = false;
                return *client;
            }
        }
        return Client::Empty;
    }
    const Server& BaseCommConfig::server(const String& name) const
    {
        for (uint i=0; i<_servers.count(); i++)
        {
            const Server* server = _servers[i];
            if(server->name == name)
                return *server;
        }
        return Server::Empty;
    }
    void BaseCommConfig::loadServerNodes(XmlTextReader& reader)
    {
        Server server;
        if(ServerConfig::loadServerNode(reader, server))
        {
            Server* temp = new Server();
            temp->copyFrom(&server);
            _servers.add(temp);
        }
    }
    void BaseCommConfig::loadClientNodes(XmlTextReader& reader)
    {
        Client client;
        if(ClientConfig::loadClientNode(reader, client))
        {
            Client* temp = new Client();
            temp->copyFrom(&client);
            _clients.add(temp);
        }
    }
    bool BaseCommConfig::parseBufferLength(const String& str, uint32_t& value)
    {
        if (!str.isNullOrEmpty())
        {
            uint32_t length = 0;
            size_t numberLength = str.length() - 1;
            const char unit = str[numberLength];
            if (unit == 'k' || unit == 'K')
            {
                if(!UInt32::parse(String::substr(str, 0, numberLength), length))
                    return false;
                length *= 1024;
            }
            else if (unit == 'm' || unit == 'M')
            {
                if(!UInt32::parse(String::substr(str, 0, numberLength), length))
                    return false;
                length *= 1024 * 1024;
            }
            else
            {
                if(!UInt32::parse(str, length))
                    return false;
                length *= 1024;
            }
            value = length;
            return true;
        }
        return false;
    }
    const String BaseCommConfig::toBufferLengthStr(uint32_t value)
    {
        if(value > 0 && (value % 1024) == 0)
        {
            if(value >= 1024 && value < 1024 * 1024)
            {
                return String::convert("%dK", value / 1024);
            }
            else
            {
                return String::convert("%dM", value / 1024 / 1024);
            }
        }
        else
        {
            return UInt32(value).toString();
        }
    }
    
    ServerConfig::ServerConfig(const ConfigFile& file) : Configuration(file)
    {
    }
    ServerConfig::~ServerConfig()
    {
    }
    void ServerConfig::loadServerNode(XmlTextReader& reader)
    {
        loadServerNode(reader, _server);
    }
    bool ServerConfig::loadServerNode(XmlTextReader& reader, Server& server, const String& name)
    {
        if (reader.nodeType() == XmlNodeType::Element &&
            reader.localName() == name)
        {
            server.name = reader.getAttribute("name");
            server.address = reader.getAttribute("address");
            Int32::parse(reader.getAttribute("port"), server.port);
            Int32::parse(reader.getAttribute("maxConnections"), server.maxConnections);
            BaseCommConfig::parseBufferLength(reader.getAttribute("sendBufferSize"), server.sendBufferSize);
            BaseCommConfig::parseBufferLength(reader.getAttribute("receiveBufferSize"), server.receiveBufferSize);
            Boolean::parse(reader.getAttribute("noDelay"), server.noDelay);
            Boolean::parse(reader.getAttribute("reuseAddress"), server.reuseAddress);
            server.receiverType = TcpChannelContext::parseTypeStr(reader.getAttribute("receiverType"));
            server.senderType = TcpChannelContext::parseTypeStr(reader.getAttribute("senderType"));
            Boolean::parse(reader.getAttribute("enabled"), server.enabled);
            while (reader.read() &&
                   !(reader.nodeType() == XmlNodeType::EndElement &&
                     reader.localName() == "server"))
            {
                if (reader.nodeType() == XmlNodeType::Element &&
                    reader.localName() == "broadcast")
                {
                    server.broadcast.read(reader);
                }
                else if (reader.nodeType() == XmlNodeType::Element &&
                         reader.localName() == "timeout")
                {
                    server.timeout.read(reader);
                }
                else if (reader.nodeType() == XmlNodeType::Element &&
                         reader.localName() == "secure")
                {
                    server.secure.read(reader);
                }
            }
            return true;
        }
        return false;
    }
    const Server& ServerConfig::server() const
    {
        return _server;
    }
    
    ClientConfig::ClientConfig(const ConfigFile& file) : Configuration(file)
    {
    }
    
    ClientConfig::~ClientConfig()
    {
    }
    
    void ClientConfig::loadClientNode(XmlTextReader& reader)
    {
        loadClientNode(reader, _client);
    }
    bool ClientConfig::loadClientNode(XmlTextReader& reader, Client& client, const String& name)
    {
        if (reader.nodeType() == XmlNodeType::Element &&
            reader.localName() == name)
        {
            client.name = reader.getAttribute("name");
            client.address = reader.getAttribute("address");
            Int32::parse(reader.getAttribute("port"), client.port);
            BaseCommConfig::parseBufferLength(reader.getAttribute("packetlength"), client.packetLength);
            BaseCommConfig::parseBufferLength(reader.getAttribute("sendBufferSize"), client.sendBufferSize);
            BaseCommConfig::parseBufferLength(reader.getAttribute("receiveBufferSize"), client.receiveBufferSize);
            Boolean::parse(reader.getAttribute("noDelay"), client.noDelay);
            Boolean::parse(reader.getAttribute("reuseAddress"), client.reuseAddress);
            Boolean::parse(reader.getAttribute("blocking"), client.blocking);
            Boolean::parse(reader.getAttribute("enabled"), client.enabled);
            while (reader.read() &&
                   !(reader.nodeType() == XmlNodeType::EndElement &&
                   reader.localName() == "client"))
            {
                if (reader.nodeType() == XmlNodeType::Element &&
                    reader.localName() == "broadcast")
                {
                    client.broadcast.read(reader);
//                    TimeSpan::parse(reader.getAttribute("interval"), client.broadcast.interval);
//                    Int32::parse(reader.getAttribute("count"), client.broadcast.count);
//                    Int32::parse(reader.getAttribute("sendport"), client.broadcast.sendport);
//                    Int32::parse(reader.getAttribute("receiveport"), client.broadcast.receiveport);
//                    Boolean::parse(reader.getAttribute("enabled"), client.broadcast.enabled);
                }
                else if (reader.nodeType() == XmlNodeType::Element &&
                         reader.localName() == "connection")
                {
                    client.connection.read(reader);
//                    TimeSpan::parse(reader.getAttribute("detectioninterval"), client.connection.detectionInterval);
//                    TimeSpan::parse(reader.getAttribute("resumeinterval"), client.connection.resumeInterval);
//                    Int32::parse(reader.getAttribute("detectioncount"), client.connection.detectionCount);
//                    Boolean::parse(reader.getAttribute("showError"), client.connection.showError);
                }
                else if (reader.nodeType() == XmlNodeType::Element &&
                         reader.localName() == "timeout")
                {
                    client.timeout.read(reader);
//                    TimeSpan::parse(reader.getAttribute("open"), client.timeout.open);
//                    TimeSpan::parse(reader.getAttribute("receive"), client.timeout.receive);
//                    TimeSpan::parse(reader.getAttribute("send"), client.timeout.send);
                }
                else if (reader.nodeType() == XmlNodeType::Element &&
                         reader.localName() == "secure")
                {
                    client.secure.read(reader);
                }
            }
            return true;
        }
        return false;
    }
    
    const Client& ClientConfig::client() const
    {
        return _client;
    }
}
