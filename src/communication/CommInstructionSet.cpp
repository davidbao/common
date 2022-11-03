#include "system/BCDUtilities.h"
#include "system/Application.h"
#include "driver/channels/Channel.h"
#include "driver/devices/Device.h"
#include "communication/CommInstructionSet.h"
#include "driver/channels/TcpInteractive.h"

#ifndef WIN32
#include <netinet/in.h>
#endif

namespace Communication
{
    TcpInstructionSet::TcpInstructionSet(void* owner, instructions_callback action) : _buffer(4096)
    {
        _owner = owner;
        _instructionsCallback = action;
    }
    TcpInstructionSet::~TcpInstructionSet()
    {
    }
    
    void TcpInstructionSet::generateInstructions(Instructions* instructions)
    {
        if(_instructionsCallback != nullptr)
        {
            _instructionsCallback(_owner, instructions);
        }
    }
    
    bool TcpInstructionSet::receive(Device* device, Channel* channel, ByteArray* buffer, int order)
    {
        if (channel == nullptr || !channel->connected())
        {
            Debug::writeLine("The channel is disconnected!", Trace::Error);
            return false;
        }
        
        uint timeout = device->description()->receiveTimeout();
        
        int headerLength = channel->receiveBySize(buffer, ClientContext::HeaderLength, timeout);
        if (ClientContext::HeaderLength != headerLength ||
            (*buffer)[ClientContext::HeaderPosition] != ClientContext::Header)
        {
#ifdef DEBUG
            TcpInteractive* ti = dynamic_cast<TcpInteractive*>(channel->interactive());
            String address = ti != nullptr ? ti->peerEndpoint().toString() : String::Empty;
            ByteArray messages(buffer->data(), Math::min((int)buffer->count(), 128));
            Debug::writeFormatLine("tcp(%s) receive1 error: length is incorrect! expected length: %d, recv: %s",
                                   address.c_str(),
                                   ClientContext::HeaderLength,
                                   messages.toString().c_str());
#endif
            return false;
        }
        
        int validLength = (int)BCDUtilities::BCDToInt64(buffer->data(), ClientContext::LengthPosition, ClientContext::BufferBCDLength);
        if(validLength > ClientContext::MaxValidLength)
        {
#ifdef DEBUG
            TcpInteractive* ti = dynamic_cast<TcpInteractive*>(channel->interactive());
            String address = ti != nullptr ? ti->peerEndpoint().toString() : String::Empty;
            ByteArray messages(buffer->data(), Math::min((int)buffer->count(), 128));
            Debug::writeFormatLine("tcp(%s) receive2 error: length is incorrect! expected length: %d, recv: %s",
                                   address.c_str(),
                                   validLength,
                                   messages.toString().c_str());
#endif
            return false;
        }
        
        int dataLen = channel->receiveBySize(buffer, validLength, timeout);
        if (validLength != dataLen)
        {
#ifdef DEBUG
            ByteArray messages(buffer->data(), Math::min((int)buffer->count(), 128));
            Debug::writeFormatLine("tcp receive3 error: length is incorrect! expected length: %d, recv: %s", validLength, messages.toString().c_str());
#endif
            return false;
        }
        return true;
    }

