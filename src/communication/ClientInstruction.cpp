#include "net/Dns.h"
#include "diag/Trace.h"
#include "diag/Trace.h"
#include "IO/MemoryStream.h"
#include "system/BCDUtilities.h"
#include "system/CheckUtilities.h"
#include "driver/devices/Device.h"
#include "driver/channels/Channel.h"
#include "driver/channels/TcpInteractive.h"
#include "driver/channels/TcpServerInteractive.h"
#include "driver/channels/UdpServerInteractive.h"
#include "system/Resources.h"
#include "communication/ClientInstruction.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif  // __EMSCRIPTEN__

using namespace Common;

namespace Communication
{
    struct ClientInstructionHolder
    {
        ClientInstruction* ci;
        TcpInteractive* ti;
        
        ClientInstructionHolder(ClientInstruction* ci, TcpInteractive* ti)
        {
            this->ci = ci;
            this->ti = ti;
        }
    };
    
    char ClientInstruction::_frameId = -1;
    ClientInstruction::ClientInstruction(InstructionDescription* id) : Instruction(id)
    {
        _currentFrameId = 0;
        _isSender = false;
        
        _version = Version::Verson1_0;
    }
    ClientInstruction::~ClientInstruction()
    {
    }
    InstructionContext* ClientInstruction::execute(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer)
    {
#ifdef __EMSCRIPTEN__
        return executeAsync(interactive, device, context, buffer);
#else
        if(syncSendReceive(interactive))
        {
            return executeSync(interactive, device, context, buffer);
        }
        else
        {
            return executeAsync(interactive, device, context, buffer);
        }
#endif
    }
    InstructionContext* ClientInstruction::executeAsync(Interactive* interactive, Device* device, InstructionContext* context)
    {
        return executeAsync(interactive, device, context, nullptr);
    }

