#ifndef DRIVERDESCRIPTION_H
#define DRIVERDESCRIPTION_H

#include "data/PList.h"
#include "devices/DeviceDescription.h"
#include "channels/ChannelDescription.h"

using namespace Common;

namespace Drivers
{
	typedef PList<DeviceDescription> DeviceDescriptions;

	class DriverDescription
	{
	public:
        DriverDescription();
        ~DriverDescription();

        DeviceDescriptions* getDevices() const;
        void addDevice(DeviceDescription* description);
        const DeviceDescription* getDevice(const String& name) const;
        const ChannelDescription* getChannel(const String& name) const;
        
	private:
		DeviceDescriptions* _devices;
	};
}
#endif // DRIVERDESCRIPTION_H
