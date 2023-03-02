#include <assert.h>
#include "driver/devices/Sampler.h"
#include "thread/Locker.h"
#include "thread/TickTimeout.h"
#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "system/Resources.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/devices/DeviceDescription.h"

using namespace Diag;

namespace Drivers {
    Sampler::Sampler(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd, bool skipSampler)
            : InstructionPool(dm, cd, dd) {
        _connected = Device::Unknown;
        _sampleStart = (uint32_t) -1;    // the first sample value.

        _showConnectedError = true;
        _isInvalidStatus = false;
        _checkOnlineFailedCount = 0;
        _connetedFailedCount = 0;
        _sampleInterval = detectionInterval();

        _skipSampler = skipSampler;
    }

    Sampler::~Sampler() {
        stop();

        _connected = Device::Unknown;
    }

    Device::Status Sampler::getConnectStatus() const {
        return _connected;
    }

    bool Sampler::isOnline() const {
        return getConnectStatus() == Device::Online;
    }

    bool Sampler::isOffline() const {
        return getConnectStatus() == Device::Offline;
    }

    uint32_t Sampler::detectionInterval() const {
        return 3000;
    }

    // return the resume interval, unit: ms
    uint32_t Sampler::resumeInterval() const {
        return 10 * 1000;
    }

    // return the sample interval, unit: ms
    int Sampler::detectionCount() const {
        return 3;
    }

    void Sampler::setConnectStatus(Device::Status status) {
        if (_connected != status) {
            Device::Status oldStatus = _connected;
            _connected = status;

            DeviceStatusEventArgs e(_dd->name(), oldStatus, status);
            _statusChangedAction.invoke(this, &e);
        }
    }

    InstructionDescription *Sampler::sampleInstruction() {
        return new InstructionDescription(String::Empty);
    }

    void Sampler::addSampleInstruction() {
        // add the sample instruction into the looparray.
        if (_sampleStart == 0) {
            _sampleStart = TickTimeout::getCurrentTickCount();
        }
        TimeSpan interval = TimeSpan::fromMilliseconds(_sampleInterval);
        if (_sampleStart == (uint32_t) -1 ||
            TickTimeout::isTimeout(_sampleStart, interval)) {
            if (_isInvalidStatus) {
                _isInvalidStatus = false;
                _sampleInterval = detectionInterval();

                if (_showConnectedError)
                    Trace::writeFormatLine(Resources::getString("RetryingConnectDevice").c_str(), _dd->name().c_str());
            }
            _sampleStart = 0;

            addInstructionInner(sampleInstruction());
        }
    }

    bool isChannelOpened(void *parameter) {
        const Channel *channel = static_cast<const Channel *>(parameter);
        assert(channel);
        return channel->opened();
    }

    void Sampler::instructionProc() {
        if (!_pause) {
            if (_sampleStart == (uint32_t) -1) {
                TickTimeout::msdelay(5000, isChannelOpened, _channel);
            }

            addSampleInstruction();
        }
        InstructionPool::instructionProc();
    }

    bool Sampler::reopen() {
        return _channel != nullptr ? _channel->reopen() : false;
    }

    void Sampler::errorHandle(const DeviceDescription *dd, const InstructionDescription *id, bool error) {
        if (error) {
            if (_showConnectedError)
                Trace::writeFormatLine(Resources::getString("UnableToConnectDevice").c_str(), _dd->name().c_str());

            _checkOnlineFailedCount++;

            if (checkOnlineFailed()) {
                setConnectStatus(Device::Offline);

                if (_channel != NULL && _channel->reopened()) {
                    reopen();
                }

                if (_showConnectedError)
                    Trace::writeFormatLine(Resources::getString("DeviceFailure").c_str(), _dd->name().c_str());

                _sampleStart = 0;
                _sampleInterval = resumeInterval();

                _isInvalidStatus = true;

                _checkOnlineFailedCount = 0;
                _connetedFailedCount++;
            }
        } else {
            setConnectStatus(Device::Online);

            _checkOnlineFailedCount = 0;
            _connetedFailedCount = 0;

            _sampleInterval = detectionInterval();
        }
    }

    bool Sampler::checkOnlineFailed() {
        return _checkOnlineFailedCount >= detectionCount();
    }

    void Sampler::setConnectedError(bool showError) {
        _showConnectedError = showError;
    }

    Delegates *Sampler::statusChangedDelegates() {
        return &_statusChangedAction;
    }

    void Sampler::reset() {
        InstructionPool::reset();

        _connected = Device::Unknown;
        _sampleStart = (uint32_t) -1;

        _isInvalidStatus = false;
        _checkOnlineFailedCount = 0;
        _connetedFailedCount = 0;
        _sampleInterval = detectionInterval();
    }
}
