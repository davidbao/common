//
//  ClientService.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef ClientService_h
#define ClientService_h

#include "data/TimeSpan.h"
#include "system/Math.h"
#include "data/Dictionary.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "system/Application.h"
#include "driver/instructions/InstructionContext.h"
#include "driver/DriverManager.h"
#include "system/ServiceFactory.h"
#include "CommSampler.h"
#include "BaseCommService.h"
#include "BaseCommConfig.h"
#ifdef PHONE_OS
#include "system/PhoneApplication.h"
#endif

using namespace Common;

namespace Drivers
{
    class TcpInteractive;
}
using namespace Drivers;

namespace Communication
{
    class ClientService : public BaseCommService
    {
    public:
        enum DownloadFileResult
        {
            Unknown = 0,
            Succeed = 1,
            CommunicationError = 2,
            FileNotFound = 3,
            MD5Failed = 4,
            MoveFailed = 5,
            NoNeedDownload = 6,
            Abort = 7
        };
        
        ClientService();
        ClientService(const Client& client);
        ~ClientService() override;
        
        bool initialize(const InstructionCallback& callback) override;
        bool unInitialize() override;
        
        const Client& client() const;
        void setClient(const Client& client);
        
        Delegates* receivedDelegates();
        
        TcpInteractive* ti() const;
        Interactive* interactive() const;
        
        // T is input data, C is context.
        // async send.
        template<class T, class C>
        class LoopSender : public BaseLoopSender
        {
        public:
            LoopSender(BaseCommService* service, const String& name, const TimeSpan& interval, int sendCount, const T& inputData) : BaseLoopSender(service, name, interval, Endpoint::Empty, sendCount)
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
                ClientService* cs = dynamic_cast<ClientService*>(_service);
                if(cs != nullptr)
                {
                    cs->sendAsync<T, C>(_inputData, _name);
                    return true;
                }
                return false;
            }
            
        private:
            T _inputData;
        };
        // T is input data, C is context.
        // start async sender.
        template<class T, class C>
        void startLoopSender(const T& inputData, const String& name, const TimeSpan& interval, int sendCount = -1)
        {
            if (!_instructionPool)
                return;
            if (clientSampler()->isOffline())
                return;
            
            Debug::writeFormatLine("ClientService::startLoopSender, name: %s", name.c_str());
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
            BaseLoopSender* sender = new LoopSender<T, C>(this, name, interval, sendCount, inputData);
            sender->start();
            _loopSenders.add(sender);
        }
        // stop async sender.
        void stopLoopSender(const String& name);
        
        bool hasLoopSender(const String& name);
        
        // T is input data, K is item of T, C is context.
        // async send.
        template<class T, class K, class C>
        class PacketSender : public BasePacketSender
        {
        public:
            typedef LoopVector<K> Buffer;
            
            PacketSender(BaseCommService* service, const String& name, const TimeSpan& interval, int packetCount = 10) : BasePacketSender(service, name, interval), _buffer(MaxBufferLength)
            {
                _packetCount = packetCount;
            }
            ~PacketSender() override
            {
            }
            
            void add(const K* data)
            {
                Locker locker(&_bufferMutex);
                _buffer.enqueue(data);
            }
            
        protected:
            void invoke() override
            {
                ClientService* cs = dynamic_cast<ClientService*>(_service);
                assert(cs);
                
                Locker locker(&_bufferMutex);
                size_t count = _buffer.count();
                if(count > 0)
                {
//                    Debug::writeFormatLine("send async packet, buffer count: %d, packet count: %d, name: %s", count, _packetCount, _name.c_str());
                    K** datas = new K*[count];
                    _buffer.copyTo(datas);
                    _buffer.makeNull(false);
                    T data;
                    data.addRange((const K**)datas, count);
                    cs->sendVectorAsync<T, C>(data, _packetCount, _name);
                    delete[] datas;
                }
            }
            
        private:
            Mutex _bufferMutex;
            Buffer _buffer;
            