    InstructionContext* ClientInstruction::send(Interactive* interactive, Device* device, InstructionContext* context, uint8_t frameId)
    {
        if (interactive == nullptr)
        {
            return nullptr;
        }
        
        Channel* channel = interactive->channel();
        if (channel == nullptr)
        {
            return nullptr;
        }
        ClientContext* ic = dynamic_cast<ClientContext*>(context);
        EthernetEndpoint* ea = dynamic_cast<EthernetEndpoint*>(interactive);
        if(ea != nullptr)
        {
            ic->setPeerEndpoint(ea->peerEndpoint());
        }
        
        MemoryStream ms;
        if(!getSendBuffer(ms, ic, frameId))
        {
            Debug::writeLine("Can not get send buffer in ClientInstruction.", Trace::Error);
            return nullptr;
        }
        
//        channel->clearReceiveBuffer();
        int sendLen = channel->Sender::send(*ms.buffer());
        if(sendLen <= 0)
        {
//            Debug::writeLine("Clear the receive buffer.");
//            channel->clearReceiveBuffer();
            
#ifdef DEBUG
            const ByteArray* buffer = ms.buffer();
            String str;
            size_t logLength = 128;
            size_t count = buffer->count();
            if(count > logLength)
            {
                ByteArray array(buffer->data(), Math::min(logLength, count));
                str = array.toString();
            }
            else
            {
                str = buffer->toString();
            }
            Debug::writeFormatLine("Can not send the instruction! device name: %s, send buffer: %s",
                                   device->name().c_str(), str.c_str());
#endif
            return nullptr;
        }
        
        if(sendLen > 0)
            log(ms.buffer(), sendLen, true, ic->peerEndpoint());
        
        return context;
    }
    InstructionContext* ClientInstruction::receive(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer)
    {
        if (interactive == nullptr)
        {
            return nullptr;
        }
        if(buffer == nullptr)
        {
            return nullptr;
        }
        
        Channel* channel = interactive->channel();
        if (channel == nullptr)
        {
            return nullptr;
        }
        
        if (buffer->count() < ClientContext::MinFrameLen)
        {
            return nullptr;
        }
        
        ClientContext* ic = dynamic_cast<ClientContext*>(context);
        EthernetEndpoint* ea = dynamic_cast<EthernetEndpoint*>(interactive);
        if(ea != nullptr)
        {
            ic->setPeerEndpoint(ea->peerEndpoint());
        }
        
        log(buffer, false, ic->peerEndpoint());
        
        MemoryStream ms((ByteArray*)buffer, false);
        InstructionContext* result = setReceiveBuffer(ms, ic, false);

		if (result != nullptr)
        {
            if(autoResponsed() && !_isSender)
            {
                // send back.
                uint8_t frameId = (*buffer)[ClientContext::FrameIdPosition];
                send(interactive, device, context, frameId);
            }
        }
        return result;
    }
    InstructionContext* ClientInstruction::receiveFromBuffer(Interactive* interactive, Device* device, InstructionContext* context)
    {
        if (interactive == nullptr)
        {
            return nullptr;
        }

        ByteArray buffer;
        if(hasBackgroudReceiver(interactive))
        {
            ClientInstruction* ci = dynamic_cast<ClientInstruction*>(_receiveInstruction);
            if(ci != nullptr)
            {
#ifdef DEBUG
                String info = String::format("ClientInstruction::receiveFromBuffer, device name: %s, instruction name: %s",
                                             device->name().c_str(), ci->description()->name().c_str());
                Stopwatch sw(info, 1000);
#endif
                uint timeout = device->description()->receiveTimeout(context);
                ClientInstructionHolder holder(ci, dynamic_cast<TcpInteractive*>(interactive));
                if(TickTimeout::msdelay(timeout, isReceiveFinished, &holder))
                    ci->copyReceiveBuffer(buffer);
                else
                {
                    // timeout
//                    Debug::writeFormatLine("ClientInstruction::receiveFromBuffer.timeout, instruction name: %s, timeout: %d ms", description()->name().c_str(), timeout);
                    return nullptr;
                }
            }
        }
        else
        {
            if(!device->receive(&buffer))
                return nullptr;
        }
        
        ClientContext* ic = dynamic_cast<ClientContext*>(context);
        EthernetEndpoint* ea = dynamic_cast<EthernetEndpoint*>(interactive);
        if(ea != nullptr)
        {
            ic->setPeerEndpoint(ea->peerEndpoint());
        }
        log(&buffer, false, ea->peerEndpoint());
        
        MemoryStream ms(&buffer, false);
        ClientContext* result = setReceiveBuffer(ms, ic, true);
        
        return result;
    }
    bool ClientInstruction::isReceiveFinished(void* parameter)
    {
        if(parameter != nullptr)
        {
            ClientInstructionHolder* holder = (ClientInstructionHolder*)parameter;
            ClientInstruction* ci = holder->ci;
            TcpInteractive* ti = holder->ti;
            if(ti != nullptr)
            {
                if(!ti->connected())
                    return true;
            }
            return ci->isReceiveFinishedInner();
        }
        return false;
    }
    bool ClientInstruction::isReceiveFinishedInner()
    {
#ifdef DEBUG
        Stopwatch sw("ClientInstruction::isReceiveFinishedInner", 1000);
#endif
        Locker locker(&_backgroundBufferMutex);
        return _backgroundBuffer.count() > 0;
    }
    void ClientInstruction::copyReceiveBuffer(ByteArray& buffer)
    {
#ifdef DEBUG
        Stopwatch sw("ClientInstruction::copyReceiveBuffer", 1000);
#endif
        Locker locker(&_backgroundBufferMutex);
        buffer.addRange(_backgroundBuffer);
        _backgroundBuffer.clear();
    }
    InstructionContext* ClientInstruction::executeAsync(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer)
    {
        if(buffer == nullptr)   // sent
        {
            return send(interactive, device, context);
        }
        else                    // received.
        {
            return receive(interactive, device, context, buffer);
        }
    }
    InstructionContext* ClientInstruction::executeSync(Interactive* interactive, Device* device, InstructionContext* context, const ByteArray* buffer)
    {
        if(buffer == nullptr)
        {
            send(interactive, device, context);
        
            return receiveFromBuffer(interactive, device, context);
        }
        else
        {
#ifdef DEBUG
            Stopwatch sw("ClientInstruction::executeSync_buffer_is_not_null", 1000);
#endif
            Locker locker(&_backgroundBufferMutex);
            _backgroundBuffer.clear();
            _backgroundBuffer.addRange(*buffer);
            return context;
        }
    }
    bool ClientInstruction::getSendBuffer(MemoryStream& ms, ClientContext* context, uint8_t frameId)
    {
        ms.writeByte(ClientContext::Header);
        ms.writeByte(frameId == 0xFF ? getFrameId() : frameId);
        ms.writeByte(0);    // state is ok
        const int lengthCount = ClientContext::BufferBCDLength;
        uint8_t lengthBuffer[lengthCount];
        memset(lengthBuffer, 0, lengthCount);
        ms.write(lengthBuffer, 0, lengthCount);
        
        if (!getCommandBuffer(ms, context))
        {
            Debug::writeLine("Can not get command buffer in ClientInstruction.", Trace::Error);
            return false;
        }
        
        int64_t position = ms.position();
        BCDUtilities::Int64ToBCD(ms.length() - ClientContext::LengthPosition - lengthCount + 2, lengthBuffer, lengthCount);
        ms.seek(ClientContext::LengthPosition);
        ms.write(lengthBuffer, 0, lengthCount);
        
        ushort crc16 = Crc16Utilities::quickCheckByBit(ms.buffer()->data(), 1, (int)(ms.length() - 1));
        ms.seek(position);
        ms.writeUInt16(crc16);
        
        return true;
    }
    void ClientInstruction::getSendBuffer(MemoryStream& ms, uint8_t frameId, uint8_t state, uint8_t command)
    {
        ms.writeByte(ClientContext::Header);
        ms.writeByte(frameId);
        ms.writeByte(state);    // state
        const int lengthCount = ClientContext::BufferBCDLength;
        uint8_t lengthBuffer[lengthCount];
        memset(lengthBuffer, 0, lengthCount);
        ms.write(lengthBuffer, 0, lengthCount);
        
        ms.writeByte(command);
        Version version(1, 0);
        version.writeByte(&ms);
        
        int64_t position = ms.position();
        BCDUtilities::Int64ToBCD(ms.length() - ClientContext::LengthPosition - lengthCount + 2, lengthBuffer, lengthCount);
        ms.seek(ClientContext::LengthPosition);
        ms.write(lengthBuffer, 0, lengthCount);
        
        ushort crc16 = Crc16Utilities::quickCheckByBit(ms.buffer()->data(), 1, (int)(ms.length() - 1));
        ms.seek(position);
        ms.writeUInt16(crc16);
    }
    ClientContext* ClientInstruction::setReceiveBuffer(MemoryStream& ms, ClientContext* context, bool checkFrameId)
    {
        if (context == nullptr)
        {
            return nullptr;
        }
        
        const ByteArray* buffer = ms.buffer();
        if (!match(buffer, nullptr))
        {
            return nullptr;
        }
        if (checkFrameId && !isSameFrameId((*buffer)[ClientContext::FrameIdPosition]))
        {
#ifdef DEBUG
            String info = String::convert("The frame id is not same between sender and receiver, current frame id: %x", _currentFrameId);
            Debug::writeLine(info.c_str(), Trace::Error);
            logDirectly(buffer, buffer->count(), false);
#endif
            return nullptr;
        }
        
        if((*buffer)[ClientContext::StatePosition] != 0)
        {
            context->setState((*buffer)[ClientContext::StatePosition]);
            return context;
        }
        
        ms.seek(ClientContext::CommandPosition);
        if (setCommandBuffer(ms, context))
        {
            return context;
        }
        return nullptr;
    }
    bool ClientInstruction::match(const ByteArray* buffer, DeviceDescription* dd)
    {
        if(!canMatch())
            return false;
        
        if(buffer == nullptr)
            return false;

        size_t count = buffer->count();
        if (count == 0)
        {
            return false;
        }
        if (count < ClientContext::MinFrameLen)	// The shortest instruction.
        {
            return false;
        }
        if ((*buffer)[ClientContext::HeaderPosition] != ClientContext::Header)
        {
            return false;
        }
        if (command() != (*buffer)[ClientContext::CommandPosition])
        {
            return false;
        }
        if(buffer->count() <= 10 * 1024)
        {
            ushort crc16 = Crc16Utilities::quickCheckByBit(buffer->data(), 1, count - 1 - 2);
            if (!((*buffer)[count - 2] == ((crc16 >> 8) & 0xFF) &&
                  (*buffer)[count - 1] == (crc16 & 0xFF)))
            {
                return false;
            }
        }
        return true;
    }
    bool ClientInstruction::autoResponsed() const
    {
        return false;
    }
    bool ClientInstruction::syncSendReceive(Interactive* interactive) const
    {
        return !hasBackgroudReceiver(interactive);
    }
    bool ClientInstruction::canMatch() const
    {
        return true;
    }
    const Version& ClientInstruction::version() const
    {
        return _version;
    }
    void ClientInstruction::setVersion(const Version& version)
    {
        _version = version;
    }
    void ClientInstruction::writeVersion(Stream* stream)
    {
        const Version& ver = version();
        ver.writeByte(stream);
        stream->setVersion(ver);
    }
    void ClientInstruction::readVersion(Stream* stream)
    {
        Version ver;
        ver.readByte(stream);
        stream->setVersion(ver);
    }
    bool ClientInstruction::hasBackgroudReceiver(Interactive* interactive) const
    {
        if (interactive == nullptr)
        {
            return false;
        }
        Channel* channel = interactive->channel();
        if (channel == nullptr)
        {
            return false;
        }
        
        ChannelDescription* cd = channel->description();
        TcpChannelContext* tcc = dynamic_cast<TcpChannelContext*>(cd->context());
        return tcc != nullptr ? tcc->backgroundReceiver() : false;
    }
    void ClientInstruction::setSender(bool isSender)
    {
        _isSender = isSender;
    }
    
