#ifndef SENDER_H
#define SENDER_H

#include "data/ValueType.h"
#include "data/TimeSpan.h"
#include "EscapeOption.h"

using namespace Common;

namespace Net
{
	class Sender
	{
	public:
		Sender();
		virtual ~Sender();
        
        virtual bool connected() = 0;
        virtual ssize_t send(const uint8_t* buffer, off_t offset, size_t count) = 0;

        ssize_t send(const uint8_t* buffer, off_t offset, size_t count, const EscapeOption* escape);
        ssize_t send(const ByteArray& buffer, const EscapeOption* escape = nullptr);
        
    private:
        void getEscapeBuffer(const uint8_t* buffer, off_t offset, size_t count, ByteArray& dst, const EscapeOption* escape);
	};
};
#endif //SENDER_H
