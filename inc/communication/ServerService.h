//
//  ServerService.h
//  common
//
//  Created by baowei on 2016/9/24.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef ServerService_h
#define ServerService_h

#include "data/ValueType.h"
#include "data/Map.h"
#include "thread/Locker.h"
#include "thread/Thread.h"
#include "system/ServiceFactory.h"
#include "communication/BaseCommService.h"
#include "communication/BaseCommConfig.h"
#include "driver/channels/TcpServerInteractive.h"

using namespace Common;

namespace Drivers
{
    class TcpServerInteractive;
    class UdpServerInteractive;
}
using namespace Drivers;

namespace Communication
{
    class ServerService : public BaseCommService
    {
    public:
        ServerService();
        ServerService(const Server& server);
        ~ServerService() override;
        
        bool initialize(const InstructionCallback& callback) override;
        bool unInitialize() override;
        
        const Server& server() const;
        
        TcpServerInteractive* tsi() const;
        
        size_t clientCount() const;
        
        TcpServerInteractive::Client* getClient(const Endpoint& peerEndpoint) const;
        
        void addDynamicInstructions(const Endpoint& peerEndpoint, Instructions* sendInstructions, Instructions* recvInstructions);
        void clearDynamicInstructions(const Endpoint& peerEndpoint);
        
        Delegates* receivedDelegates();
        
        bool contains(const Endpoint& peerEndpoint) const;
        
        void addCloseDelegate(const Delegate& delegate);
        void removeCloseDelegate(const Delegate& delegate);
        
        void addAcceptDelegate(const Delegate& delegate);
        void removeAcceptDelegate(const Delegate& delegate);

        void addReceivedDelegate(const Delegate& delegate);
        void removeReceivedDelegate(const Delegate& delegate);
        
        bool closeClient(const Endpoint& peerEndpoint);
        
        // T is input data, C is context.
        // async send.
        template<class T, class C>
        class LoopSender : public BaseLoopSender
        {
        public:
            LoopSender(BaseCommService* service, const String& name, const TimeSpan& interval, const Endpoint& endpoint, int sendCount, const T& inputData) : BaseLoopSender(service, name, interval, endpoint, sendCount)
            {
                _inputData.copyFrom(&inputData);
            }
            ~LoopSender() override
            {
            }
            
        protected:
            bool invoke() override
            {
                if(!canInvoke())
                    return false;
                
                ServerService* ss = dynamic_cast<ServerService*>(_service);
                assert(ss);
                if(_endpoint.isEmpty())
                    ss->sendAsyncAll<T, C>(_inputData, _name);
                else
                    ss->sendAsync<T, C>(_endpoint, _inputData, _name);
                return true;
            }
            
        private:
            T _inputData;
        };
        // T is input data, C is context.
        // start async sender.
        template<class T, class C>
        void startLoopSender(const T& inputData, const String& name, const TimeSpan& interval, const Endpoint& endpoint = Endpoint::Empty, int sendCount = -1)
        {
            Debug::writeFormatLine("ServerService::startLoopSender, name: %s", name.c_str());
            Locker locker(&_loopSendersMutex);
            for (uint i=0; i<_loopSenders.count(); i++)
            {
                BaseLoopSender* sender = _loopSenders[i];
                if(sender->name() == name)
                {
                    _loopSenders.removeAt(i);
                    break;
                }
            }
            BaseLoopSender* sender = new LoopSender<T, C>(this, name, interval, endpoint, sendCount, inputData);
            sender->start();
            _loopSenders.add(sender);
        }
        // T is input data, C is context.
        // start async sender.
        template<class T, class C>
        void startLoopSender(const T& inputData, const String& name, const TimeSpan& interval, int sendCount = -1)
        {
            startLoopSender<T, C>(inputData, name, interval, Endpoint::Empty, sendCount);
        }
        // stop async sender.
        void stopLoopSender(const String& name);
        
        // T is input data, K is item of T, C is context.
        // async send.
        template<class T, class K, class C>
        class PacketSender : public BasePacketSender
        {
        public:
            typedef LoopPList<K> Buffer;
            typedef Map<Endpoint, Buffer> EndpointBuffers;
            
            PacketSender(ServerService* cs, const String& name, const TimeSpan& interval, int packetCount = 10) : BasePacketSender(cs, name, interval), _includedBuffer(true), _excludedBuffer(true)
            {
                _packetCount = packetCount;
            }
            ~PacketSender() override
            {
            }
            