            int _packetCount;
            static const int MaxBufferLength = 60000;
        };
        // T is input data, K is item of T, C is context.
        // start async sender.
        template<class T, class K, class C>
        bool startPacketSender(const String& name, const TimeSpan& interval, int packetCount = 10)
        {
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            Debug::writeFormatLine("ClientService::startPacketSender, name: %s", name.c_str());
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                BasePacketSender* sender = _packetSenders[i];
                if(sender->name() == name)
                {
                    // Do not remove the old one.
                    return false;
                }
            }
            auto* sender = new PacketSender<T, K, C>(this, name, interval, packetCount);
            sender->start();
            _packetSenders.add(sender);
            return true;
        }
        // stop async sender.
        bool stopPacketSender(const String& name);
        bool hasPacketSender(const String& name);
        
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSender(const String& name, const K* data)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                PacketSender<T, K, C>* sender = dynamic_cast<PacketSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->add(data);
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
            typedef LoopVector<K> Buffer;
            typedef void (*FailedAction)(void*, const T&);
            
            PacketSyncSender(BaseCommService* service, const String& name, const TimeSpan& interval, FailedAction failedAction, void* owner, int packetCount = 10) : BasePacketSender(service, name, interval), _buffer(MaxBufferLength)
            {
                _packetCount = packetCount;
                _failedAction = failedAction;
                _owner = owner;
            }
            ~PacketSyncSender() override
            {
            }
            
            void add(const K* data)
            {
                Locker locker(&_bufferMutex);
                _buffer.enqueue(data);
            }
            void addRange(const T& data)
            {
                Locker locker(&_bufferMutex);
                for (uint i=0; i<data.count(); i++)
                {
                    _buffer.enqueue(data[i]->clone());
                }
            }
            
        protected:
            void invoke() override
            {
                ClientService* cs = dynamic_cast<ClientService*>(_service);
                assert(cs);
                
                _bufferMutex.lock();
                size_t count = _buffer.count();
#ifdef DEBUG
                Stopwatch sw(String::format("PacketSyncSender::invoke, buffer count: %d", count), 1000);
#endif
                if(count > 0)
                {
//                    Trace::debug(String::format("send sync packet, buffer count: %d, packet count: %d, name: %s", count, _packetCount, _name.c_str()));
                    K** datas = new K*[count];
                    _buffer.copyTo(datas);
                    _buffer.makeNull(false);
                    _bufferMutex.unlock();
                    
                    T data;
                    data.addRange((const K**)datas, count);
                    StatusContext status = cs->sendVectorSync<T, C>(data, _packetCount, _name);
                    if(!status.isSuccessful() && _failedAction != nullptr)
                        _failedAction(_owner, data);
                    delete[] datas;
                }
                else
                {
                    _bufferMutex.unlock();
                }
            }
            
        private:
            Mutex _bufferMutex;
            Buffer _buffer;
            
            int _packetCount;
            static const int MaxBufferLength = 60000;
            
            FailedAction _failedAction;
            void* _owner;
        };
        // T is input data, K is item of T, C is context.
        // start sync sender.
        template<class T, class K, class C>
        bool startPacketSyncSender(typename PacketSyncSender<T, K, C>::FailedAction failedAction, void* owner, const String& name, const TimeSpan& interval, int packetCount = 10)
        {
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            Debug::writeFormatLine("ClientService::startPacketSyncSender, name: %s", name.c_str());
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                BasePacketSender* sender = _packetSenders[i];
                if(sender->name() == name)
                {
                    // Do not remove the old one.
                    return false;
                }
            }
            auto* sender = new PacketSyncSender<T, K, C>(this, name, interval, failedAction, owner, packetCount);
            sender->start();
            _packetSenders.add(sender);
            return true;
        }
        // stop sync sender.
        bool stopPacketSyncSender(const String& name);
        bool hasPacketSyncSender(const String& name);
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String& name, const K* data)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                PacketSyncSender<T, K, C>* sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->add(data);
                    return true;
                }
            }
            
            delete data;
            return false;
        }
        // T is input data, K is item of T, C is context.
        // add a packet sender.
        template<class T, class K, class C>
        bool addPacketSyncSender(const String& name, const T& data)
        {
            Locker locker(&_packetSendersMutex);
            for (uint i=0; i<_packetSenders.count(); i++)
            {
                PacketSyncSender<T, K, C>* sender = dynamic_cast<PacketSyncSender<T, K, C>*>(_packetSenders[i]);
                if(sender != nullptr && sender->name() == name)
                {
                    sender->addRange(data);
                    return true;
                }
            }
            return false;
        }
        
        // T is input data, K is output data, C is context.
        template<class T, class K, class C>
        bool sendSync(const T& inputData, K& outputData, const String& name, int trySendCount = -1) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::sendSync, name: %s", name.c_str()), 2000);