    uint8_t ClientInstruction::getFrameId()
    {
        _frameId++;
        _frameId &= 0x3F;
        _currentFrameId = _frameId;
        
        return (uint8_t)_currentFrameId;
    }
    bool ClientInstruction::isSameFrameId(uint8_t frameId)
    {
        return _currentFrameId == frameId;
    }
    
#ifdef __EMSCRIPTEN__
    extern "C"
    {
        void EMSCRIPTEN_KEEPALIVE ClientInstruction_saveFile()
        {
            Debug::writeLine("ClientInstruction_saveFile");
            FileInstructionEntry* fe = FileInstructionEntry::instance();
            if(fe != nullptr)
            {
                fe->saveFile();
            }
            
            // sync from memory state to persisted and then
            // run 'success'
            EM_ASM(
                FS.syncfs(function (err) {
                    //assert(!err);
                });
            );
        }
    }
#endif  // __EMSCRIPTEN__    
    
    FileInstructionEntry* FileInstructionEntry::_instance = nullptr;
    FileInstructionEntry::FileInstructionEntry()
    {
    }
    FileInstructionEntry::~FileInstructionEntry()
    {
    }

    FileInstructionEntry* FileInstructionEntry::instance()
    {
        return _instance;
    }
    bool FileInstructionEntry::saveFile()
    {
        Debug::writeFormatLine("FileInstructionEntry::saveFile, packet count: %d", _fds.count());
        bool result = saveFileInner(&_header, &_fds);
        _instance = nullptr;
        _header.clear();
        _fds.clear();
        return result;
    }

