#ifndef BLUETOOTHCHANNELCONTEXT_H
#define BLUETOOTHCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "data/Convert.h"
#include "data/TimeSpan.h"

using namespace Common;

namespace Drivers
{
	class BluetoothChannelBaseContext : public ChannelContext
	{
	public:
		BluetoothChannelBaseContext()
		{
			_recvBufferSize = 0;
			_sendBufferSize = 0;
            _closeTimeout = TimeSpan(0, 1, 0);
            _asyncReceiver = true;
            _asyncSender = true;
		}
		~BluetoothChannelBaseContext() override
        {
        }

		inline int sendBufferSize() const
		{
			return _sendBufferSize;
		}
		inline void setSendBufferSize(int bufferSize)
		{
			_sendBufferSize = bufferSize;
		}
		inline int receiveBufferSize() const
		{
			return _recvBufferSize;
		}
		inline void setReceiveBufferSize(int bufferSize)
		{
			_recvBufferSize = bufferSize;
		}
        
        inline const TimeSpan& closeTimout() const
        {
            return _closeTimeout;
        }
        inline void setCloseTimeout(const TimeSpan& timeout)
        {
            _closeTimeout = timeout;
        }
		inline void setCloseTimeout(uint milliSeconds)
        {
            _closeTimeout = TimeSpan::fromMilliseconds((double)milliSeconds);
        }
        
        virtual void copyFrom(const BluetoothChannelBaseContext* context)
        {
            _recvBufferSize = context->_recvBufferSize;
            _sendBufferSize = context->_sendBufferSize;
            
            _closeTimeout = context->_closeTimeout;
            _asyncReceiver = context->_asyncReceiver;
            _asyncSender = context->_asyncSender;
        }
        
        bool asyncReceiver() const
        {
            return _asyncReceiver;
        }
        void setAsyncReceiver(bool asyncReceiver)
        {
            _asyncReceiver = asyncReceiver;
        }
        
        bool asyncSender() const
        {
            return _asyncReceiver;
        }
        void setAsyncSender(bool asyncSender)
        {
            _asyncSender = asyncSender;
        }

	protected:
        int _recvBufferSize;
		int _sendBufferSize;

   		TimeSpan _closeTimeout;
        
        bool _asyncReceiver;
        bool _asyncSender;
	};
    
    class BluetoothChannelContext : public BluetoothChannelBaseContext
    {
    public:
        BluetoothChannelContext() : BluetoothChannelBaseContext()
        {
            _openTimeout = 3000;
        }
        ~BluetoothChannelContext() override
        {
        }
        
        inline uint getOpenTimeout() const
        {
            return _openTimeout;
        }
        inline void setOpenTimeout(uint timeout)
        {
            _openTimeout = timeout;
        }
        inline void setOpenTimeout(TimeSpan timeout)
        {
            _openTimeout = (uint)timeout.totalMilliseconds();
        }
        
        void copyFrom(const BluetoothChannelBaseContext* context) override
        {
            BluetoothChannelBaseContext::copyFrom(context);
            
            const BluetoothChannelContext* tc = dynamic_cast<const BluetoothChannelContext*>(context);
            if(tc != NULL)
            {
                _openTimeout = tc->_openTimeout;
            }
        }
        
    private:
        uint _openTimeout;
    };
}

#endif // BLUETOOTHCHANNELCONTEXT_H