#endif
            if(trySendCount <= 0)
                trySendCount = 1;
            
            bool result = false;
            while(trySendCount > 0)
            {
                trySendCount--;

				if (!_instructionPool)
					return false;
				if (clientSampler()->isOffline())
					return false;
                
                C* context = new C();
                context->setInputData(&inputData);
                context->setOutputData(&outputData);
                InstructionDescription* id = new InstructionDescription(name, context);
                C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(id));
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
        // T is input data, C is context.
        // sync send input data.
        template<class T, class C>
        StatusContext sendSync(const T& inputData, const String& name, int trySendCount = -1) const
        {
            StatusContext status;
            if(sendSync<T, StatusContext, ElementSContext<T>>(inputData, status, name, trySendCount))
            {
                return status;
            }
            return StatusContext(StatusContext::CommunicationError);
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        StatusContext sendVectorSync(const T& inputData, int packetCount, const String& name, int trySendCount = -1)
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorSync.sendSync, name: %s, count: %d", name.c_str(), temp.count());
                StatusContext status = sendSync<T, C>(temp, name, trySendCount);
                if(!status.isSuccessful())
                    return status;
            }
            return StatusContext();
        }
        // T is input data, C is context.
        // async send.
        template<class T, class C>
        bool sendAsync(const T& inputData, const String& name, bool waitingForExecution = false) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::sendAsync, name: %s", name.c_str()), 200);
#endif
            
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            C* context = new C();
            context->setInputData(&inputData);
            InstructionDescription* id = new InstructionDescription(name, context);
            if(!waitingForExecution)
                return _instructionPool->addInstruction(id) != nullptr;
            else
                return _instructionPool->executeInstructionSync(id) != nullptr;
        }
        // T is input data and T is vector, C is context.
        // Sent all of clients if endpoint is empty.
        template<class T, class C>
        bool sendVectorAsync(const T& inputData, int packetCount, const String& name)
        {
            const size_t count = inputData.count();
            for (uint i=0; i<count; i+=packetCount)
            {
                T temp(false);
                temp.addRange(&inputData, i, Math::min(packetCount, (int)(count-i)));
                
//                Debug::writeFormatLine("sendVectorAsync.sendAsync, name: %s, count: %d", name.c_str(), temp.count());
                sendAsync<T, C>(temp, name);
            }
            return true;
        }
        // T is input data, K is output data(Vector), C is context.
        template<class T, class K, class C>
        bool downloadPacketSync(const T& inputData, K& outputData, const String& name) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::downloadPacketSync, name: %s", name.c_str()), 500);
#endif
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            bool result = false;
            C* context = new C();
            context->setInputData(&inputData);
            context->transferHeader();
            InstructionDescription* ids = new InstructionDescription(name, context);
            C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(ids));
            if (isSendSuccessfully(ids->name(), rcontext))
            {
                result = true;
                uint packetCount = rcontext->packetCount();
                for (uint i = 0; i < packetCount; i++)
                {
                    context->setPacketNo(i);
                    context->transferData();
                    context->outputData()->clear();
                    InstructionDescription* id = new InstructionDescription(name, context, false);
                    C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(id));
                    if (isSendSuccessfully(id->name(), rcontext))
                    {
                        outputData.copyFrom(rcontext->outputData(), true);
                        delete id;
                    }
                    else
                    {
                        result = false;
                        delete id;
                        break;
                    }
                }
            }
            delete ids;
            return result;
        }
        // T is input data, C is context.
        template<class T, class C>
        bool downloadPacketAsync(const T& inputData, const String& name, uint packetNo = (uint)-1) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::downloadPacketAsync, name: %s", name.c_str()), 500);
#endif
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            C* context = new C();
            context->setInputData(&inputData);
            if(packetNo == (uint)-1)
            {
                context->transferHeader();
                context->outputData()->clear();
            }
            else
            {
                context->setPacketNo(packetNo);
                context->transferData();
            }
            InstructionDescription* id = new InstructionDescription(name, context);
            return _instructionPool->addInstruction(id) != nullptr;
        }

        // T is input data, C is context.
        template<class T, class C>
        DownloadFileResult downloadFileSync(const String& path, const T& inputData, const String& name, String& fullFileName, FileCommEntry* entry = nullptr, const TimeSpan& timeout = TimeSpan::Zero) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::downloadFile, name: %s", name.c_str()), 200);
#endif
            if (!_instructionPool)
                return DownloadFileResult::CommunicationError;
            if (clientSampler()->isOffline())
                return DownloadFileResult::CommunicationError;

#ifdef DEBUG
            uint start = TickTimeout::getCurrentTickCount();