    bool TcpInstructionSet::recombine(const ByteArray& origin, PList<ByteArray>& buffers)
    {
#ifdef DEBUG
        Stopwatch sw("TcpInstructionSet::recombine", 1000);
#endif
//#ifdef DEBUG
//        Application* app = Application::instance();
//        if(app->name() == "tcp-gateway")
//        {
//            static int count = 0;
//            String path = Path::combine(app->logPath(), "receives").c_str();
//            if(!Directory::exists(path))
//                Directory::createDirectory(path);
//            String fileName = String::format("%s/%04d_%s.bin", path.c_str(), ++count, DateTime::now().toString("yyyy-MM-dd HHmmss.fff").c_str());
//            FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
//            fs.write(origin.data(), 0, origin.count());
//            fs.close();
//        }
//#endif

        _bufferMutex.lock();
        MemoryStream* stream;
        if(_buffer.count() > 0)
        {
            _buffer.addRange(origin);
            stream = new MemoryStream(&_buffer, false);
        }
        else
        {
            stream = new MemoryStream(&origin, false);
        }

        const ByteArray* buffer = stream->buffer();
        int64_t count = buffer->count();
        int64_t length = 0;
        do
        {
            int64_t postion = stream->position();
            uint8_t start = stream->readByte();
            if(start == ClientContext::Header)
            {
                stream->readByte();     // skip frame.
                stream->readByte();     // skip status.
                int validLength = stream->readBCDUInt32();
                length = ClientContext::HeaderLength + validLength;
                stream->seek(validLength, SeekOrigin::SeekCurrent);
                if(length > count)
                {
                    if(_buffer.count() == 0)
                        _buffer.addRange(origin);
                    delete stream;
                    _bufferMutex.unlock();
                    return false;
                }
                else
                {
                    int64_t remain = count - postion;
                    if(remain >= length)
                    {
                        ByteArray* array = new ByteArray(buffer->data() + postion, Math::min(length, count - postion), 4096);
                        buffers.add(array);
                        
                        if(remain == length)
                        {
                            _buffer.clear();
                            break;
                        }
                    }
                    else
                    {
                        // remain < length
                        ByteArray array(buffer->data() + postion, remain, 4096);
                        _buffer = array;
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }while(length < count);
        
        delete stream;
        _bufferMutex.unlock();
        return true;
    }

	SerialInstructionSet::SerialInstructionSet(void* owner, instructions_callback action)
	{
		_owner = owner;
		if (action == nullptr)
			throw ArgumentException("action");
		_instructionsCallback = action;
	}
	SerialInstructionSet::~SerialInstructionSet()
	{
	}

	void SerialInstructionSet::generateInstructions(Instructions* instructions)
	{
		if (_instructionsCallback != nullptr)
		{
			_instructionsCallback(_owner, instructions);
		}
	}

	bool SerialInstructionSet::receive(Device* device, Channel* channel, ByteArray* buffer, int order)
	{
		if (channel == nullptr)
		{
			return false;
		}
		if (!channel->connected())
		{
			return false;
		}

		uint timeout = device->description()->receiveTimeout();

		bool matchHeader = false;
		do
		{
			int rLength = channel->receiveBySize(buffer, 1, timeout);
			if (rLength == 1 && (*buffer)[ClientContext::HeaderPosition] == ClientContext::Header)
			{
				matchHeader = true;
				break;
			}
			else if (rLength != 1)
			{
				matchHeader = false;
				break;
			}
		} while (!matchHeader);
		if (!matchHeader)
		{
			return false;
		}

		int headerLength = channel->receiveBySize(buffer, ClientContext::HeaderLength - 1, timeout);
		if (ClientContext::HeaderLength - 1 != headerLength ||
            (*buffer)[ClientContext::HeaderPosition] != ClientContext::Header)
		{
			return false;
		}
        
		int validLength = (int)BCDUtilities::BCDToInt64(buffer->data(), ClientContext::LengthPosition, ClientContext::BufferBCDLength);
		int dataLen = channel->receiveBySize(buffer, validLength, timeout);
        if (validLength != dataLen)
        {
            ByteArray messages(buffer->data(), Math::min((int)buffer->count(), 128));
            Debug::writeFormatLine("serial receive error: length is incorrect! expected length: %d, recv: %s", validLength, messages.toString().c_str());
            return false;
        }
        
        return true;
	}
    
    UdpInstructionSet::UdpInstructionSet(void* owner, instructions_callback action)
    {
        _owner = owner;
        if(action == nullptr)
            throw ArgumentException("action");
        _instructionsCallback = action;
    }
    UdpInstructionSet::~UdpInstructionSet()
    {
    }
    
    void UdpInstructionSet::generateInstructions(Instructions* instructions)
    {
        if(_instructionsCallback != nullptr)
        {
            _instructionsCallback(_owner, instructions);
        }
    }
    
    bool UdpInstructionSet::receive(Device* device, Channel* channel, ByteArray* buffer, int order)
    {
        if (channel == nullptr)
        {
            return false;
        }
        if (!channel->connected())
        {
            return false;
        }
        
        UdpServerChannelContext* context = static_cast<UdpServerChannelContext*>(channel->context());
        assert(context != nullptr);
        context->setPeek(true);
        
        uint timeout = device->description()->receiveTimeout();
        
        int headerLength = channel->receiveBySize(buffer, ClientContext::HeaderLength, timeout);
        if (ClientContext::HeaderLength != headerLength ||
            (*buffer)[ClientContext::HeaderPosition] != ClientContext::Header)
        {
            return false;
        }
        
        context->setPeek(false);
        
        int validLength = (int)BCDUtilities::BCDToInt64(buffer->data(), ClientContext::LengthPosition, ClientContext::BufferBCDLength);
        validLength += ClientContext::HeaderLength;
        buffer->clear();
        int dataLen = channel->receiveBySize(buffer, validLength, timeout);
        if (validLength != dataLen)
        {
            ByteArray messages(buffer->data(), Math::min((int)buffer->count(), 128));
            Debug::writeFormatLine("udp receive error: length is incorrect! expected length: %d, recv: %s", validLength, messages.toString().c_str());
            return false;
        }
        
        return true;
    }
}
