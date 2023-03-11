//
//  ClientInstruction.h
//  common
//
//  Created by baowei on 2016/11/16.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef ClientInstruction_h
#define ClientInstruction_h

#include <typeinfo>
#include "thread/Mutex.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/FileInfo.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/MappingStream.h"
#include "IO/FileStream.h"
#include "data/String.h"
#include "diag/Stopwatch.h"
#include "driver/instructions/Instruction.h"
#include "system/Math.h"
#include "BaseCommContext.h"

using namespace Data;

namespace Communication
{
    class ClientInstruction : public Instruction
    {
    public:
        ClientInstruction(InstructionDescription* id);
        ~ClientInstruction() override;
        
        InstructionContext* execute(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer = NULL) override;
        InstructionContext* executeAsync(Interactive* interactive, Device* device, InstructionContext* context) override;
        
        bool match(const ByteArray* buffer, DeviceDescription* dd) override;
        
        void setSender(bool isSender);
        
    public:
        static void getSendBuffer(MemoryStream& ms, uint8_t frameId, uint8_t state, uint8_t command);
        
    protected:
        virtual bool getSendBuffer(MemoryStream& ms, ClientContext* context, uint8_t frameId = 0xFF);
        virtual bool getCommandBuffer(MemoryStream& ms, ClientContext* context) = 0;
        
        virtual ClientContext* setReceiveBuffer(MemoryStream& ms, ClientContext* context, bool checkFrameId = true);
        virtual bool setCommandBuffer(MemoryStream& ms, ClientContext* context) = 0;
        
        virtual uint8_t command() const = 0;
        virtual bool autoResponsed() const;
        
        virtual bool syncSendReceive(Interactive* interactive) const;
        
        virtual bool canMatch() const;
        
        const Version& version() const;
        void setVersion(const Version& version);
        void writeVersion(Stream* stream);
        void readVersion(Stream* stream);
        
        virtual InstructionContext* executeSync(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer);
        virtual InstructionContext* executeAsync(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer);
        
        InstructionContext* send(Interactive* interactive, Device* device, InstructionContext* context, uint8_t frameId = -1);
        InstructionContext* receiveFromBuffer(Interactive* interactive, Device* device, InstructionContext* context);
        InstructionContext* receive(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer);
        
    private:
        uint8_t getFrameId();
        bool isSameFrameId(uint8_t frameId);
        
        bool hasBackgroudReceiver(Interactive* interactive) const;
        
        bool isReceiveFinishedInner();
        void copyReceiveBuffer(ByteArray& buffer);
        
    private:
        static bool isReceiveFinished(void* parameter);
        
    private:
        static char _frameId;
        char _currentFrameId;
        
        ByteArray _backgroundBuffer;
        Mutex _backgroundBufferMutex;
        
        bool _isSender;
        
        Version _version;
    };
    
    // T is input data, K is output data.
    template <class T, class K>
    class ElementInstruction : public ClientInstruction
    {
    public:
        ElementInstruction(InstructionDescription* id) : ClientInstruction(id)
        {
        }
        ~ElementInstruction() override
        {
        }
        