#endif
            DownloadFileResult result = DownloadFileResult::CommunicationError;
            C* context = new C();
            context->setInputData(&inputData);
            context->transferHeader();
            InstructionDescription* ids = new InstructionDescription(name, context);
            if(timeout != TimeSpan::Zero)
            {
                context->setReceiveTimeout((int)timeout.totalMilliseconds());
            }
            C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(ids));
            if (isSendSuccessfully(ids->name(), rcontext))
            {
                if(rcontext->isFileNotFound())
                {
                    result = DownloadFileResult::FileNotFound;
                }
                else if(rcontext->isNoNeedDownload())
                {
                    result = DownloadFileResult::NoNeedDownload;
                }
                else
                {
                    FileHeader* header = rcontext->header();
                    header->path = path;
                    
                    String fileName = header->file_name;
#ifdef DEBUG
                    uint length = header->fileLength;
#endif
                    uint packetCount = rcontext->packetCount();
                    result = DownloadFileResult::Succeed;
                    
                    for (uint i = 0; i < packetCount; i++)
                    {
                        C* context = new C();
                        context->setInputData(&inputData);
                        context->setHeader(header);
                        context->transferData();
                        InstructionDescription* id = new InstructionDescription(name, context);
                        context->setPacketNo(i);
                        C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(id));
                        if (isSendSuccessfully(id->name(), rcontext))
                        {
                            delete id;
                            
                            if(entry != nullptr && entry->callback != nullptr)
                            {
                                FileCommInfo info;
                                info.packetCount = packetCount;
                                info.packetNo = i;
                                info.fileLength = header->fileLength;
                                info.fileName = fileName;
                                if(!entry->callback(entry->owner, info))
                                {
                                    delete ids;
                                    return DownloadFileResult::Abort;
                                }
                            }
                        }
                        else
                        {
                            result = DownloadFileResult::CommunicationError;
                            delete id;
                            break;
                        }
                    }
                    
                    // check MD5.
                    if(packetCount > 0)
                    {
                        result = header->checkmd5(header->tempFullFileName()) ?
                            DownloadFileResult::Succeed : DownloadFileResult::MD5Failed;
                        if(result == DownloadFileResult::Succeed)
                        {
                            String tempFileName = header->fullFileName();
                            if(File::move(header->tempFullFileName(), tempFileName))
                            {
                                fullFileName = tempFileName;
                            }
                            else
                            {
                                result = DownloadFileResult::MoveFailed;
                            }
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
                        uint elapsed = TickTimeout::elapsed(start);
                        const char* elapsedStr = elapsed <= 10 * 1000 ? "elapsed: %.0f ms" : "elapsed: %.3f s";
                        String info = (String)"download a file %s, file name: %s, file length: %s, speed: %.1f K/s, " + elapsedStr;
                        Trace::writeFormatLine(info.c_str(),
                                               result == DownloadFileResult::Succeed ? "successfully" : "unsuccessfully",
                                               fileName.c_str(),
                                               lengthStr.c_str(),
                                               ((double)length / 1024.0) / ((double)elapsed / 1000.0),
                                               elapsed <= 10 * 1000 ? elapsed : elapsed / 1000.0);
#endif
                    }
                }
            }
            delete ids;
            return result;
        }
        // T is input data, C is context.
        template<class T, class C>
        DownloadFileResult downloadFileSync(const String& path, const T& inputData, const String& name, FileCommEntry* entry = nullptr, const TimeSpan& timeout = TimeSpan::Zero) const
        {
            String fullFileName;
            return downloadFileSync<T, C>(path, inputData, name, fullFileName, entry, timeout);
        }
        
        // T is input data, C is context.
        template<class T, class C>
        bool downloadFileHeaderAsync(const T& inputData, const String& name, uint packetLength = 1 * 1024 * 1024) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::downloadFileHeaderAsync, name: %s", name.c_str()), 200);
#endif

            uint maxPacketLength = this->maxPacketLength();
			if(packetLength > maxPacketLength)
				packetLength = maxPacketLength;

            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;

            C* context = new C();
            context->setInputData(&inputData);
            context->setPacketLength(packetLength);
            context->transferHeader();
            context->outputData()->clear();
            InstructionDescription* id = new InstructionDescription(name, context);
            return _instructionPool->addInstruction(id) != nullptr;
        }
        // T is input data, C is context.
        template<class T, class C>
        bool downloadFileDataAsync(const T& inputData, const String& name, const FileHeader* header, uint packetNo, uint packetLength = 1 * 1024 * 1024) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::downloadFileDataAsync, name: %s", name.c_str()), 200);
