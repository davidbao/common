//
//  BaseCommContext.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef BaseCommContext_h
#define BaseCommContext_h

#include "IO/MemoryStream.h"
#include "data/StringArray.h"
#include "data/Version.h"
#include "data/DateTime.h"
#include "data/StreamVector.h"
#include "data/ByteArray.h"
#include "data/NetType.h"
#include "data/PrimitiveInterface.h"
#include "driver/instructions/InstructionContext.h"

using namespace Common;
using namespace Drivers;

namespace Communication
{
    class ClientContext : public InstructionContext
    {
    public:
        ClientContext();
        ~ClientContext() override;
        
        const Endpoint& peerEndpoint() const;
        void setPeerEndpoint(const Endpoint& endpoint);
        
        uint8_t state() const;
        void setState(uint8_t state);
        
    public:
        static const int BufferBCDLength = 4;
        
        static const int Header = 0xEE;
        static const int HeaderLength = 3 + BufferBCDLength;
        static const int MinFrameLen = HeaderLength + 1 + 2;
        
        static const int HeaderPosition = 0;
        static const int FrameIdPosition = 1;
        static const int StatePosition = 2;
        static const int LengthPosition = 3;
        static const int AddressPosition = HeaderLength;
        static const int CommandPosition = HeaderLength;
        
        static const int MaxPacketLength = 5 * 1024 * 1024;    // 5 M
        static const int MaxValidLength = MaxPacketLength - HeaderLength;

    private:
        Endpoint _peerEndpoint;
        uint8_t _state;
    };
    
    // element instruction's context.
    // T is input data, K is output data.
    template <class T, class K>
    class ElementContext : public ClientContext
    {
    public:
        ElementContext() : ClientContext()
        {
        }
        ~ElementContext() override
        {
        }
        
        inline void setInputData(const T* inputData)
        {
            if (inputData != NULL)
            {
                _inputData.copyFrom(inputData);
            }
        }
        inline T* inputData() const
        {
            return (T*)&_inputData;
        }
        inline void setOutputData(const K* outputData)
        {
            if (outputData != NULL)
            {
                _outputData.copyFrom(outputData);
            }
        }
        inline K* outputData() const
        {
            return (K*)&_outputData;
        }
        
    private:
        T _inputData;
        K _outputData;
    };
    
    class StatusContext
    {
    public:
        uint8_t status;
        
        StatusContext(uint8_t status = Succeed);
        StatusContext(const StatusContext& context);
        virtual ~StatusContext();
        
        void write(Stream* stream) const;
        void read(Stream* stream);
        void copyFrom(const StatusContext* value);
        
        bool isSuccessful() const;

        void operator=(const uint8_t& value);
        void operator=(const StatusContext& value);
        bool operator==(const StatusContext& value) const;
        bool operator!=(const StatusContext& value) const;
        
    public:
        static const uint8_t Succeed = 0x00;
        static const uint8_t CommunicationError = 0xFF;
    };
    struct EmptyContext
    {
        void write(Stream* stream) const
        {
        }
        void read(Stream* stream)
        {
        }
        void copyFrom(const EmptyContext* value)
        {
        }
    };
    
    // async element instruction's context.
    // T is input data, K is EmptyContext.
    template <class T>
    class ElementAContext : public ElementContext<T, EmptyContext>
    {
    };
    // sync element instruction's context.
    // T is input data, K is StatusContext.
    template <class T>
    class ElementSContext : public ElementContext<T, StatusContext>
    {
    };
    
    // base packet context.
    class BasePacketContext
    {
    public:
        BasePacketContext();
        virtual ~BasePacketContext();
        
        size_t packetCount() const;
        void setPacketCount(size_t count);
        virtual size_t getPacketNo() const;
        void setPacketNo(size_t no);
        bool isLastPacketNo() const;
        
        void transferHeader();
        void transferData();
        void transferStatus();
        void setTransfer(uint8_t transfer);
        uint8_t transfer() const;
        bool isTransferHeader() const;
        bool isTransferData() const;
        bool isTransferStatus() const;

        uint packetLength() const;
        void setPacketLength(uint packetLength);
        
    public:
        static const uint8_t TransferHeader = 0x01;
        static const uint8_t TransferData = 0x02;
        static const uint8_t TransferStatus = 0x03;
        
        static const int MinPacketLength = 256;
        static const int DefaultPacketLength = 1024 * 64;
        static const int MaxPacketLength = 1024 * 1024 * 10;
        
    protected:
        size_t _packetLength;
        
        size_t _packetCount;
        size_t _packetNo;
        
        uint8_t _transfer;
    };