            void add(const K* data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_includedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_includedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _includedBuffer.add(peerEndpoint, buffer);
                }
                buffer->enqueue(data);
            }
            void addRange(const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_includedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_includedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _includedBuffer.add(peerEndpoint, buffer);
                }
                for (uint i=0; i<data.count(); i++)
                {
                    buffer->enqueue(data[i]->clone());
                }
            }
            
            void addWithoutClient(const K* data, const Endpoint& peerEndpoint)
            {
                Locker locker(&_excludedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_excludedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _excludedBuffer.add(peerEndpoint, buffer);
                }
                buffer->enqueue(data);
            }
            void addRangeWithoutClient(const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_excludedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_excludedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _excludedBuffer.add(peerEndpoint, buffer);
                }
                for (uint i=0; i<data.count(); i++)
                {
                    buffer->enqueue(data[i]->clone());
                }
            }
            
        protected:
            void invoke() override
            {
                processIncluded();
                processExcluded();
            }
            
        private:
            void processIncluded()
            {
                ServerService* ss = dynamic_cast<ServerService*>(_service);
                assert(ss);
                
                Locker locker(&_includedBufferMutex);
                PList<Endpoint> keys;
                _includedBuffer.keys(keys);
                for(uint i=0;i<keys.count();i++)
                {
                    const Endpoint peerEndpoint = *keys[i];
                    Buffer* buffer = nullptr;
                    if(_includedBuffer.at(peerEndpoint, buffer))
                    {
                        assert(buffer);
                        size_t count = buffer->count();
                        if(count > 0)
                        {
                            K** datas = new K*[count];
                            buffer->copyTo(datas);
                            buffer->makeNull(false);
                            T data;
                            data.addRange((const K**)datas, count);
//                            for(uint i=0;i<count;i++)
//                            {
//                                Debug::writeFormatLine("async tag value, %s", datas[i]->toString().c_str());
//                            }
                            ss->sendVectorAsync<T, C>(peerEndpoint, data, _packetCount, _name);
                            delete[] datas;
                        }
                    }
                }
                _includedBuffer.clear();
            }
            void processExcluded()
            {
                ServerService* ss = dynamic_cast<ServerService*>(_service);
                assert(ss);
                
                Locker locker(&_excludedBufferMutex);
                PList<Endpoint> keys;
                _excludedBuffer.keys(keys);
                for(uint i=0;i<keys.count();i++)
                {
                    const Endpoint peerEndpoint = *keys[i];
                    Buffer* buffer = nullptr;
                    if(_excludedBuffer.at(peerEndpoint, buffer))
                    {
                        assert(buffer);
                        size_t count = buffer->count();
                        if(count > 0)
                        {
                            K** datas = new K*[count];
                            buffer->copyTo(datas);
                            buffer->makeNull(false);
                            T data;
                            data.addRange((const K**)datas, count);
                            ss->sendVectorAsyncWithoutClient<T, C>(peerEndpoint, data, _packetCount, _name);
                            delete[] datas;
                        }
                    }
                }
                _excludedBuffer.clear();
            }
            
        private:
            Mutex _includedBufferMutex;
            EndpointBuffers _includedBuffer;
            
            Mutex _excludedBufferMutex;
            EndpointBuffers _excludedBuffer;
            
            int _packetCount;
            static const int MaxBufferLength = 6000;
        };
        // T is input data, K is item of T, C is context.
        // start async sender.
        template<class T, class K, class C>
        void startPacketSender(const String& name, const TimeSpan& interval, int packetCount = 10)
        {
            Debug::writeFormatLine("ServerService::startPacketSender, name: %s", name.c_str());
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                BasePacketSender* sender = _packetSenders[i];
                if(sender->name() == name)
                {
                    _packetSenders.removeAt(i);
                    break;
                }
            }
            auto sender = new PacketSender<T, K, C>(this, name, interval, packetCount);
            sender->start();
            _packetSenders.add(sender);
        }
        // stop async sender.
        void stopPacketSender(const String& name);
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSender(const String& name, const K* data, const Endpoint& peerEndpoint = Endpoint::Empty)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->add(data, peerEndpoint);
                    return true;
                }
            }
            
            delete data;
            return false;
        }
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSender(const String& name, const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->addRange(data);
                    return true;
                }
            }
            return false;
        }
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSenderWithoutClient(const String& name, const K* data, const Endpoint& peerEndpoint)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->addWithoutClient(data, peerEndpoint);
                    return true;
                }
            }
            
            delete data;
            return false;
        }
        
        // T is input data, K is item of T, C is context.
        // sync send.
        template<class T, class K, class C>
        class PacketSyncSender : public BasePacketSender
        {
        public:
            typedef LoopPList<K> Buffer;
            typedef Map<Endpoint, Buffer> EndpointBuffers;
            typedef PList<Buffer> Buffers;
            typedef void (*FailedAction)(void*, const T&);
            
            PacketSyncSender(BaseCommService* service, const String& name, const TimeSpan& interval, FailedAction failedAction, void* owner, int packetCount = 10) : BasePacketSender(service, name, interval), _includedBuffer(true), _excludedBuffer(true)
            {
                _packetCount = packetCount;
                _failedAction = failedAction;
                _owner = owner;
            }
            ~PacketSyncSender() override
            {
            }
            
            void add(const K* data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_includedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_includedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _includedBuffer.add(peerEndpoint, buffer);
                }
                buffer->enqueue(data);
            }
            void addRange(const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_includedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_includedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _includedBuffer.add(peerEndpoint, buffer);
                }
                for (uint i=0; i<data.count(); i++)
                {
                    buffer->enqueue(data[i]->clone());
                }
            }
            
            void addWithoutClient(const K* data, const Endpoint& peerEndpoint)
            {
                Locker locker(&_excludedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_excludedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _excludedBuffer.add(peerEndpoint, buffer);
                }
                buffer->enqueue(data);
            }
            void addRangeWithoutClient(const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
            {
                Locker locker(&_excludedBufferMutex);
                Buffer* buffer = nullptr;
                if(!_excludedBuffer.at(peerEndpoint, buffer))
                {
                    buffer = new Buffer(MaxBufferLength);
                    _excludedBuffer.add(peerEndpoint, buffer);
                }
                for (uint i=0; i<data.count(); i++)
                {
                    buffer->enqueue(data[i]->clone());
                }
            }
            
        protected:
            void invoke() override
            {
                processIncluded();
                processExcluded();
            }
            
        private:
            void processIncluded()
            {
                ServerService* ss = dynamic_cast<ServerService*>(_service);
                assert(ss);

                Buffers buffers(true);
                Endpoints peerEndpoints;
                
                _includedBufferMutex.lock();
                PList<Endpoint> keys;
                _includedBuffer.keys(keys);
                for(uint i=0;i<keys.count();i++)
                {
                    const Endpoint peerEndpoint = *keys[i];
                    Buffer* buffer = nullptr;
                    if(_includedBuffer.at(peerEndpoint, buffer))
                    {
                        peerEndpoints.add(peerEndpoint);
                        buffers.add(buffer);
                    }
                }
                _includedBuffer.clear(false);
                _includedBufferMutex.unlock();
                
                for (uint i=0; i<buffers.count(); i++)
                {
                    const Endpoint peerEndpoint = peerEndpoints[i];
                    Buffer* buffer = buffers[i];
                    size_t count = buffer->count();
                    if(count > 0)
                    {
                        K** datas = new K*[count];
                        buffer->copyTo(datas);
                        buffer->makeNull(false);
                        T data;
                        data.addRange((const K**)datas, count);
//                            for(uint i=0;i<count;i++)
//                            {
//                                Debug::writeFormatLine("sync tag value, %s", datas[i]->toString().c_str());
//                            }
                        StatusContext status = ss->sendVectorSync<T, C>(peerEndpoint, data, _packetCount, _name);
                        if(!status.isSuccessful() && _failedAction != nullptr)
                            _failedAction(_owner, data);
                        delete[] datas;
                    }
                }
            }
            void processExcluded()
            {
                ServerService* ss = dynamic_cast<ServerService*>(_service);
                assert(ss);
                
                Buffers buffers(true);
                Endpoints peerEndpoints;
                
                _excludedBufferMutex.lock();
                PList<Endpoint> keys;
                _excludedBuffer.keys(keys);
                for(uint i=0;i<keys.count();i++)
                {
                    const Endpoint peerEndpoint = *keys[i];
                    Buffer* buffer = nullptr;
                    if(_excludedBuffer.at(peerEndpoint, buffer))
                    {
                        peerEndpoints.add(peerEndpoint);
                        buffers.add(buffer);
                    }
                }
                _excludedBuffer.clear(false);
                _excludedBufferMutex.unlock();
                
                for (uint i=0; i<buffers.count(); i++)
                {
                    const Endpoint peerEndpoint = peerEndpoints[i];
                    Buffer* buffer = buffers[i];
                    size_t count = buffer->count();
                    if(count > 0)
                    {
                        K** datas = new K*[count];
                        buffer->copyTo(datas);
                        buffer->makeNull(false);
                        T data;
                        data.addRange((const K**)datas, count);
                        StatusContext status = ss->sendVectorSyncWithoutClient<T, C>(peerEndpoint, data, _packetCount, _name);
                        if(!status.isSuccessful() && _failedAction != nullptr)
                            _failedAction(_owner, data);
                        delete[] datas;
                    }
                }
            }
            
        private:
            Mutex _includedBufferMutex;
            EndpointBuffers _includedBuffer;
            
            Mutex _excludedBufferMutex;
            EndpointBuffers _excludedBuffer;
            
            int _packetCount;
            static const int MaxBufferLength = 60000;
            
            FailedAction _failedAction;
            void* _owner;
        };
        // T is input data, K is item of T, C is context.
        // start sync sender.
        template<class T, class K, class C>
        void startPacketSyncSender(typename PacketSyncSender<T, K, C>::FailedAction failedAction, void* owner, const String& name, const TimeSpan& interval, int packetCount = 10)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                BasePacketSender* sender = _packetSenders[i];
                if(sender->name() == name)
                {
                    // Do not remove the old one.
                    return;
                }
            }
            auto sender = new PacketSyncSender<T, K, C>(this, name, interval, failedAction, owner, packetCount);
            sender->start();
            _packetSenders.add(sender);
        }
        // T is input data, K is item of T, C is context.
        // start sync sender.
        template<class T, class K, class C>
        void startPacketSyncSender(const String& name, const TimeSpan& interval, int packetCount = 10)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                BasePacketSender* sender = _packetSenders[i];
                if(sender->name() == name)
                {
                    // Do not remove the old one.
                    return;
                }
            }
            auto sender = new PacketSyncSender<T, K, C>(this, name, interval, nullptr, nullptr, packetCount);
            sender->start();
            _packetSenders.add(sender);
        }
        // stop sync sender.
        bool stopPacketSyncSender(const String& name);
        bool hasPacketSyncSender(const String& name);
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String& name, const K* data, const Endpoint& peerEndpoint = Endpoint::Empty)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->add(data, peerEndpoint);
                    return true;
                }
            }
            
            delete data;
            return false;
        }
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String& name, const T& data, const Endpoint& peerEndpoint = Endpoint::Empty)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->addRange(data);
                    return true;
                }
            }
            return false;
        }
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSenderWithoutClient(const String& name, const K* data, const Endpoint& peerEndpoint)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                auto sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->addWithoutClient(data, peerEndpoint);
                    return true;
                }
            }
            
            delete data;
            return false;
        }
        
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsyncAll(const T& inputData, int packetCount, const String& name) const
        {
            return sendVectorAsync<T, C>(Endpoint::Empty, inputData, packetCount, name);
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsync(const Endpoint& endpoint, const T& inputData, int packetCount, const String& name) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendVectorAsync<T, C>(endpoints, inputData, packetCount, name);
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsync(const Endpoints& endpoints, const T& inputData, int packetCount, const String& name) const
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorAsync.sendAsync, name: %s, count: %d", name.c_str(), temp.count());
                sendAsync<T, C>(endpoints, temp, name);
            }
            return true;
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsyncWithoutClient(const Endpoint& endpoint, const T& inputData, int packetCount, const String& name) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendVectorAsyncWithoutClient<T, C>(endpoints, inputData, packetCount, name);
        }
        // T is input data, C is context.
        template<class T, class C>
        bool sendVectorAsyncWithoutClient(const Endpoints& endpoints, const T& inputData, int packetCount, const String& name) const
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorAsyncWithoutClient.sendAsync, name: %s, count: %d", name.c_str(), temp.count());
                sendAsyncWithoutClient<T, C>(endpoints, temp, name);
            }
            return true;
        }
        
        // T is input data, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendAsync(const Endpoint& endpoint, const T& inputData, const String& name) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendAsync<T, C>(endpoints, inputData, name);
        }
        // T is input data, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendAsync(const Endpoints& endpoints, const T& inputData, const String& name) const
        {
            bool result = true;
            InstructionPools ips;
            getClientPools(endpoints, ips);
            for (uint i=0; i<ips.count(); i++)
            {
                if(!sendAsync<T, C>(ips[i], inputData, name))
                    result = false;
            }
            return result;
        }
        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncWithoutClient(const Endpoint& endpoint, const T& inputData, const String& name) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendAsyncWithoutClient<T, C>(endpoints, inputData, name);
        }
        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncWithoutClient(const Endpoints& endpoints, const T& inputData, const String& name) const
        {
            bool result = true;
            InstructionPools ips;
            getClientPoolsWithoutClient(endpoints, ips);
//#ifdef DEBUG
//            if(ips.count() > 0)
//            {
//				Debug::writeFormatLine("ServerService::sendAsyncWithoutClient, name: %s, client pool count: %d",
//						name.c_str(), ips.count());
//            }
//#endif
            for (uint i=0; i<ips.count(); i++)
            {
                if(!sendAsync<T, C>(ips[i], inputData, name))
                    result = false;
            }
            return result;
        }
        // T is input data, C is context.
        template<class T, class C>
        bool sendAsyncAll(const T& inputData, const String& name) const
        {
            return sendAsync<T, C>(Endpoint::Empty, inputData, name);
        }
        
        // T is input data, K is output data, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class K, class C>
        bool sendSync(const Endpoint& endpoint, const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendSync<T, K, C>(endpoints, inputData, outputData, name, trySendCount);
        }
        // T is input data, K is output data, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class K, class C>
        bool sendSync(const Endpoints& endpoints, const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            bool result = true;
            InstructionPools ips;
            getClientPools(endpoints, ips);
            for (uint i=0; i<ips.count(); i++)
            {
                if(!sendSync<T, K, C>(ips[i], inputData, outputData, name, trySendCount))
                    result = false;
            }
            return result;
        }
        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendSyncAll(const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            return sendSync<T, K, C>(Endpoint::Empty, inputData, outputData, name, trySendCount);
        }
        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendSyncWithoutClient(const Endpoint& endpoint, const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendSyncWithoutClient<T, K, C>(endpoints, inputData, name, trySendCount);
        }
        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendSyncWithoutClient(const Endpoints& endpoints, const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            bool result = true;
            InstructionPools ips;
            getClientPoolsWithoutClient(endpoints, ips);
//#ifdef DEBUG
//            if(ips.count() > 0)
//            {
//                Debug::writeFormatLine("ServerService::sendSyncWithoutClient, name: %s, client pool count: %d",
//                        name.c_str(), ips.count());
//            }
//#endif
            for (uint i=0; i<ips.count(); i++)
            {
                if(!sendSync<T, K, C>(ips[i], inputData, outputData, name, trySendCount))
                    result = false;
            }
            return result;
        }
        // T is input data, C is context.
        // sync send input data.
        template<class T, class C>
        StatusContext sendSync(const Endpoint& endpoint, const T& inputData, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendSync<T, C>(endpoints, inputData, name, trySendCount);
        }
        // T is input data, C is context.
        // sync send input data.
        template<class T, class C>
        StatusContext sendSync(const Endpoints& endpoints, const T& inputData, const String& name, int trySendCount = -1) const
        {
            StatusContext status;
            if(sendSync<T, StatusContext, ElementSContext<T>>(endpoints, inputData, status, name, trySendCount))
            {
                return status;
            }
            return StatusContext(StatusContext::CommunicationError);
        }
        
        // T is input data, C is context.
        // sync send input data.
        template<class T, class C>
        StatusContext sendSyncWithoutClient(const Endpoint& endpoint, const T& inputData, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendSyncWithoutClient<T, C>(endpoints, inputData, name, trySendCount);
        }
        // T is input data, C is context.
        // sync send input data.
        template<class T, class C>
        StatusContext sendSyncWithoutClient(const Endpoints& endpoints, const T& inputData, const String& name, int trySendCount = -1) const
        {
            StatusContext status;
            if(sendSyncWithoutClient<T, StatusContext, ElementSContext<T>>(endpoints, inputData, status, name, trySendCount))
            {
                return status;
            }
            return StatusContext(StatusContext::CommunicationError);
        }

        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        StatusContext sendVectorSync(const Endpoint& endpoint, const T& inputData, int packetCount, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendVectorSync<T, C>(endpoints, inputData, packetCount, name, trySendCount);
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        StatusContext sendVectorSync(const Endpoints& endpoints, const T& inputData, int packetCount, const String& name, int trySendCount = -1) const
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorSync.sendSync, name: %s, count: %d", name.c_str(), temp.count());
                StatusContext status = sendSync<T, C>(endpoints, temp, name, trySendCount);
                if(!status.isSuccessful())
                    return status;
            }
            return StatusContext();
        }
        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendVectorSyncAll(const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
            return sendVectorSync<T, K, C>(Endpoint::Empty, inputData, outputData, name, trySendCount);
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        StatusContext sendVectorSyncWithoutClient(const Endpoint& endpoint, const T& inputData, int packetCount, const String& name, int trySendCount = -1) const
        {
            Endpoints endpoints;
            endpoints.add(endpoint);
            return sendVectorSyncWithoutClient<T, C>(endpoints, inputData, packetCount, name, trySendCount);
        }
        // T is input data, C is context.
        template<class T, class C>
        StatusContext sendVectorSyncWithoutClient(const Endpoints& endpoints, const T& inputData, int packetCount, const String& name, int trySendCount = -1) const
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorSyncWithoutClient.sendAsync, name: %s, count: %d", name.c_str(), temp.count());
                StatusContext status = sendSyncWithoutClient<T, C>(endpoints, temp, name, trySendCount);
                if(!status.isSuccessful())
                    return status;
            }
            return StatusContext();
        }
        
        // T is input data, C is context.
        template<class T, class C>
        bool uploadFileSync(const FileHeader& header, const T& inputData, const String& name, uint packetLength = (uint)-1, const TimeSpan& interval = TimeSpan::Zero) const
        {
            return uploadFileSync<T, C>(Endpoint::Empty, header, inputData, name, packetLength, interval);
        }
        // T is input data, C is context.
        template<class T, class C>
        bool uploadFileSync(const Endpoint& endpoint, const FileHeader& header, const T& inputData, const String& name, uint packetLength = (uint)-1, const TimeSpan& interval = TimeSpan::Zero) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ServerService::uploadFileSync, name: %s", name.c_str()), 200);