#endif
            
            uint maxPacketLength = this->maxPacketLength();
            if(packetLength > maxPacketLength)
                packetLength = maxPacketLength;
            
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;

            C* context = new C();
            context->setInputData(&inputData);
            context->setPacketLength(packetLength);
            context->setHeader(header);
            context->transferData();
            context->setPacketNo(packetNo);
            InstructionDescription* id = new InstructionDescription(name, context);
            return _instructionPool->addInstruction(id) != nullptr;
        }

        // T is input data(Vector), K is EmptyContext, C is context.
        template<class T, class C>
        bool uploadPacketSync(const T& inputData, const String& name) const
        {
            EmptyContext empty;
            return uploadPacketSync<T, EmptyContext, C>(inputData, empty, name);
        }
        // T is input data(Vector), K is output data, C is context.
        template<class T, class K, class C>
        bool uploadPacketSync(const T& inputData, K& outputData, const String& name) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::upload, name: %s", name.c_str()), 200);
#endif
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            bool result = false;
            C* context = new C();
            context->setInputData(&inputData);
            context->transferHeader();
            InstructionDescription* ids = new InstructionDescription(name, context);
            C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(ids));
            if (isSendSuccessfully(ids->name(), rcontext))
            {
                result = true;
                uint packetCount = rcontext->packetCount();
                for (uint i = 0; i < packetCount; i++)
                {
                    context->setPacketNo(i);
                    context->transferData();
                    InstructionDescription* id = new InstructionDescription(name, context, false);
                    C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(id));
                    if (isSendSuccessfully(id->name(), rcontext))
                    {
                        outputData.copyFrom(rcontext->outputData());
                        delete id;
                    }
                    else
                    {
                        result = false;
                        delete id;
                        break;
                    }
                }
            }
            delete ids;
            return result;
        }
        
        
        // T is input data, C is context.
        template<class T, class C>
        bool uploadFileSync(const FileHeader& header, const T& inputData, const String& name, FileCommEntry* entry = nullptr, uint packetLength = (uint)-1, const TimeSpan& interval = TimeSpan::Zero) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::uploadFileSync, name: %s", name.c_str()), 200);
#endif
            if(packetLength != (uint)-1)
            {
                uint maxPacketLength = this->maxPacketLength();
                if(packetLength > maxPacketLength)
                    packetLength = maxPacketLength;
            }
            
            if (!_instructionPool)
                return false;
            if (clientSampler()->isOffline())
                return false;
            
            FileHeader temp;
            temp.copyFrom(&header);
            if (!temp.update())
            {
                Trace::writeLine(String::convert("can not find upload file, path: '%s', file name: '%s'.",
                                                 header.path.c_str(), header.file_name.c_str()), Trace::Error);
                return false;
            }

            String fileName = temp.file_name;
#ifdef DEBUG
            uint start = TickTimeout::getCurrentTickCount();
            uint length = temp.fileLength;
#endif
            bool result = false;
            C* context = new C();
            context->setHeader(&temp);
            context->setInputData(&inputData);
            context->transferHeader();
            if(packetLength != (uint)-1)
                context->setPacketLength(packetLength);
            InstructionDescription* ids = new InstructionDescription(name, context);
            C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(ids));
            if (isSendSuccessfully(ids->name(), rcontext))
            {
                result = true;
                uint packetCount = rcontext->packetCount();
                for (uint i = 0; i < packetCount; i++)
                {
                    InstructionDescription* id = new InstructionDescription(name, context, false);
                    context->setPacketNo(i);
                    context->transferData();
                    C* rcontext = dynamic_cast<C*>(_instructionPool->executeInstructionSync(id));
                    if (isSendSuccessfully(id->name(), rcontext))
                    {
                        delete id;
                        
                        if(entry != nullptr && entry->callback != nullptr)
                        {
                            FileCommInfo info;
                            info.packetCount = packetCount;
                            info.packetNo = i;
                            info.fileLength = temp.fileLength;
                            info.fileName = fileName;
                            if(!entry->callback(entry->owner, info))
                            {
                                delete ids;
                                return false;
                            }
                        }
                        
                        if(interval != TimeSpan::Zero)
                        {
                            Thread::sleep(interval);
                        }
                    }
                    else
                    {
                        delete id;
                        result = false;
                        break;
                    }
                }
            }
            delete ids;
            
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
            uint elapsed = TickTimeout::elapsed(start);
            const char* elapsedStr = elapsed <= 10 * 1000 ? "elapsed: %.0f ms" : "elapsed: %.3f s";
            String info = (String)"upload a file %s, file name: %s, file length: %s, speed: %.1f K/s, " + elapsedStr;
            Trace::writeFormatLine(info.c_str(),
                                   result ? "successfully" : "unsuccessfully",
                                   fileName.c_str(),
                                   lengthStr.c_str(),
                                   ((double)length / 1024.0) / ((double)elapsed / 1000.0),
                                   elapsed <= 10 * 1000 ? elapsed : elapsed / 1000.0);
