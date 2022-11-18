#include "exception/Exception.h"
#include "data/DateTime.h"
#include "diag/Trace.h"
#include "diag/Trace.h"
#include "thread/Thread.h"
#include "thread/Locker.h"
#include "thread/ThreadPool.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/DriverManager.h"
#include "driver/devices/Sampler.h"

using namespace Common;

namespace Drivers
{
    DriverDescription::DriverDescription()
    {
        _devices = new DeviceDescriptions();
    }
    DriverDescription::~DriverDescription()
    {
        delete _devices;
    }
    
    DeviceDescriptions* DriverDescription::getDevices() const
    {
        return _devices;
    }
    void DriverDescription::addDevice(DeviceDescription* description)
    {
        _devices->add(description);
    }
    const DeviceDescription* DriverDescription::getDevice(const String& name) const
    {
        for (uint32_t i=0; i<_devices->count(); i++)
        {
            const DeviceDescription* dd = _devices->at(i);
            if(dd->name() == name)
                return dd;
        }
        return nullptr;
    }
    const ChannelDescription* DriverDescription::getChannel(const String& name) const
    {
        const DeviceDescription* dd = getDevice(name);
        if(dd != nullptr)
            return dd->getChannel();
        return nullptr;
    }
}