#endif
            
            if(packetLength != (uint)-1)
            {
                uint maxPacketLength = this->maxPacketLength();
                if(packetLength > maxPacketLength)
                    packetLength = maxPacketLength;
            }
            
            InstructionPool* ip = getClientPool(endpoint);
            if (!ip)
                return false;
            if (!ip->channelConnected())
                return false;
            
            FileHeader temp;
            temp.copyFrom(&header);
            if (!temp.update())
            {
                Trace::writeLine(String::convert("can not find upload file, path: '%s', file name: '%s'.",
                                                 header.path.c_str(), header.file_name.c_str()), Trace::Error);
                return false;
            }
            
#ifdef DEBUG
            uint start = TickTimeout::getCurrentTickCount();
            uint length = temp.fileLength;
            String fileName = temp.file_name;
#endif
            bool result = false;
            C* context = new C();
            context->setHeader(&temp);
            context->transferHeader();
            if(packetLength != (uint)-1)
                context->setPacketLength(packetLength);
            context->setInputData(&inputData);
            InstructionDescription* ids = new InstructionDescription(name, context);
            C* rcontext = dynamic_cast<C*>(ip->executeInstructionSync(ids));
            if (isSendSuccessfully(ids->name(), rcontext))
            {
                result = true;
                uint packetCount = rcontext->packetCount();
                for (uint i = 0; i < packetCount; i++)
                {
                    InstructionDescription* id = new InstructionDescription(name, context, false);
                    context->setPacketNo(i);
                    context->transferData();
                    C* rcontext = dynamic_cast<C*>(ip->executeInstructionSync(id));
                    if (!isSendSuccessfully(id->name(), rcontext))
                    {
                        delete id;
                        result = false;
                        break;
                    }
                    delete id;
                    
                    if(interval != TimeSpan::Zero)
                    {
                        Thread::sleep(interval);
                    }
                }
            }
            delete ids;
            