        bool getCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            ElementContext<T, K>* rcontext = dynamic_cast<ElementContext<T, K>*>(context);
            if (rcontext != nullptr)
            {
                if(getValue(context) != NULL)
                {
                    ms.writeByte(command());
                    writeVersion(&ms);
                    
                    rcontext->inputData()->write(&ms);
                    
                    return true;
                }
            }
            return false;
        }
        bool setCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            ElementContext<T, K>* rcontext = dynamic_cast<ElementContext<T, K>*>(context);
            if (rcontext != NULL)
            {
                ms.readByte();	// skip command
                readVersion(&ms);
                
                rcontext->outputData()->read(&ms);
                
                return setValue(rcontext) != NULL;
            }
            return false;
        }
        
        virtual ClientContext* getValue(ClientContext* context)
        {
            ElementContext<T, K>* rcontext = dynamic_cast<ElementContext<T, K>*>(context);
            if (rcontext != NULL)
            {
                return rcontext;
            }
            return NULL;
        }
        virtual ClientContext* setValue(ClientContext* context)
        {
            ElementContext<T, K>* rcontext = dynamic_cast<ElementContext<T, K>*>(context);
            if (rcontext != NULL)
            {
                return rcontext;
            }
            return NULL;
        }
        
    protected:
        bool syncSendReceive(Interactive*) const override
        {
            return true;
        }
    };

    typedef void (*receive_callback)(void* owner, ClientContext* context);

    // T is input data, K is output data.
    template <class T, class K>
    class ElementCInstruction : public ElementInstruction<T, K>
    {
    public:
        ElementCInstruction(InstructionDescription* id, receive_callback callback = nullptr, void* owner = nullptr) : ElementInstruction<T, K>(id), _callback(callback), _owner(owner)
        {
        }
        ~ElementCInstruction() override
        {
        }
        
        ClientContext* setValue(ClientContext* context) override
        {
            if(_callback != nullptr)
                _callback(_owner, context);
            
            return context;
        }
        
    private:
        receive_callback _callback;
        void* _owner;
    };

    // T is input data, K is EmptyContext.
    // Set channel context syncSendReceive=false before use this instruction.
    template <class T>
    class ElementAInstruction : public ElementInstruction<T, EmptyContext>
    {
    public:
        ElementAInstruction(InstructionDescription* id) : ElementInstruction<T, EmptyContext>(id)
        {
        }
        ~ElementAInstruction() override
        {
        }
        
    protected:
        bool syncSendReceive(Interactive*) const override
        {
            return false;
        }
        bool canMatch() const override
        {
            return false;
        }
    };
    // T is input data, K is StatusContext.
    template <class T>
    class ElementSInstruction : public ElementInstruction<T, StatusContext>
    {
    public:
        ElementSInstruction(InstructionDescription* id) : ElementInstruction<T, StatusContext>(id)
        {
        }
        ~ElementSInstruction() override
        {
        }
        
    protected:
        bool syncSendReceive(Interactive*) const override
        {
            return true;
        }
    };
    
    // T is input data, K is output data.
    template <class T, class K>
    class DownloadPacketInstruction : public ClientInstruction
    {
    public:
        DownloadPacketInstruction(InstructionDescription* id) : ClientInstruction(id)
        {
        }
        ~DownloadPacketInstruction() override
        {
        }
        
        bool getCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            PacketContext<T, K>* rcontext = dynamic_cast<PacketContext<T, K>*>(context);
            if (rcontext != NULL)
            {
                ms.writeByte(command());
                writeVersion(&ms);
                
                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                ms.writeByte(rcontext->transfer());
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->inputData()->write(&ms);
                    ms.writeUInt32(rcontext->packetLength());
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    ms.writeUInt32(rcontext->getPacketNo());
                }
                else
                {
                    return false;
                }
                
                return true;
            }
            return false;
        }
        bool setCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            PacketContext<T, K>* rcontext = dynamic_cast<PacketContext<T, K>*>(context);
            if (rcontext != NULL)
            {
                ms.readByte();	// skip command
                readVersion(&ms);
                
                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->status = ms.readByte();
                    if(rcontext->status == StatusContext::Succeed)
                    {
                        rcontext->setPacketCount(ms.readUInt32());
                    }
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    if(ms.readBoolean())
                        rcontext->outputData()->read(&ms);
                }
                else
                {
                    return false;
                }
                
                return setValue(rcontext) != NULL;
            }
            return false;
        }
        
    protected:
        virtual ClientContext* setValue(ClientContext* context)
        {
            return context;
        }
        
        bool syncSendReceive(Interactive*) const override
        {
            return true;
        }
    };

    // T is input data, K is output data.
    template <class T, class K>
    class DownloadPacketCInstruction : public DownloadPacketInstruction<T, K>
    {
    public:
        DownloadPacketCInstruction(InstructionDescription* id, receive_callback callback = nullptr, void* owner = nullptr) : DownloadPacketInstruction<T, K>(id), _callback(callback), _owner(owner)
        {
        }
        ~DownloadPacketCInstruction() override
        {
        }
        
        ClientContext* setValue(ClientContext* context) override
        {
            if(_callback != nullptr)
                _callback(_owner, context);

            return context;
        }
        
    private:
        receive_callback _callback;
        void* _owner;
    };

    // T is input data, K is output data, P is item of T.
    template <class T, class K, class P>
    class UploadPacketInstruction : public ClientInstruction
    {
    public:
        UploadPacketInstruction(InstructionDescription* id, bool syncSendReceive = true) : ClientInstruction(id), _syncSendReceive(syncSendReceive)
        {
        }
        ~UploadPacketInstruction() override
        {
        }
        
        bool getCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            UploadPacketContext<T, P, K>* rcontext = dynamic_cast<UploadPacketContext<T, P, K>*>(context);
            if (rcontext != NULL)
            {
                setValue(rcontext);
                
                ms.writeByte(command());
                writeVersion(&ms);
                
                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                ms.writeByte(rcontext->transfer());
                if(transfer == BasePacketContext::TransferHeader)
                {
                    uint32_t packetCount = rcontext->calcPacketCount();
                    rcontext->setPacketCount(packetCount);
                    ms.writeUInt32(packetCount);
                    ms.writeUInt32(rcontext->inputData()->count());
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    // send the elements.
                    uint32_t packetNo = rcontext->getPacketNo();
                    assert(packetNo < rcontext->ranges().count());
                    T* t = rcontext->ranges().at(packetNo);
                    t->write(&ms);
                }
                else
                {
                    return false;
                }
                
                return true;
            }
            return false;
        }
        bool setCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            UploadPacketContext<T, P, K>* rcontext = dynamic_cast<UploadPacketContext<T, P, K>*>(context);
            if (rcontext != NULL)
            {
                ms.readByte();	// skip command
                readVersion(&ms);
                
                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->status = ms.readByte();
                    if(rcontext->status == StatusContext::Succeed)
                    {
                        rcontext->outputData()->read(&ms);
                    }
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    rcontext->status = ms.readByte();
                }
                else
                {
                    return false;
                }
                return true;
            }
            return false;
        }
        
    protected:
        virtual ClientContext* setValue(ClientContext* context)
        {
            return context;
        }
        
        bool syncSendReceive(Interactive*) const override
        {
            return _syncSendReceive;
        }
        
