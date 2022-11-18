#ifndef CHANNEL_H
#define CHANNEL_H

#include "data/ByteArray.h"
#include "data/Convert.h"
#include "exception/Exception.h"
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "system/Delegate.h"
#include "net/Receiver.h"
#include "../IContextProperty.h"
#include "Interactive.h"
#include "ChannelDescription.h"
#include "net/Sender.h"
#include "net/Receiver.h"

namespace Drivers
{
	class Interactive;
    class DriverManager;
    class MockInteractive;
    class Channel : public Sender, public Receiver
	{
	public:
        class OpenedEventArgs : public EventArgs
        {
        public:
            OpenedEventArgs(bool succeed)
            {
                this->succeed = succeed;
            }
            
            bool succeed;
        };
        
		Channel(DriverManager* dm, ChannelDescription* description);
		~Channel() override;

        bool connected() override;
        bool useReceiveTimeout() const override;
        size_t available() override;
        
        ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
        
        ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;
        ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout) override;
        ssize_t receive(ByteArray* buffer, size_t count, uint32_t timeout) override;
        
        bool opened() const;

        const String& name() const;
        ChannelDescription* description() const;
        ChannelContext* context() const;

        Interactive* interactive() const;
        
        bool enabled() const;
			
		bool open();
        void openAsync();
		void close();
        bool reopen();
        void reopenAsync();
        
        bool reopened() const;

        void updateMockRecvBuffer(const ByteArray& buffer);

        Delegates* openedDelegates();
        Delegates* closedDelegates();
        
	private:
        DriverManager* manager();
        
        static void openAction(ThreadHolder* holder);
        static void reopenAction(ThreadHolder* holder);
        
        static bool isOpened(void* parameter);
        
	private:
		ChannelDescription* _description;
		Interactive* _interactive;
        
        DriverManager* _manager;
        
        MockInteractive* _mock;
        
        bool _opened;
        bool _opening;

        Delegates _openedDelegates;
        Delegates _closedDelegates;
	};
    typedef PList<Channel> Channels;
}

#endif // CHANNEL_H
