#ifndef PARALLELCHANNELCONTEXT_H
#define PARALLELCHANNELCONTEXT_H

#include "ChannelContext.h"

namespace Drivers
{
	class ParallelChannelContext : public ChannelContext
	{
	public:
		ParallelChannelContext(const String& name);
		~ParallelChannelContext() override;

        void setPortName(const String& portName);
        const String portName() const;

	private:
		String _portName;
	};
}

#endif //PARALLELCHANNELCONTEXT_H
