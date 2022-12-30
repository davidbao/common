//
//  BaseCommConfig.h
//  common
//
//  Created by baowei on 2015/7/28.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef BaseCommConfig_h
#define BaseCommConfig_h

#include "configuration/Configuration.h"
#include "data/TimeSpan.h"
#include "driver/channels/TcpChannelContext.h"

using namespace Data;
using namespace Drivers;

namespace Communication
{
    struct Broadcast
    {
    public:
        TimeSpan interval;
        int count;
        int sendport;
        int receiveport;
        bool enabled;
        
        Broadcast();
        Broadcast(const Broadcast& value);
        
        void operator=(const Broadcast& value);
        bool operator==(const Broadcast& value) const;
        bool operator!=(const Broadcast& value) const;
        
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        
        void read(XmlNode& node);
        void write(XmlNode& node) const;
    };
    
    class Secure
    {
    public:
        String certFile;
        String keyFile;
        String cacertFile;
        bool enabled;
        
        Secure(bool enabled = false);
        Secure(const Secure& value);
        
        void operator=(const Secure& value);
        bool operator==(const Secure& value) const;
        bool operator!=(const Secure& value) const;
        
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        
        void read(XmlNode& node);
        void write(XmlNode& node) const;
        
    public:
        static const Secure None;
    };

    class Client
    {
    public:
        struct Connection
        {
        public:
            TimeSpan detectionInterval;
            TimeSpan resumeInterval;
            int detectionCount;
            bool showError;
            
            Connection();
            Connection(const Connection& value);
            
            void operator=(const Connection& value);
            bool operator==(const Connection& value) const;
            bool operator!=(const Connection& value) const;
            
            void read(XmlTextReader& reader);
            void write(XmlTextWriter& writer) const;
            
            void read(XmlNode& node);
            void write(XmlNode& node) const;
        };
        struct Timeout
        {
        public:
            TimeSpan send;
            TimeSpan receive;
            TimeSpan open;
            
            Timeout();
            Timeout(const Timeout& value);
            
            void operator=(const Timeout& value);
            bool operator==(const Timeout& value) const;
            bool operator!=(const Timeout& value) const;
            
            void read(XmlTextReader& reader);
            void write(XmlTextWriter& writer) const;
            
            void read(XmlNode& node);
            void write(XmlNode& node) const;
        };
        
        String name;
        String address;
        Broadcast broadcast;
        int port;
        Connection connection;
        Timeout timeout;
        Secure secure;
        uint32_t packetLength;		// unit: bytes
        uint32_t sendBufferSize;
        uint32_t receiveBufferSize;
        bool noDelay;
        bool reuseAddress;
        bool blocking;
        TcpChannelContext::Types receiverType;
        TcpChannelContext::Types senderType;
        bool enabled;
        
        Client(bool enabled = false);
        Client(const String& name, const String& address, int port);
        Client(const Client& value);
        
        bool isEmpty() const;
        
        void copyFrom(const Client* value);
        
        void operator=(const Client& value);
        bool operator==(const Client& value) const;
        bool operator!=(const Client& value) const;
        
        Endpoint endpoint() const;
        
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        
        void read(XmlNode& node);
        void write(XmlNode& node) const;
        
        void updateConfig(const StringMap& properties);
        
        bool asyncReceiver() const;
        bool syncReceiver() const;
        bool multiPlexingReceiver() const;
        
        bool asyncSender() const;
        bool syncSender() const;
        bool multiPlexingSender() const;
        
    public:
        static const Client Empty;
    };
    typedef PList<Client> Clients;
    
    class Server
    {
    public:
        struct Timeout
        {
        public:
            TimeSpan send;
            TimeSpan receive;
            TimeSpan close;
            
            Timeout();
            Timeout(const Timeout& value);
            
            void operator=(const Timeout& value);
            bool operator==(const Timeout& value) const;
            bool operator!=(const Timeout& value) const;
            
            void read(XmlTextReader& reader);
            void write(XmlTextWriter& writer) const;
            
            void read(XmlNode& node);
            void write(XmlNode& node) const;
        };
        
        String name;
        String address;
        Broadcast broadcast;
        int port;
        int maxConnections;
        Timeout timeout;
        Secure secure;
        uint32_t sendBufferSize;
        uint32_t receiveBufferSize;
        bool reuseAddress;
        bool noDelay;
        TcpChannelContext::Types receiverType;
        TcpChannelContext::Types senderType;
        bool enabled;
        
        Server(bool enabled = false);
        Server(const String& name, const String& address, int port);
        Server(const Server& value);
        
        bool isEmpty() const;
        
        void copyFrom(const Server* value);
        
        void operator=(const Server& value);
        bool operator==(const Server& value) const;
        bool operator!=(const Server& value) const;
        
        Endpoint endpoint() const;
        
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        
        void read(XmlNode& node);
        void write(XmlNode& node) const;
        
        void updateConfig(const StringMap& properties, const String& prefix);
        
        bool asyncReceiver() const;
        bool syncReceiver() const;
        bool multiPlexingReceiver() const;
        
        bool asyncSender() const;
        bool syncSender() const;
        bool multiPlexingSender() const;
        
    public:
        static const Server Empty;
    };
    typedef PList<Server> Servers;
    
    class BaseCommConfig : public Configuration
    {
    public:
        BaseCommConfig(const ConfigFile& file);
        ~BaseCommConfig() override;
        
        const Server& server() const;
        const Client& client() const;
        
        const Client& client(const String& name, bool useSSL = true) const;
        const Server& server(const String& name) const;
        
    public:
        static bool parseBufferLength(const String& str, uint32_t& value);
        static const String toBufferLengthStr(uint32_t value);
        
    protected:
        void loadServerNodes(XmlTextReader& reader);
        void loadClientNodes(XmlTextReader& reader);
        
    protected:
        Servers _servers;
        Clients _clients;
    };
    
    class ServerConfig : public Configuration
    {
    public:
        ServerConfig(const ConfigFile& file);
        ~ServerConfig() override;
        
        const Server& server() const;
        
    public:
        static bool loadServerNode(XmlTextReader& reader, Server& server, const String& name = "server");
        
    protected:
        void loadServerNode(XmlTextReader& reader);
        
    protected:
        Server _server;
    };
    
    class ClientConfig : public Configuration
    {
    public:
        ClientConfig(const ConfigFile& file);
        ~ClientConfig() override;
        
        const Client& client() const;        
        
    public:
        static bool loadClientNode(XmlTextReader& reader, Client& client, const String& name = "client");
        
    protected:
        void loadClientNode(XmlTextReader& reader);
        
    protected:
        Client _client;
    };
}

#endif  // BaseCommConfig_h
