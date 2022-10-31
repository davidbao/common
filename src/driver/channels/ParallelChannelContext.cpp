#include "driver/channels/ParallelChannelContext.h"

namespace Drivers
{
	ParallelChannelContext::ParallelChannelContext(const String& name)
	{
		_portName = name;
	}
	ParallelChannelContext::~ParallelChannelContext()
	{
	}

    void ParallelChannelContext::setPortName(const String& portName)
    {
        _portName = portName;
    }
    const String ParallelChannelContext::portName() const
    {
        return _portName;
    }
}