    class FileHeader
    {
    protected:
        static const int MD5_COUNT = 16;
        
    public:
        uint fileLength;
        uint8_t filemd5[MD5_COUNT];
        String file_name;
        uint packetCount;
        String path;	// use by local db, it is not necessary to use in communications.
        
        FileHeader();
        virtual ~FileHeader();
        
        virtual void write(Stream* stream) const;
        virtual void read(Stream* stream);
        virtual void copyFrom(const FileHeader* obj);
        
        bool update();
        bool checkmd5() const;
        bool checkmd5(const String& filename) const;
        String md5Str() const;
        static bool parseMd5Str(const String& file_md5, uint8_t md5[MD5_COUNT]);
        bool parseMd5Str(const String& file_md5);
        
        String fullFileName() const;
        String tempFullFileName() const;
        
        void clear();
    };
    class FileData
    {
    public:
        uint packetNo;
        ByteArray data;
        
        FileData();
        virtual ~FileData();
        
        virtual void write(Stream* stream) const;
        virtual void read(Stream* stream);
        virtual void copyFrom(const FileData* obj, bool append = false);
        FileData* clone() const;
        
        bool isFirstPart() const;
        bool isLastPart(const FileHeader* header) const;
    };
    typedef StreamVector<FileData> FileDatas;
    
    // download packet instruction's context.
    // T is input data, K is output data.
    template <class T, class K>
    class PacketContext : public ElementContext<T, K>, public BasePacketContext, public StatusContext
    {
    public:
        PacketContext(bool autoDelete = false) : ElementContext<T, K>(), BasePacketContext(), StatusContext(), _count(0)
        {
            _autoDelete = autoDelete;
        }
        ~PacketContext() override
        {
        }
        
        inline void setCount(size_t count)
        {
            _count = count;
        }
        inline size_t count() const
        {
            return _count;
        }
        
        inline bool autoDelete() const
        {
            return _autoDelete;
        }
        
    public:
        static const uint8_t PacketNotFound = 1;
        
    protected:
        uint _count;
        bool _autoDelete;
    };
    // download packet instruction's context.
    // T is input data, K is EmptyContext.
    template <class T>
    class PacketEContext : public PacketContext<T, EmptyContext>
    {
    public:
        PacketEContext() : PacketContext<T, EmptyContext>()
        {
        }
        ~PacketEContext() override
        {
        }
    };
    
    // upload packet instruction's context.
    // T is input data, K is output data.
    template <class T, class P, class K>
    class UploadPacketContext : public PacketContext<T, K>
    {
    public:
        typedef PList<T> Ranges;
        
        UploadPacketContext(bool autoDelete = false) : PacketContext<T, K>()
        {
        }
        ~UploadPacketContext() override
        {
        }
        
        uint calcPacketCount()
        {
            _ranges.clear();
            const T* inputData = this->inputData();
            assert(inputData);
            MemoryStream ms;
            size_t count = 0;
            for (uint i = 0; i < inputData->count(); i++)
            {
                P* p = inputData->at(i);
                p->write(&ms);
                if (ms.length() > this->BasePacketContext::packetLength())
                {
                    count++;
                    // full, so seek the zero position, and write the last one.
                    ms.clear();
                    p->write(&ms);
                }
                T* t;
                if (_ranges.count() == count)
                {
                    t = new T(false);
                    _ranges.add(t);
                }
                else
                {
                    t = _ranges.at(count);
                }
                t->add(p);
            }
            return _ranges.count();
        }
        
        inline bool isLastPart() const
        {
            const T* inputData = this->inputData();
            assert(inputData);
            return this->_count == inputData->count();
        }
        
        inline const Ranges& ranges() const
        {
            return _ranges;
        }

    private:
        Ranges _ranges;
    };
    // upload packet instruction's context.
    // T is input data, K is EmptyContext.
    template <class T, class P>
    class UploadPacketEContext : public UploadPacketContext<T, P, EmptyContext>
    {
    public:
        UploadPacketEContext() : UploadPacketContext<T, P, EmptyContext>()
        {
        }
        ~UploadPacketEContext() override
        {
        }
    };
    
    // download & upload file instruction's context.
    // T is input data
    template <class T>
    class FileContext : public ElementContext<T, FileDatas>, public BasePacketContext, public StatusContext
    {
    public:
        static const uint8_t FileNotFound = 1;
        static const uint8_t NoNeedDownload = 2;
        static const uint8_t Failed = 3;
        
    public:
        FileContext() : ElementContext<T, FileDatas>(), BasePacketContext(), StatusContext()
        {
        }
        ~FileContext() override
        {
        }
        