//    private:
//        uint32_t calcPacketCount(PacketContext<T, K>* context)
//        {
//            const T* inputData = context->inputData();
//            assert(inputData);
//            MemoryStream ms;
//            size_t count = 0;
//            _ranges.clear();
//            for (size_t i = 0; i < inputData->count(); i++)
//            {
//                P* p = inputData->at(i);
//                p->write(&ms);
//                if (ms.length() > context->packetLength())
//                {
//                    count++;
//                    // full, so seek the zero position, and write the last one.
//                    ms.clear();
//                    p->write(&ms);
//                }
//                T* t;
//                if (_ranges.count() == count)
//                {
//                    t = new T(false);
//                    _ranges.add(t);
//                }
//                else
//                {
//                    t = _ranges.at(count);
//                }
//                t->add(p);
//            }
//            return _ranges.count();
//        }
        
    private:
//        typedef PList<T> Ranges;
//
//        Ranges _ranges;
        
        bool _syncSendReceive;
    };
    // T is input data, K is EmptyContext, P is item of T.
    template <class T, class P>
    class UploadPacketEInstruction : public UploadPacketInstruction<T, EmptyContext, P>
    {
    public:
        UploadPacketEInstruction(InstructionDescription* id, bool syncSendReceive = true) : UploadPacketInstruction<T, EmptyContext, P>(id, syncSendReceive)
        {
        }
        ~UploadPacketEInstruction() override
        {
        }
    };
    
    class FileInstructionEntry
    {
    public:
        FileInstructionEntry();
        virtual ~FileInstructionEntry();
        
        bool saveFile();
        
    public:
        static FileInstructionEntry* instance();
        
    protected:
        bool saveFile(const FileHeader* header, const FileDatas* fds);
        bool saveFile(const FileHeader* header, const FileData* fd);
        bool saveFileInner(const FileHeader* header, const FileDatas* fds);
        bool saveFileInner(const FileHeader* header, const FileData* fd);
        
        bool readFile(Stream* stream, const FileHeader* header, uint32_t packetNo, uint32_t packetLength) const;
        
    protected:
        static uint32_t calcPacketCount(FileHeader* header, uint32_t packetLength);
        
    private:
        FileHeader _header;
        FileDatas _fds;
        
    private:
        static FileInstructionEntry* _instance;
    };
    
    // T is input data.
    template <class T>
    class FileInstruction : public FileInstructionEntry
    {
    public:
        ~FileInstruction() override
        {
        }
        
    public:
        static uint32_t calcPacketCount(FileContext<T>* context)
        {
            return FileInstructionEntry::calcPacketCount(context->header(), context->packetLength());
//            FileHeader* header = context->header();
//            String fileName = Path::combine(header->path, header->file_name);
//            if(!header->path.isNullOrEmpty() && !header->file_name.isNullOrEmpty())
//            {
//                if(File::exists(fileName))
//                {
//                    if(!header->update())
//                        return 0;
//
//                    FileStream fs(fileName.c_str(), FileMode::FileOpen, FileAccess::FileRead);
//                    uint32_t packetLength = context->packetLength();
//                    assert(packetLength > 0);
//                    int64_t fileLength = fs.length();
//                    return (fileLength % packetLength == 0) ? (uint32_t)(fileLength / packetLength) : (uint32_t)(fileLength / packetLength + 1);
//                }
//                else
//                {
//                    Trace::writeFormatLine("Can not find file in file communication, name: %s", fileName.c_str());
//                }
//            }
//            return 0;
        }
        
    protected:
        bool saveFile(const FileContext<T>* context)
        {
            return FileInstructionEntry::saveFile(context->header(), context->outputData());
        }
        
        bool readFile(FileContext<T>* context, Stream* stream) const
        {
            return FileInstructionEntry::readFile(stream, context->header(), context->getPacketNo(), context->packetLength());
//            const FileHeader* header = rcontext->header();
//            String fileName = Path::combine(header->path, header->file_name);
//            if(File::exists(fileName))
//            {
//                FileStream fs(fileName.c_str(), FileMode::FileOpen, FileAccess::FileRead);
//                if(!fs.isOpen())
//                {
//                    Debug::writeFormatLine("FileInstruction::readFile, can not open file, file name: %s", fileName.c_str());
//                    return false;
//                }
//                uint32_t packetNo = rcontext->getPacketNo();
//                uint32_t position = packetNo * rcontext->packetLength();
//                fs.seek(position);
//                uint32_t fileLength = header->fileLength;
//                uint32_t lastLength = fileLength - position;
//                uint32_t dataLength = rcontext->packetLength() <= lastLength ? rcontext->packetLength() : lastLength;
//                uint8_t* buffer = new uint8_t[dataLength];
//                memset(buffer, 0, dataLength);
//                fs.read(buffer, 0, dataLength);
//
//                FileDatas data;
//                FileData* item = new FileData();
//                item->packetNo = packetNo;
//                item->data.addRange(buffer, dataLength);
//                delete[] buffer;
//                data.add(item);
//                data.write(stream);
//
//                return true;
//            }
//            else
//            {
//                Debug::writeFormatLine("FileInstruction::readFile, can not find file, file name: %s", fileName.c_str());
//            }
//            return false;
        }
    };
    // T is input data.
    template <class T>
    class DownloadFileInstruction : public ClientInstruction, public FileInstruction<T>
    {
    public:
        DownloadFileInstruction(InstructionDescription* id) : ClientInstruction(id)
        {
        }
        ~DownloadFileInstruction() override
        {
        }
        
        bool getCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            FileContext<T>* rcontext = dynamic_cast<FileContext<T>*>(context);
            if (rcontext != NULL)
            {
                ms.writeByte(command());
                writeVersion(&ms);
                
                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                ms.writeByte(rcontext->transfer());
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->inputData()->write(&ms);
                    ms.writeUInt32(rcontext->packetLength());
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    rcontext->header()->write(&ms);
                    ms.writeUInt32(rcontext->getPacketNo());
                }
                else
                {
                    return false;
                }
                
                return true;
            }
            return false;
        }
        bool setCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            FileContext<T>* rcontext = dynamic_cast<FileContext<T>*>(context);
            if (rcontext != NULL)
            {
                ms.readByte();	// skip command
                readVersion(&ms);
                
                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData);
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->status = ms.readByte();
                    if(rcontext->status == StatusContext::Succeed)
                    {
                        rcontext->setPacketCount(ms.readUInt32());
                        rcontext->header()->read(&ms);
                    }
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    rcontext->outputData()->read(&ms);
                    this->saveFile(rcontext);
                }
                else
                {
                    return false;
                }
                
                return setValue(rcontext) != NULL;
            }
            return false;
        }
        
        virtual ClientContext* setValue(ClientContext* context)
        {
            return context;
        }
        
    protected:
        bool syncSendReceive(Interactive*) const override
        {
            return true;
        }
        
        bool allowLogMessage() const override
        {
            return false;
        }
    };

    // T is input data.
    template <class T>
    class DownloadFileCInstruction : public DownloadFileInstruction<T>
    {
    public:
        DownloadFileCInstruction(InstructionDescription* id, receive_callback callback = nullptr, void* owner = nullptr) : DownloadFileInstruction<T>(id), _callback(callback), _owner(owner)
        {
        }
        ~DownloadFileCInstruction() override
        {
        }
        
        ClientContext* setValue(ClientContext* context) override
        {
            if(_callback != nullptr)
                _callback(_owner, context);
            
            return context;
        }
        
    private:
        receive_callback _callback;
        void* _owner;
    };
    
    // T is input data.
    template <class T>
    class UploadFileInstruction : public ClientInstruction, public FileInstruction<T>
    {
    public:
        UploadFileInstruction(InstructionDescription* id, bool syncSendReceive = true) : ClientInstruction(id)
        {
            _syncSendReceive = syncSendReceive;
        }
        ~UploadFileInstruction() override
        {
        }
        
        bool getCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            FileContext<T>* rcontext = dynamic_cast<FileContext<T>*>(context);
            if (rcontext != NULL)
            {
                setValue(rcontext);
                
                ms.writeByte(command());
                writeVersion(&ms);

                uint8_t transfer = rcontext->transfer();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData ||
                       transfer == BasePacketContext::TransferStatus);
                ms.writeByte(transfer);
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->inputData()->write(&ms);
                    uint32_t packetCount = this->calcPacketCount(rcontext);
                    rcontext->setPacketCount(packetCount);
                    rcontext->header()->write(&ms);
                    return true;
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    // send the elements.
                    if(this->readFile(rcontext, &ms))
                    {
                        return true;
                    }
                }
                else if(transfer == BasePacketContext::TransferStatus)
                {
                    ms.writeByte(rcontext->status);
                    rcontext->inputData()->write(&ms);
                    return true;
                }
                return false;
            }
            return false;
        }
        bool setCommandBuffer(MemoryStream& ms, ClientContext* context) override
        {
            FileContext<T>* rcontext = dynamic_cast<FileContext<T>*>(context);
            if (rcontext != NULL)
            {
                ms.readByte();	// skip command
                readVersion(&ms);
                
                uint8_t transfer = ms.readByte();
                assert(transfer == BasePacketContext::TransferHeader ||
                       transfer == BasePacketContext::TransferData ||
                       transfer == BasePacketContext::TransferStatus);
                if(transfer == BasePacketContext::TransferHeader)
                {
                    rcontext->status = ms.readByte();
                }
                else if(transfer == BasePacketContext::TransferData)
                {
                    rcontext->status = ms.readByte();
                }
                else if(transfer == BasePacketContext::TransferStatus)
                {
                    rcontext->status = ms.readByte();
                }
                else
                {
                    return false;
                }                
                return true;
            }
            return false;
        }
        
        virtual ClientContext* setValue(ClientContext* context)
        {
            return context;
        }
        
    protected:
        bool syncSendReceive(Interactive*) const override
        {
            return _syncSendReceive;
        }
        
    private:
        bool _syncSendReceive;
    };
}

#endif // ClientInstruction_h
