#ifndef PARALLELINTERACTIVE_H
#define PARALLELINTERACTIVE_H

#include "Interactive.h"
#include "ParallelChannelContext.h"
#include "ChannelDescription.h"
#include "Channel.h"
#include "IO/ParallelPort.h"

namespace Drivers
{
	class ParallelInteractive : public Interactive
	{
	public:
		ParallelInteractive(DriverManager* dm, Channel* channel);
		~ParallelInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;

	private:
		inline ParallelChannelContext* getChannelContext()  
		{
			return (ParallelChannelContext*)(_channel->description()->context());
		}

	private:
		ParallelPort* _port;	
	};
}

#endif //PARALLELINTERACTIVE_H
