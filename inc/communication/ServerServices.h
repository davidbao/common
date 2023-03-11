//
//  ServerService.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef ServerServices_h
#define ServerServices_h

#include "data/StringArray.h"
#include "communication/ServerService.h"

namespace Communication {
    class ServerServices {
    public:
        enum Type : uint8_t {
            None = 0x00,
            Tcp = 0x01,
            WebSocket = 0x02,
            SSL = 0x04,
            WebSocketSSL = 0x08,
            TcpAndSSL = Tcp | SSL,
            All = Tcp | WebSocket | SSL | WebSocketSSL,
            Count = 4
        };

        class TypeString {
        public:
            Type type;
            String str;

            explicit TypeString(Type type = Type::None, const String &str = String::Empty);

            bool isEmpty() const;

        public:
            static const TypeString Empty;
            static const TypeString Tcp;
            static const TypeString WebSocket;
            static const TypeString SSL;
            static const TypeString WebSocketSSL;
        };

        class TypeStrings : public IIndexGetter<const TypeString &> {
        public:
            TypeStrings();

            TypeStrings(const TypeString &tcp, const TypeString &ssl);

            TypeStrings(const TypeString &tcp, const TypeString &ws, const TypeString &ssl, const TypeString &wss);

            const TypeString &at(size_t pos) const override;

        public:
            static const TypeStrings TcpAndSSL;
            static const TypeStrings All;

        private:
            TypeString _strings[Type::Count];
        };

        explicit ServerServices(Type type = Type::All);

        ~ServerServices();

        void updateConfig(const BaseCommConfig &config, const String &prefix,
                          const TypeStrings &typeStrings = TypeStrings::All);

        void updateConfig(const StringMap &properties);

        bool initialize(const BaseCommService::InstructionCallback &callback);

        bool unInitialize();

        void getAllServices(PList<ServerService> &services) const;

        ServerService *getService(const Endpoint &peerEndpoint) const;

        ServerService *getService(Type type) const;

        bool hasServices() const;

        TcpServerInteractive::Client *getClient(const Endpoint &peerEndpoint) const;

        Server *getServer(Type type);

        bool isEmpty(Type type);

        bool contains(const Endpoint &endpoint) const;

        bool contains(Type type) const;

        InstructionPool *getClientPool(const Endpoint &endpoint);

        void getClientPools(const Endpoints &endpoints, InstructionPools &ips) const;

        size_t clientCount() const;

        void addCloseDelegate(const Delegate &delegate);

        void removeCloseDelegate(const Delegate &delegate);

        void addAcceptDelegate(const Delegate &delegate);

        void removeAcceptDelegate(const Delegate &delegate);

        void addReceivedDelegate(const Delegate &delegate);

        void removeReceivedDelegate(const Delegate &delegate);

        bool closeClient(const Endpoint &endpoint);

        // T is input data, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class C>
        bool sendAsync(const Endpoint &endpoint, const T &inputData, const String &name) {
            bool result = true;
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                if (!ss->sendAsync<T, C>(endpoint, inputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class C>
        bool sendAsync(const Endpoints &endpoints, const T &inputData, const String &name) {
            bool result = true;
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                if (!ss->sendAsync<T, C>(endpoints, inputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncAll(const T &inputData, const String &name) {
            bool result = true;
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                if (!ss->sendAsyncAll<T, C>(inputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncWithoutClient(const Endpoint &endpoint, const T &inputData, const String &name) {
            bool result = true;
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                if (!ss->sendAsyncWithoutClient<T, C>(endpoint, inputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncWithoutClient(const Endpoints &endpoints, const T &inputData, const String &name) {
            bool result = true;
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                if (!ss->sendAsyncWithoutClient<T, C>(endpoints, inputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, K is output data, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class K, class C>
        bool sendSync(const Endpoint &endpoint, const T &inputData, K &outputData, const String &name) {
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                return ss->sendSync<T, K, C>(endpoint, inputData, outputData, name);
            }
            return false;
        }

        // T is input data, K is output data, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class K, class C>
        bool sendSync(const Endpoints &endpoints, const T &inputData, K &outputData, const String &name) {
            bool result = true;
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                if (!ss->sendSync<T, K, C>(endpoints, inputData, outputData, name))
                    result = false;
            }
            return result;
        }

        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendSyncAll(const T &inputData, K &outputData, const String &name) {
            return sendSync<T, K, C>(inputData, outputData, name);
        }

        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSender(const String &name, const K &data, const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    K *newData = data.clone();
                    ServerService *ss = services[i];
                    ss->addPacketSender<T, K, C>(name, newData, peerEndpoint);
                }
                return true;
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    K *newData = data.clone();
                    return ss->addPacketSender<T, K, C>(name, newData, peerEndpoint);
                }
            }
            return false;
        }

        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSenderWithoutClient(const String &name, const K &data, const Endpoint &peerEndpoint) {
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                K *newData = data.clone();
                services[i]->addPacketSenderWithoutClient<T, K, C>(name, newData, peerEndpoint);
            }
            return true;
        }

        // T is input data, K is item of T, C is context.
        // start async sender.
        template<class T, class K, class C>
        void startPacketSender(const String &name, const TimeSpan &interval, int packetCount = 10,
                               const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    ServerService *ss = services[i];
                    ss->startPacketSender<T, K, C>(name, interval, packetCount);
                }
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    ss->startPacketSender<T, K, C>(name, interval, packetCount);
                }
            }
        }

        // stop async sender.
        void stopPacketSender(const String &name, const Endpoint &peerEndpoint = Endpoint::Empty);

        // T is input data, K is item of T, C is context.
        // start sync sender.
        template<class T, class K, class C>
        void
        startPacketSyncSender(typename ServerService::PacketSyncSender<T, K, C>::FailedAction failedAction, void *owner,
                              const String &name, const TimeSpan &interval, int packetCount = 10,
                              const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    ServerService *ss = services[i];
                    ss->startPacketSyncSender<T, K, C>(failedAction, owner, name, interval, packetCount);
                }
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    ss->startPacketSyncSender<T, K, C>(failedAction, owner, name, interval, packetCount);
                }
            }
        }

        // T is input data, K is item of T, C is context.
        // start sync sender.
        template<class T, class K, class C>
        void startPacketSyncSender(const String &name, const TimeSpan &interval, int packetCount = 10,
                                   const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    ServerService *ss = services[i];
                    ss->startPacketSyncSender<T, K, C>(name, interval, packetCount);
                }
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    ss->startPacketSyncSender<T, K, C>(name, interval, packetCount);
                }
            }
        }

