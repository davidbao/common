#include "thread/TickTimeout.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "system/Resources.h"
#include "communication/CommSampler.h"
#include "communication/BaseCommContext.h"
#include "communication/BaseCommConfig.h"
#include "driver/channels/TcpInteractive.h"

using namespace Data;
using namespace Diag;

namespace Communication {
    TcpMultiSampler::TcpMultiSampler(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd,
                                     const Client::Connection &connection, sampler_callback action, void *owner)
            : Sampler(dm, cd, dd) {
        _detectionInterval = (uint32_t) connection.detectionInterval.totalMilliseconds();
        _resumeInterval = (uint32_t) connection.resumeInterval.totalMilliseconds();
        _detectionCount = connection.detectionCount;

        _sampleCallback = action;
        _owner = owner;
    }

    TcpMultiSampler::~TcpMultiSampler() {
    }

    uint32_t TcpMultiSampler::detectionInterval() const {
        return _detectionInterval;
    }

    // return the resume interval, unit: ms
    uint32_t TcpMultiSampler::resumeInterval() const {
        return _resumeInterval;
    }

    // return the sample interval, unit: ms
    int TcpMultiSampler::detectionCount() const {
        return _detectionCount;
    }

    bool TcpMultiSampler::checkOnlineFailed() {
        if (_channel != nullptr) {
            TcpInteractive *ti = dynamic_cast<TcpInteractive *>(_channel->interactive());
            if (ti != nullptr) {
                if (!ti->connected())
                    return true;
            }
        }
        return Sampler::checkOnlineFailed();
    }

    void TcpMultiSampler::setConnectStatus(Device::Status status) {
        Device::Status oldStatus = getConnectStatus();
        if (oldStatus != status) {
            String str = ((TcpClientChannelContext *) context())->endpoint().toString();
            if (Device::isStatusChanged(oldStatus, status)) {
#ifdef WIN32
                String fmt = String::UTF8toGBK(Resources::getString("NetStateChanged"));
#else
                String fmt = Resources::getString("NetStateChanged");
#endif
                Trace::writeFormatLine(fmt.c_str(), str.c_str(),
                                       Device::getStatusStr(oldStatus).c_str(), Device::getStatusStr(status).c_str());
            }
        }
        Sampler::setConnectStatus(status);
    }

    InstructionDescription *TcpMultiSampler::sampleInstruction() {
        if (_sampleCallback != nullptr)
            return _sampleCallback(_owner);

        return nullptr;
    }

    Thread *TcpSingleSampler::_singleThread = nullptr;
    Mutex TcpSingleSampler::_poolsMutex;
    PList<TcpSingleSampler> TcpSingleSampler::_pools(false);

    TcpSingleSampler::TcpSingleSampler(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd,
                                       const Client::Connection &connection, sampler_callback action, void *owner)
            : Sampler(dm, cd, dd) {
        _detectionInterval = (uint32_t) connection.detectionInterval.totalMilliseconds();
        _resumeInterval = (uint32_t) connection.resumeInterval.totalMilliseconds();
        _detectionCount = connection.detectionCount;

        _sampleCallback = action;
        _owner = owner;
    }

    TcpSingleSampler::~TcpSingleSampler() {
    }

    uint32_t TcpSingleSampler::detectionInterval() const {
        return _detectionInterval;
    }

    // return the resume interval, unit: ms
    uint32_t TcpSingleSampler::resumeInterval() const {
        return _resumeInterval;
    }

    // return the sample interval, unit: ms
    int TcpSingleSampler::detectionCount() const {
        return _detectionCount;
    }

    bool TcpSingleSampler::checkOnlineFailed() {
        if (_channel != nullptr) {
            TcpInteractive *ti = dynamic_cast<TcpInteractive *>(_channel->interactive());
            if (ti != nullptr) {
                if (!ti->connected())
                    return true;
            }
        }
        return Sampler::checkOnlineFailed();
    }

    void TcpSingleSampler::setConnectStatus(Device::Status status) {
        Device::Status oldStatus = getConnectStatus();
        if (oldStatus != status) {
            String str = ((TcpClientChannelContext *) context())->endpoint().toString();
            if (Device::isStatusChanged(oldStatus, status)) {
                Trace::writeFormatLine(Resources::getString("NetStateChanged").c_str(), str.c_str(),
                                       Device::getStatusStr(oldStatus).c_str(), Device::getStatusStr(status).c_str());
            }
        }
        Sampler::setConnectStatus(status);
    }

    InstructionDescription *TcpSingleSampler::sampleInstruction() {
        if (_sampleCallback != nullptr)
            return _sampleCallback(_owner);

        return nullptr;
    }

    void TcpSingleSampler::start(Channel *channel, Device *device) {
        if (!channel)
            return;
        if (!device)
            return;

        _channel = channel;
        _device = device;

        _poolsMutex.lock();
        _pools.add(this);
        _poolsMutex.unlock();

        resume();
    }

    void TcpSingleSampler::stop() {
        pause();

        reset();

        _poolsMutex.lock();
        _pools.remove(this);
        _poolsMutex.unlock();

        _channel = nullptr;
        _device = nullptr;
    }

    void TcpSingleSampler::startSinglePool() {
        if (_singleThread == nullptr) {
            _singleThread = new Thread("client.InstructionSingleProc");
            _singleThread->startProc(instructionSingleProc, nullptr, 1);
        }
    }

    void TcpSingleSampler::stopSinglePool() {
        if (_singleThread != nullptr) {
            _singleThread->stop();
            delete _singleThread;
            _singleThread = nullptr;
        }
    }

    void TcpSingleSampler::instructionSingleProc(void *parameter) {
#ifdef DEBUG
        Stopwatch sw("multiplexing.TcpSingleSampler::instructionSingleProc", 1000);
#endif

        _poolsMutex.lock();
        for (uint32_t i = 0; i < _pools.count(); i++) {
            TcpSingleSampler *pool = _pools[i];
            pool->addSampleInstruction();
            pool->processInstructions();
        }
        _poolsMutex.unlock();
    }

    bool TcpSingleSampler::reopen() {
#ifdef DEBUG
        Stopwatch sw("multiplexing.TcpSingleSampler::reopen", 1000);
#endif
        if (_channel != nullptr) {
            return _channel->reopen();
//            _channel->reopenAsync();
//            return true;
        }
        return false;
    }

    UdpSampler::UdpSampler(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd, const Broadcast &broadcast,
                           sampler_callback action, void *owner)
            : Sampler(dm, cd, dd) {
        _detectionInterval = (uint32_t) broadcast.interval.totalMilliseconds();
        _resumeInterval = _detectionInterval;
        _detectionCount = broadcast.count;

        _broadcast = broadcast;
        _sampleCallback = action;
        _owner = owner;
    }

    UdpSampler::~UdpSampler() {
    }

    // return the sample interval, unit: ms
    uint32_t UdpSampler::detectionInterval() const {
        return _detectionInterval;
    }

    // return the resume interval, unit: ms
    uint32_t UdpSampler::resumeInterval() const {
        return _resumeInterval;
    }

    // return the sample interval, unit: ms
    int UdpSampler::detectionCount() const {
        return _detectionCount;
    }

    InstructionDescription *UdpSampler::sampleInstruction() {
        if (_sampleCallback != nullptr)
            return _sampleCallback(_owner);

        return nullptr;
    }
}
