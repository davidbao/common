#ifndef RECEIVER_H
#define RECEIVER_H

#include "data/ValueType.h"
#include "data/TimeSpan.h"
#include "EscapeOption.h"

using namespace Common;

namespace Net
{
	class Receiver
	{
	public:
		Receiver();
		virtual ~Receiver();
        
        virtual bool connected() = 0;
        virtual bool useReceiveTimeout() const = 0;
        virtual size_t available() = 0;
        
        virtual ssize_t receive(uint8_t* buffer, off_t offset, size_t count) = 0;
        virtual ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout) = 0;
        virtual ssize_t receive(ByteArray* buffer, size_t count, uint32_t timeout) = 0;
        ssize_t receive(size_t count, ByteArray& buffer);
        
        virtual void clearReceiveBuffer();

        ssize_t receiveByEndBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* endBuffer, size_t ebLength, int suffix, uint32_t timeout);
        ssize_t receiveByEndBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* startBuffer, size_t sbLength, const uint8_t* endBuffer, size_t ebLength, int suffix, uint32_t timeout);
        ssize_t readLine(uint8_t* buffer, size_t bufferLength, uint32_t timeout, const char* newLine = "\n");

        ssize_t receiveBySize(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint32_t timeout, const EscapeOption* escape = nullptr);
        ssize_t receiveBySize(uint8_t* buffer, size_t bufferLength, size_t size, uint32_t timeout);
        ssize_t receiveBySize(uint8_t* buffer, size_t bufferLength, size_t size, const TimeSpan& timeout);

        ssize_t receiveBySize(ByteArray* buffer, size_t count, uint32_t timeout, const EscapeOption* escape = nullptr);
        ssize_t receiveBySize(ByteArray* buffer, size_t count, const TimeSpan& timeout, const EscapeOption* escape = nullptr);
        
        bool receiveByte(uint8_t& data, uint32_t timeout);

        ssize_t getLengthByLine(uint32_t timeout, const char* newLine = "\n");
        
    private:
        ssize_t getLengthByEndBytes(const ByteArray& endBuffer, uint32_t timeout);
        
        bool receiveStartBytes(const uint8_t* startBuffer, size_t sbLength, uint32_t timeout);
        bool receiveStartBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* startBuffer, size_t sbLength, uint32_t timeout);
        ssize_t receiveBySizeWithEscape(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint32_t timeout, const EscapeOption* escape = nullptr);
        ssize_t receiveBySizeWithoutEscape(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint32_t timeout);
        
        ssize_t receiveBySizeWithoutEscape(ByteArray* buffer, size_t count, uint32_t timeout);
        ssize_t receiveDirectly(ByteArray* buffer, size_t count);
        
    public:
        static const size_t BufferLength = 65535;
	};
};
#endif //RECEIVER_H