        // stop sync sender.
        bool stopPacketSyncSender(const String &name, const Endpoint &peerEndpoint = Endpoint::Empty);

        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String &name, const K &data, const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    K *newData = data.clone();
                    ServerService *ss = services[i];
                    ss->addPacketSyncSender<T, K, C>(name, newData, peerEndpoint);
                }
                return true;
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    K *newData = data.clone();
                    return ss->addPacketSyncSender<T, K, C>(name, newData, peerEndpoint);
                }
            }
            return false;
        }

        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String &name, const T &data, const Endpoint &peerEndpoint = Endpoint::Empty) {
            if (peerEndpoint.isEmpty()) {
                PList<ServerService> services;
                getAllServices(services);
                for (size_t i = 0; i < services.count(); i++) {
                    T *newData = data.clone();
                    ServerService *ss = services[i];
                    ss->addPacketSender<T, K, C>(name, newData, peerEndpoint);
                }
                return true;
            } else {
                ServerService *ss = getService(peerEndpoint);
                if (ss != nullptr) {
                    T *newData = data.clone();
                    return ss->addPacketSender<T, K, C>(name, newData, peerEndpoint);
                }
            }
            return false;
        }

        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSenderWithoutClient(const String &name, const K &data, const Endpoint &peerEndpoint) {
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                K *newData = data.clone();
                services[i]->addPacketSyncSenderWithoutClient<T, K, C>(name, newData, peerEndpoint);
            }
            return true;
        }

        // T is input data and T is vector, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsync(const Endpoint &endpoint, const T &inputData, int packetCount, const String &name) {
            bool result = true;
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                if (!ss->sendVectorAsync<T, C>(endpoint, inputData, packetCount, name))
                    result = false;
            }
            return result;
        }

        // T is input data and T is vector, C is context.
        // Sent all clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsync(const Endpoints &endpoints, const T &inputData, int packetCount, const String &name) {
            bool result = true;
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                if (!ss->sendVectorAsync<T, C>(endpoints, inputData, packetCount, name))
                    result = false;
            }
            return result;
        }

        // T is input data, P is item of T, C is context.
        template<class T, class P, class C>
        void uploadPacketAsync(const Endpoint &endpoint, const T &inputData, const String &name,
                               uint32_t packetLength = 1 * 1024 * 1024) const {
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                ss->uploadPacketAsync<T, P, C>(endpoint, inputData, name, packetLength);
            }
        }

        // T is input data, C is context.
        template<class T, class C>
        bool uploadFileSync(const Endpoint &endpoint, const FileHeader &header, const T &inputData,
                            const String &name) const {
            ServerService *ss = getService(endpoint);
            if (ss != nullptr) {
                return ss->uploadFileSync<T, C>(endpoint, header, inputData, name);
            }
            return false;
        }

    private:
        enum Index : uint8_t {
            TcpIndex = 0,
            WebSocketIndex = 1,
            SSLIndex = 2,
            WebSocketSSLIndex = 3
        };

        Server _servers[Type::Count];
        ServerService *_services[Type::Count];

        Type _type;
    };

    class IServerServices : public IService {
    public:
        virtual ServerServices *serverServices() const = 0;

        virtual ServerService *serverService(const Endpoint &peerEndpoint) = 0;
    };
}
#endif // ServerServices_h
