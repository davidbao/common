#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "driver/channels/ParallelInteractive.h"

using namespace Common;

namespace Drivers
{
	ParallelInteractive::ParallelInteractive(DriverManager* dm, Channel* channel) : Interactive(dm, channel)
	{
		if(channel == nullptr)
			throw ArgumentException("channel");
		if((ParallelChannelContext*)channel->description()->context() == nullptr)
			throw ArgumentException("channel");

		_port = nullptr;
	}
	ParallelInteractive::~ParallelInteractive()
	{
		close();
	}
	bool ParallelInteractive::open()
	{
		close();

		ParallelChannelContext* scc = getChannelContext();

#ifdef DEBUG
		String message = String::convert("open a parallel port, name = %s", scc->portName().c_str());
		Stopwatch sw(message);
#endif

		_port = new ParallelPort(scc->portName());
		return _port->open();
	}
	void ParallelInteractive::close()
	{
		if(_port != nullptr && _port->isOpen())
		{
			_port->close();
			delete _port;
			_port = nullptr;
		}
	}

	bool ParallelInteractive::connected()
	{
		return _port != nullptr ? _port->isOpen() : false;
	}
    size_t ParallelInteractive::available()
	{
		return _port->available();
	}

	ssize_t ParallelInteractive::send(const uint8_t* buffer, off_t offset, size_t count)
	{
#ifdef DEBUG
		Stopwatch sw("parallel send", 1000);
#endif
		ssize_t len = 0;
		if(connected())
		{
			len = _port->write((const char*)buffer, count);
		}
		return len;
	}
	ssize_t ParallelInteractive::receive(uint8_t* buffer, off_t offset, size_t count)
	{
#ifdef DEBUG
		Stopwatch sw("parallel receive", 1000);
#endif
		ssize_t len = 0;
		if(connected())
		{
			len = _port->read((char*)(buffer+offset),count);
		}
		return len;
	}
}
