#ifndef MOCKINTERACTIVE_H
#define MOCKINTERACTIVE_H

#include "IO/MemoryStream.h"
#include "Interactive.h"
#include "ChannelDescription.h"

using namespace Data;

namespace Drivers
{
    class MockChannelContext : public ChannelContext
    {
    public:
        MockChannelContext();
        ~MockChannelContext() override;
    };
    
    class MockInteractive : public Interactive
    {
    public:
        MockInteractive(DriverManager* dm, Channel* channel);
        ~MockInteractive() override;
        
        bool open() override;
        void close() override;
        
        bool connected() override;
        size_t available() override;
        
        ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
        ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;
        ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout) override;
        
        void updateRecvBuffer(const ByteArray& buffer);
        
    private:
        MockChannelContext* getChannelContext();
        
    private:
        MemoryStream _stream;
    };
}

#endif //MOCKINTERACTIVE_H