#ifdef DEBUG
            if(result)
            {
                String lengthStr;
                if(length < 1024)
                {
                    lengthStr = Common::UInt32(length).toString();
                }
                else if(length >= 1024 && length < 1024 * 1024)     // 1K - 1M
                {
                    lengthStr = String::convert("%dK", length / 1024);
                }
                else    // > 1M
                {
                    lengthStr = String::convert("%.1fM", (double)length / 1024.0 / 1024.0);
                }
                uint elapsed = TickTimeout::elapsed(start);
                const char* elapsedStr = elapsed <= 10 * 1000 ? "elapsed: %.0f ms" : "elapsed: %.3f s";
                String info = (String)"upload a file %s, file name: %s, file length: %s, speed: %.1f K/s, " + elapsedStr;
                Trace::writeFormatLine(info.c_str(),
                                       result ? "successfully" : "unsuccessfully",
                                       fileName.c_str(),
                                       lengthStr.c_str(),
                                       ((double)length / 1024.0) / ((double)elapsed / 1000.0),
                                       elapsed <= 10 * 1000 ? elapsed : elapsed / 1000.0);
            }
#endif
            return result;
        }
        
        // T is input data, C is context.
        template<class T, class C>
        void uploadFileAsync(const FileHeader& header, const T& inputData, const String& name, uint packetLength = (uint)-1) const
        {
            return uploadFileAsync<T, C>(Endpoint::Empty, header, inputData, name, packetLength);
        }
        // T is input data, C is context.
        template<class T, class C>
        void uploadFileAsync(const Endpoint& endpoint, const FileHeader& header, const T& inputData, const String& name, uint packetLength = (uint)-1) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ServerService::uploadFileAsync, name: %s", name.c_str()), 200);
