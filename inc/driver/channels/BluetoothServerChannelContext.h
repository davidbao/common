#ifndef BLUETOOTHSERVERCHANNELCONTEXT_H
#define BLUETOOTHSERVERCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "data/TimeSpan.h"
#include "BluetoothChannelContext.h"

using namespace Data;

namespace Drivers
{
	class BluetoothServerChannelContext : public BluetoothChannelBaseContext
	{
	public:
		BluetoothServerChannelContext(int maxConnections = 65535) : BluetoothChannelBaseContext()
		{
			_maxConnections = maxConnections;
		}
		~BluetoothServerChannelContext() override
		{
		}

		inline int maxConnections() const
		{
			return _maxConnections;
		}
		inline void setMaxConnections(int maxConnections)
		{
			_maxConnections = maxConnections;
		}
        
		void copyFrom(const BluetoothChannelBaseContext* context) override
        {
			BluetoothChannelBaseContext::copyFrom(context);
            
            const BluetoothServerChannelContext* tc = dynamic_cast<const BluetoothServerChannelContext*>(context);
            _maxConnections = tc->_maxConnections;
        }

	private:
		int _maxConnections;
	};
}

#endif // BLUETOOTHSERVERCHANNELCONTEXT_H
