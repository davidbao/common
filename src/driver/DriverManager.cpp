#include "driver/DriverManager.h"
#include "exception/Exception.h"
#include "diag/Trace.h"
#include "thread/Locker.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/devices/Sampler.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    DriverManager::DriverManager() {
        _description = new DriverDescription();
        _devices = new Devices();
        _channels = new Channels();
        _pools = new InstructionPools();

        _opened = false;
    }

    DriverManager::~DriverManager() {
        close();

        delete _description;
        _description = nullptr;
        delete _devices;
        _devices = nullptr;
        delete _channels;
        _channels = nullptr;
        delete _pools;
        _pools = nullptr;
    }

    DriverDescription *DriverManager::description() const {
        return _description;
    }

    void DriverManager::open() {
        Locker locker(&_mutex);

        createDevices();

        for (size_t i = 0; i < _channels->count(); i++) {
            Channel *channel = _channels->at(i);
            channel->open();
        }

        // It is not server channel if it has a sampler.
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *pool = _pools->at(i);
            pool->start();
        }

        _opened = true;
    }

    void DriverManager::openAsync() {
        Locker locker(&_mutex);

        createDevices();

        for (size_t i = 0; i < _channels->count(); i++) {
            Channel *channel = _channels->at(i);
            channel->openAsync();
        }

        // It is not server channel if it has a sampler.
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *pool = _pools->at(i);
            pool->start();
        }

        _opened = true;
    }

    void DriverManager::reopen(const String &channelName, bool allowConnected) {
        if (channelName.isNullOrEmpty()) {
            for (size_t i = 0; i < _channels->count(); i++) {
                Channel *channel = _channels->at(i);
                reopen(channel, allowConnected);
            }
        } else {
            Channel *channel = getChannel(channelName);
            reopen(channel, allowConnected);
        }
    }

    void DriverManager::reopenAll(bool allowConnected) {
        reopen(String::Empty, allowConnected);
    }

    void DriverManager::reopen(Channel *channel, bool allowConnected) {
        if (channel != nullptr && channel->reopened() &&
            (!channel->connected() || allowConnected)) {
            channel->reopen();
        }
    }

    void DriverManager::reset() {
        Locker locker(&_mutex);

        Debug::writeLine("DriverManager::reset");

        for (size_t i = 0; i < _pools->count(); i++) {
            _pools->at(i)->stop();
        }

        reopenAll(true);

        for (size_t i = 0; i < _pools->count(); i++) {
            _pools->at(i)->start();
        }
    }

    void DriverManager::resetAsync() {
        Debug::writeFormatLine("DriverManager::resetAsync");
        _resetTask = Task::run(&DriverManager::reset, this);
    }

    void DriverManager::pause() {
        Locker locker(&_mutex);

        for (size_t i = 0; i < _pools->count(); i++) {
            _pools->at(i)->pause();
        }
    }

    void DriverManager::resume() {
        Locker locker(&_mutex);

        for (size_t i = 0; i < _pools->count(); i++) {
            _pools->at(i)->resume();
        }
    }

    void DriverManager::close() {
        if (!opened())
            return;

        _opened = false;

        _resetTask.cancel(3);

        Locker locker(&_mutex);

        for (size_t i = 0; i < _pools->count(); i++) {
            _pools->at(i)->stop();
        }
        _pools->clear();

        DeviceDescriptions *dds = _description->getDevices();
        for (size_t i = 0; i < dds->count(); i++) {
            DeviceDescription *dd = dds->at(i);

            ChannelDescription *cd = dd->getChannel();
            Channel *channel = getChannel(cd);
            if (channel != nullptr) {
                channel->close();
            }
        }
        _devices->clear();
        _channels->clear();
    }

    bool DriverManager::hasDevice(const String &deviceName) {
        bool result = false;

        Locker locker(&_mutex);
        DeviceDescriptions *dds = _description->getDevices();
        if (!(dds == nullptr || dds->count() == 0)) {
            for (size_t i = 0; i < dds->count(); i++) {
                DeviceDescription *dd = dds->at(i);
                if (dd->name() == deviceName) {
                    result = true;
                    break;
                }
            }
        }

        return result;
    }

    Device *DriverManager::getDevice(const String &deviceName) const {
        for (size_t i = 0; i < _devices->count(); i++) {
            Device *device = _devices->at(i);
            if (device != nullptr &&
                device->description()->name() == deviceName) {
                return device;
            }
        }
        return nullptr;
    }

    Device *DriverManager::getDevice(const Channel *channel) const {
        for (size_t i = 0; i < _devices->count(); i++) {
            Device *device = _devices->at(i);
            if (device != nullptr &&
                device->getChannel() == channel) {
                return device;
            }
        }
        return nullptr;
    }

    void DriverManager::getDevices(const Channel *channel, Devices &devices) const {
        devices.setAutoDelete(false);
        for (size_t i = 0; i < _devices->count(); i++) {
            Device *device = _devices->at(i);
            if (device != nullptr &&
                device->getChannel() == channel) {
                devices.add(device);
            }
        }
    }

    Device *DriverManager::getDevice(DeviceDescription *dd) const {
        return dd != nullptr ? getDevice(dd->name()) : nullptr;
    }

    bool DriverManager::containsDevice(DeviceDescription *dd) const {
        return getDevice(dd) != nullptr;
    }

    bool DriverManager::hasChannel(ChannelDescription *cd) {
        for (size_t i = 0; i < _channels->count(); i++) {
            const Channel *channel = _channels->at(i);
            if (channel != nullptr &&
                channel->description() == cd) {
                return true;
            }
        }
        return false;
    }

    Channel *DriverManager::getChannel(const String &channelName) const {
        for (size_t i = 0; i < _channels->count(); i++) {
            Channel *channel = _channels->at(i);
            if (channel->description()->name() == channelName) {
                return channel;
            }
        }
        return nullptr;
    }

    Channel *DriverManager::getChannel(ChannelDescription *cd) const {
        return cd != nullptr ? getChannel(cd->name()) : nullptr;
    }

    void DriverManager::addPool(const InstructionPool *ip) {
        if (ip != nullptr) {
            _pools->add(ip);
        }
    }

    InstructionPool *DriverManager::getPool(const String &deviceName) const {
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *ip = _pools->at(i);
            if (ip->containsDevice(deviceName)) {
                return ip;
            }
        }
        return nullptr;
    }

    InstructionPool *DriverManager::getPoolByChannelName(const String &channelName) const {
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *ip = _pools->at(i);
            if (ip->channelName() == channelName) {
                return ip;
            }
        }
        return nullptr;
    }

    InstructionPool *DriverManager::getPoolByDeviceName(const String &deviceName) const {
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *ip = _pools->at(i);
            if (ip->deviceName() == deviceName) {
                return ip;
            }
        }
        return nullptr;
    }

    void DriverManager::getPoolsWithoutDevice(const StringArray &deviceNames, InstructionPools &ips) const {
        ips.setAutoDelete(false);
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *ip = _pools->at(i);
            for (size_t j = 0; j < deviceNames.count(); j++) {
                String deviceName = deviceNames[j];
                if (deviceName.isNullOrEmpty() || ip->deviceName() != deviceName) {
                    ips.add(ip);
                }
            }
        }
    }

    void DriverManager::getPools(const StringArray &deviceNames, InstructionPools &ips) const {
        ips.setAutoDelete(false);
        for (size_t i = 0; i < _pools->count(); i++) {
            InstructionPool *ip = _pools->at(i);
            for (size_t j = 0; j < deviceNames.count(); j++) {
                String deviceName = deviceNames[j];
                if (deviceName.isNullOrEmpty() || ip->deviceName() == deviceName) {
                    ips.add(ip);
                }
            }
        }
    }

    const InstructionPools *DriverManager::getPools() const {
        return _pools;
    }

    bool DriverManager::opened() const {
        return _opened;
    }

    void DriverManager::createDevices() {
        DeviceDescriptions *dds = _description->getDevices();
        if (dds == nullptr || dds->count() == 0) {
            return;
        }

        for (size_t i = 0; i < dds->count(); i++) {
            DeviceDescription *dd = dds->at(i);
            if (!containsDevice(dd)) {
                ChannelDescription *cd = dd->getChannel();
                Channel *channel = getChannel(cd);
                if (channel == nullptr) {
                    channel = new Channel(this, cd);
                    channel->openedDelegates()->add(Delegate(this, channelOpened));
                    channel->closedDelegates()->add(Delegate(this, channelClosed));
                    _channels->add(channel);
                }

                Device *device = new Device(dd, channel);
                _devices->add(device);
            }
        }
    }

    InstructionContext *DriverManager::executeInstruction(const String &deviceName, InstructionDescription *id) {
        InstructionPool *ip = getPool(deviceName);
        if (ip != nullptr) {
            return ip->executeInstructionSync(id);
        } else {
            Device *device = getDevice(deviceName);
            if (device != nullptr) {
                return device->executeInstruction(id);
            }
        }
        return nullptr;
    }

    Delegates *DriverManager::channelOpenedDelegates() {
        return &_channelOpenedDelegates;
    }

    Delegates *DriverManager::channelClosedDelegates() {
        return &_channelClosedDelegates;
    }

    void DriverManager::channelOpened(void *owner, void *sender, EventArgs *args) {
        DriverManager *dm = (DriverManager *) owner;
        dm->_channelOpenedDelegates.invoke(sender, args);
    }

    void DriverManager::channelClosed(void *owner, void *sender, EventArgs *args) {
        DriverManager *dm = (DriverManager *) owner;
        dm->_channelClosedDelegates.invoke(sender, args);
    }
}