#endif
            
            if(packetLength != (uint)-1)
            {
                uint maxPacketLength = this->maxPacketLength();
                if(packetLength > maxPacketLength)
                    packetLength = maxPacketLength;
            }
            
            InstructionPool* ip = getClientPool(endpoint);
            if (!ip)
                return;
            if (!ip->channelConnected())
                return;
            
            FileHeader temp;
            temp.copyFrom(&header);
            if (!temp.update())
            {
                Trace::writeLine(String::convert("can not find upload file, path: '%s', file name: '%s'.",
                                                 header.path.c_str(), header.file_name.c_str()), Trace::Error);
                return;
            }
            
#ifdef DEBUG
            uint length = temp.fileLength;
            String fileName = temp.file_name;
#endif
            C* context = new C();
            context->setHeader(&temp);
            context->transferHeader();
            if(packetLength != (uint)-1)
                context->setPacketLength(packetLength);
            context->setInputData(&inputData);
            InstructionDescription* ids = new InstructionDescription(name, context);
            ip->addInstruction(ids);
            uint packetCount = context->calcPacketCount();
            for (uint i = 0; i < packetCount; i++)
            {
                Thread::msleep(10);

                context = new C();
                context->setHeader(&temp);
                context->setPacketLength(packetLength);
                context->setPacketNo(i);
//                Debug::writeFormatLine("packet no: %d", i);
                context->transferData();
                InstructionDescription* id = new InstructionDescription(name, context);
                
                ip = getClientPool(endpoint);
                if(ip != nullptr && ip->channelConnected())
                    ip->addInstruction(id);
                else
                    delete id;
            }
            