    bool FileInstructionEntry::saveFile(const FileHeader* header, const FileDatas* fds)
    {
        for (uint i=0; i<fds->count(); i++)
        {
            if(!saveFile(header, fds->at(i)))
                return false;
        }
        return true;
    }
    bool FileInstructionEntry::saveFile(const FileHeader* header, const FileData* fd)
    {
#ifdef __EMSCRIPTEN__
//        Debug::writeFormatLine("FileInstructionEntry::saveFile, packet no: %d", fd->packetNo);
        if(fd->isFirstPart())
        {
            _instance = this;
            _header.copyFrom(header);
        }
        
        _fds.add(fd->clone());
        
        if(fd->isLastPart(header))
        {
            EM_ASM(
                // sync from persisted state into memory and then
                // run the 'test' function
                FS.syncfs(false, function (err) {
                    //assert(!err);
                    ccall('ClientInstruction_saveFile');
                });
            );
        }
        return true;
#else
        return saveFileInner(header, fd);
#endif  // __EMSCRIPTEN__
    }
    bool FileInstructionEntry::saveFileInner(const FileHeader* header, const FileDatas* fds)
    {
        for (uint i=0; i<fds->count(); i++)
        {
            if(!saveFileInner(header, fds->at(i)))
                return false;
        }
        return true;
    }
    bool FileInstructionEntry::saveFileInner(const FileHeader* header, const FileData* fd)
    {
        String path = header->path;
        if (!Directory::exists(path))
        {
            if(!Directory::createDirectory(path))
                return false;
        }
        
        String filename = header->tempFullFileName();
        size_t dataCount = fd->data.count();
        bool isLastPart = fd->packetNo == header->packetCount - 1;
        int64_t position;
        if (!isLastPart)
        {
            position = fd->packetNo * dataCount;
        }
        else
        {
            // the last part.
            position = header->fileLength - dataCount;
        }
        
//#ifdef DEBUG
//        bool isFirstPart = fd->packetNo == 0;
//        if(isFirstPart || isLastPart)
//        {
//            bool fileExists = File::exists(filename);
//            const char* formatStr = "saveFile: packetNo: %d, position: %d, exists: %s, isLastPart: %s, dataCount: %d, fileLength: %d, packetCount: %d, filename: %s";
//            String str = String::convert(formatStr,
//                                         fd->packetNo, (uint)position,
//                                         Common::Boolean(fileExists).toString().c_str(),
//                                         Common::Boolean(isLastPart).toString().c_str(), dataCount,
//                                         header->fileLength, header->packetCount, header->file_name.c_str());
//            Debug::writeLine(str, Trace::Info);
//        }
//#endif
        
        FileStream fs(filename.c_str(), FileMode::FileOpenOrCreate);
        if(!fs.isOpen())
            return false;
        fs.seek(position);
        fs.write(fd->data.data(), 0, dataCount);
        fs.close();
        
        //            MappingStream ms(filename.c_str(), header->fileLength);
        //            ms.seek(position);
        //            ms.write(fd->data.data(), 0, dataCount);
        //            ms.close();
        
#ifdef DEBUG
        Common::FileInfo fi(filename.c_str());
        assert(!fi.isReadOnly());
#endif
        //            // check md5
        //            if (!header->checkmd5(filename))
        //                return false;
        return true;
    }
    