        inline void setHeader(const FileHeader* header)
        {
            _header.copyFrom(header);
        }
        inline FileHeader* header() const
        {
            return (FileHeader*)&_header;
        }
        
        inline void setPacketCount(size_t count)
        {
            _header.packetCount = count;
            BasePacketContext::setPacketCount(count);
        }
        
        inline bool isFirstPart() const
        {
            FileDatas* fdatas = ElementContext<T, FileDatas>::outputData();
            if(fdatas != nullptr)
            {
                for (uint i=0; i<fdatas->count(); i++)
                {
                    const FileData* fdata = fdatas->at(i);
                    if(fdata->packetNo == 0)
                        return true;
                }
            }
            return false;
        }
        
        inline bool isLastPart() const
        {
            FileDatas* fdatas = ElementContext<T, FileDatas>::outputData();
            if(fdatas != nullptr)
            {
                for (uint i=0; i<fdatas->count(); i++)
                {
                    const FileData* fdata = fdatas->at(i);
                    if(_header.packetCount - 1 == fdata->packetNo)
                        return true;
                }
            }
            return false;
        }
        
        inline bool isFileNotFound() const
        {
            return this->status == FileNotFound;
        }
        inline bool isNoNeedDownload() const
        {
            return this->status == NoNeedDownload;
        }
        
        inline void clear()
        {
            _header.clear();
            FileDatas* fdatas = ElementContext<T, FileDatas>::outputData();
            if(fdatas != nullptr)
            {
                fdatas->clear();
            }
        }
        
        inline uint calcPacketCount()
        {
            uint fileLength = _header.FileHeader::fileLength;
            uint packetLength = this->packetLength();
            return (fileLength / packetLength) + ((fileLength % packetLength) != 0 ? 1 : 0);
        }
        
        inline uint fileLength() const
        {
            return _header.fileLength;
        }
        inline uint filePacketCount() const
        {
            return _header.packetCount;
        }
        inline uint filePacketNo() const
        {
            FileDatas* fdatas = ElementContext<T, FileDatas>::outputData();
            if(fdatas != nullptr && fdatas->count() > 0)
            {
                return fdatas->at(0)->packetNo;
            }
            return 0;
        }
        
    private:
        FileHeader _header;
    };

    class FileCommInfo : public IEquatable<FileCommInfo>, public IEvaluation<FileCommInfo>
    {
    public:
        uint packetCount;
        uint packetNo;
        uint fileLength;
        String fileName;
        
        FileCommInfo();
        FileCommInfo(const FileCommInfo& info);
        
        bool equals(const FileCommInfo& other) const override;
        void evaluates(const FileCommInfo& other) override;
        
        bool isLastPart() const;
        
        bool isEmpty() const;
        void empty();
    };
    typedef bool (*filecomm_callback)(void*, const FileCommInfo&);
    class FileCommEntry
    {
    public:
        filecomm_callback callback;
        void* owner;
        
        FileCommEntry(filecomm_callback callback, void* owner = nullptr);
    };

    class FileCommExecution : public IEquatable<FileCommExecution>
    {
    public:
        FileCommExecution() {}
        
        virtual void execute(const FileCommInfo& info) = 0;
    };
    template <class T>
    class FileCommCallback : public FileCommExecution, public ICloneable<FileCommExecution>
    {
    public:
        typedef void (T::*Method)(const FileCommInfo&);
        
        FileCommCallback() : FileCommCallback(nullptr, nullptr)
        {
        }
        FileCommCallback(T* instance, Method method)
        {
            _instance = instance;
            _method = method;
        };
        FileCommCallback(const FileCommCallback& callback)
        {
            this->operator=(callback);
        }
        
        void operator()(const FileCommInfo& info)
        {
            if(_instance != nullptr && _method != nullptr)
                return (_instance->*_method)(info);
        };
        
        void execute(const FileCommInfo& info) override
        {
            return operator()(info);
        };
        
        FileCommExecution* clone() const override
        {
            return new FileCommCallback<T>(_instance, _method);
        }
        
        bool equals(const FileCommExecution& other) const override
        {
            const FileCommCallback* callback = dynamic_cast<const FileCommCallback*>(&other);
            assert(callback);
            return callback->operator==(*this);
        }
        
        void operator=(const FileCommCallback& value)
        {
            _instance = value._instance;
            _method = value._method;
        }
        bool operator==(const FileCommCallback& value) const
        {
            return _instance == value._instance && _method == value._method;
        }
        bool operator!=(const FileCommCallback& value) const
        {
            return !operator==(value);
        }
        
        T* instance() const
        {
            return _instance;
        }
        
    private:
        T* _instance;
        Method _method;
    };
}
#endif // BaseCommContext_h