#ifdef DEBUG
            String lengthStr;
            if(length < 1024)
            {
                lengthStr = Common::UInt32(length).toString();
            }
            else if(length >= 1024 && length < 1024 * 1024)     // 1K - 1M
            {
                lengthStr = String::convert("%dK", length / 1024);
            }
            else    // > 1M
            {
                lengthStr = String::convert("%.1fM", (double)length / 1024.0 / 1024.0);
            }
            String info = (String)"upload a file, file name: %s, file length: %s";
            Trace::writeFormatLine(info.c_str(),
                                   fileName.c_str(),
                                   lengthStr.c_str());
#endif
        }
        // T is input data, C is context.
        template<class T, class C>
        void uploadFileStatusAsync(const Endpoint& endpoint, uint8_t status, const T& inputData, const String& name, uint packetLength = 1 * 1024 * 1024) const
        {
            InstructionPool* ip = getClientPool(endpoint);
            if (!ip)
                return;
            if (!ip->channelConnected())
                return;
            
            C* context = new C();
            context->status = status;
            context->transferStatus();
            context->setInputData(&inputData);
            InstructionDescription* ids = new InstructionDescription(name, context);
            ip->addInstruction(ids);
        }
        
        // T is input data, P is item of T, C is context.
        template<class T, class P, class C>
        void uploadPacketAsync(const Endpoint& endpoint, const T& inputData, const String& name, uint packetLength = 1 * 1024 * 1024) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ServerService::uploadPacketAsync, name: %s", name.c_str()), 200);