    bool FileInstructionEntry::readFile(Stream* stream, const FileHeader* header, uint packetNo, uint packetLength) const
    {
        String fileName = Path::combine(header->path, header->file_name);
        if(File::exists(fileName))
        {
            FileStream fs(fileName.c_str(), FileMode::FileOpen, FileAccess::FileRead);
            if(!fs.isOpen())
            {
                Debug::writeFormatLine("FileInstruction::readFile, can not open file, file name: %s", fileName.c_str());
                return false;
            }
            uint position = packetNo * packetLength;
            fs.seek(position);
            uint fileLength = header->fileLength;
            uint lastLength = fileLength - position;
            uint dataLength = packetLength <= lastLength ? packetLength : lastLength;
            uint8_t* buffer = new uint8_t[dataLength];
            memset(buffer, 0, dataLength);
            fs.read(buffer, 0, dataLength);
            
            FileDatas data;
            FileData* item = new FileData();
            item->packetNo = packetNo;
            item->data.addRange(buffer, dataLength);
            delete[] buffer;
            data.add(item);
            data.write(stream);
            
            return true;
        }
        else
        {
            Debug::writeFormatLine("FileInstruction::readFile, can not find file, file name: %s", fileName.c_str());
        }
        return false;
    }
    
    uint FileInstructionEntry::calcPacketCount(FileHeader* header, uint packetLength)
    {
        String fileName = Path::combine(header->path, header->file_name);
        if(!header->path.isNullOrEmpty() && !header->file_name.isNullOrEmpty())
        {
            if(File::exists(fileName))
            {
                if(!header->update())
                    return 0;
                
                FileStream fs(fileName.c_str(), FileMode::FileOpen, FileAccess::FileRead);
                assert(packetLength > 0);
                int64_t fileLength = fs.length();
                return (fileLength % packetLength == 0) ? (uint)(fileLength / packetLength) : (uint)(fileLength / packetLength + 1);
            }
            else
            {
                Trace::writeFormatLine("Can not find file in file communication, name: %s", fileName.c_str());
            }
        }
        return 0;
    }
}