#endif
            return result;
        }
        
        // T is input data, P is item of T, C is context.
        template<class T, class P, class C>
        void uploadPacketAsync(const T& inputData, const String& name, uint packetLength = 1 * 1024 * 1024) const
        {
#ifdef DEBUG
            Stopwatch sw(String::convert("ClientService::uploadPacketAsync, name: %s", name.c_str()), 200);
#endif
            uint maxPacketLength = this->maxPacketLength();
            if(packetLength > maxPacketLength)
                packetLength = maxPacketLength;
            
            if (!_instructionPool)
                return;
            if (clientSampler()->isOffline())
                return;
            
            C* context = new C();
            context->transferHeader();
            context->setPacketLength(packetLength);
            context->setInputData(&inputData);
            InstructionDescription* ids = new InstructionDescription(name, context);
            _instructionPool->addInstruction(ids);
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
                _instructionPool->addInstruction(id);
            }
        }
        
        void addDynamicInstructions(Instructions* sendInstructions, Instructions* recvInstructions);
        void clearDynamicInstructions();
        
        Sampler* clientSampler() const;
        
        const Endpoint& peerEndpoint() const;
        const Endpoint& endpoint() const;
        const P2PEndpoint p2pEndpoint() const;
        bool connected() const;
        
#ifdef PHONE_OS
        void addEventActive(const Delegate& delegate);
        void removeEventActive(const Delegate& delegate);
        
        void addEventInactive(const Delegate& delegate);
        void removeEventInactive(const Delegate& delegate);
        
        void addEventNetworkChanged(const Delegate& delegate);
        void removeEventNetworkChanged(const Delegate& delegate);
#endif
        
    protected:
        void createDevice(const InstructionCallback& callback) override;
        
        bool isSendSuccessfully(const String& instructionName, const ClientContext* ic) const;
        bool isSendSuccessfully(const String& instructionName, const ClientContext* ic, bool condition) const;
        
        virtual String interactiveName() const;
        
        virtual uint maxPacketLength() const;
        
    private:
        void createTcpDevice(const InstructionCallback& callback, bool enabled = true);
        void createUdpSendDevice(const InstructionCallback& callback);
        void createUdpReceiveDevice(const InstructionCallback& callback);
        
#ifdef PHONE_OS
        void active();
        void inactive();
        void networkChanged(PhoneApplication::NetStatus oldStatus, PhoneApplication::NetStatus newStatus);
        
        static void inactived(void* owner, void* sender, EventArgs* args);
        static void actived(void* owner, void* sender, EventArgs* args);
        static void networkChanged(void* owner, void* sender, EventArgs* args);
#endif
        
    protected:
        Client _client;
        TcpInteractive* _ti;
        
#ifdef PHONE_OS
        Delegates _activeDelegates;
        Delegates _inactiveDelegates;
        Delegates _networkChangedDelegates;
#endif
    };
    
#ifndef __EMSCRIPTEN__
    class SSLClientService : public ClientService
    {
    public:
        SSLClientService();
        SSLClientService(const Client& client);
        ~SSLClientService() override;
        
    protected:
        uint maxPacketLength() const override;
        
        String interactiveName() const override;
    };
#endif

    class ClientServices : public IIndexGetter<ClientService*>
    {
    public:
        ClientServices(bool autoDelete = true);
        
        void add(ClientService* cs);
        
        void clear();
        
        ClientService* at(const Endpoint& endpoint) const;
        
        void setAutoDelete(bool autoDelete);
        
        size_t count() const;

        ClientService* at(size_t pos) const override;

    private:
        Vector<ClientService> _services;
    };
    
    class ClientServiceFactory
    {
    public:
        static ClientService* create(const Client& client);
    };

    class IClientService : public IService
    {
    public:
        virtual ClientService* clientService() const = 0;
        virtual Delegates* statusDelegates() = 0;
        
        bool hasService() const;        
        bool connected() const;
    };
}
#endif // ClientService_h