#endif
            uint maxPacketLength = this->maxPacketLength();
            if(packetLength > maxPacketLength)
                packetLength = maxPacketLength;
            
            InstructionPool* ip = getClientPool(endpoint);
            if (!ip)
                return;
            if (!ip->channelConnected())
                return;

            C* context = new C();
            context->transferHeader();
            context->setPacketLength(packetLength);
            context->setInputData(&inputData);
            InstructionDescription* ids = new InstructionDescription(name, context);
            ip->addInstruction(ids);
            uint packetCount = context->calcPacketCount();
            for (uint i = 0; i < packetCount; i++)
            {
                Thread::msleep(10);

                context = new C();
                context->setPacketLength(packetLength);
                context->setPacketNo(i);
//                Debug::writeFormatLine("packet no: %d", i);
                context->transferData();
                context->setInputData(&inputData);
                context->calcPacketCount();
                InstructionDescription* id = new InstructionDescription(name, context);
                ip->addInstruction(id);
            }
        }
        
    protected:
        void createDevice(const InstructionCallback& callback) override;
        
        virtual String interactiveName() const;
        
        virtual uint maxPacketLength() const;

    private:
        void createTcpDevice(const InstructionCallback& callback);
        void createUdpReceiveDevice(const InstructionCallback& callback);
        void createUdpSendDevice(const InstructionCallback& callback);
        void createUdpSendDevice(const InstructionCallback& callback, const String& ipAddress);
        
        void getClientPools(const Endpoints& endpoints, InstructionPools& ips) const;
        InstructionPool* getClientPool(const Endpoint& endpoint) const;
        void getClientPoolsWithoutClient(const Endpoints& endpoints, InstructionPools& ips) const;
        
        bool isSendSuccessfully(const String& instructionName, const InstructionContext* ic) const;
        bool isSendSuccessfully(const String& instructionName, const InstructionContext* ic, bool condition) const;
        
        // T is input data, C is context.
        // Sent all of clients if address is empty.
        template<class T, class C>
        bool sendAsync(InstructionPool* ip, const T& inputData, const String& name) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ServerService::sendAsync, name: %s", name.c_str()), 200);
#endif
            
            if (!ip)
                return false;
            if (!ip->channelConnected())
                return false;
            
            C* context = new C();
            context->setInputData(&inputData);
            InstructionDescription* id = new InstructionDescription(name, context);
            return ip->addInstruction(id) != nullptr;
        }
        // T is input data, K is output data, C is context.
        // Sent all of clients if address is empty.
        template<class T, class K, class C>
        bool sendSync(InstructionPool* ip, const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ServerService::sendSync, device name: %s, instruction name: %s", ip->deviceName().c_str(), name.c_str()), 200);
#endif
            if(trySendCount <= 0)
                trySendCount = 1;
            
            bool result = false;
            while(trySendCount > 0)
            {
                trySendCount--;
            
                if (!ip)
                    return false;
                if (!ip->channelConnected())
                    return false;
                
                C* context = new C();
                context->setInputData(&inputData);
                InstructionDescription* id = new InstructionDescription(name, context);
                C* rcontext = dynamic_cast<C*>(ip->executeInstructionSync(id));
                if (isSendSuccessfully(id->name(), rcontext))
                {
                    result = true;
                    outputData.copyFrom(rcontext->outputData());
                }
                delete id;
                
                if(result)
                    break;
                else if(trySendCount > 0)
                    Thread::msleep(100);
            }
            return result;
        }
        
    protected:
        const Server _server;
        
        TcpServerInteractive* _tsi;
        UdpServerInteractive* _usi;
    };
    
    class SSLServerService : public ServerService
    {
    public:
        SSLServerService();
        SSLServerService(const Server& server);
        ~SSLServerService() override;
        
    protected:
        void createDevice(const InstructionCallback& callback) override;
        
        String interactiveName() const override;
    };
    
    class WebServerService : public ServerService
    {
    public:
        WebServerService();
        WebServerService(const Server& server);
        ~WebServerService() override;
        
    protected:
        String interactiveName() const override;
        
        uint maxPacketLength() const override;
    };
    
    class WebSSLServerService : public ServerService
    {
    public:
        WebSSLServerService();
        WebSSLServerService(const Server& server);
        ~WebSSLServerService() override;
        
    protected:
        void createDevice(const InstructionCallback& callback) override;
        
        String interactiveName() const override;
        
        uint maxPacketLength() const override;
    };

    class ServerServiceFactory
    {
    public:
        static ServerService* create(const Server& server);
    };

    class IServerService : public IService
    {
    public:
        virtual ServerService* serverService() const = 0;
    };
}
#endif // ServerService_h
