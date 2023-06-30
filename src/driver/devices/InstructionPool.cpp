#include <cassert>
#include "driver/devices/InstructionPool.h"
#include "thread/Locker.h"
#include "thread/TickTimeout.h"
#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "system/Resources.h"
#include "driver/DriverManager.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/devices/DeviceDescription.h"

using namespace Diag;

namespace Drivers {
    InstructionPool::Packet::Packet(InstructionDescription *id, AutoDelete autoDelete, uint8_t priority)
            : _dd(nullptr), _id(id), _autoDelete(autoDelete), _priority(priority), _processed(false), _ic(nullptr) {
    }

    InstructionPool::Packet::Packet(DeviceDescription *dd, InstructionDescription *id, AutoDelete autoDelete,
                                    uint8_t priority)
            : _dd(dd), _id(id), _autoDelete(autoDelete), _priority(priority), _processed(false), _ic(nullptr) {
    }

    InstructionPool::Packet::~Packet() {
        _dd = nullptr;
        if (_autoDelete == PacketAndInstruction && _id != nullptr) {
            delete _id;
        }
        _id = nullptr;
        _ic = nullptr;
    }

    DeviceDescription *InstructionPool::Packet::deviceDescription() const {
        return _dd;
    }

    InstructionDescription *InstructionPool::Packet::getInstruction() const {
        return _id;
    }

    void InstructionPool::Packet::process(InstructionContext *ic) {
        _ic = ic;
        _processed = true;
    }

    bool InstructionPool::Packet::isProcessed() const {
        return _processed;
    }

    InstructionContext *InstructionPool::Packet::getInstructionContext() const {
        return _ic;
    }

    bool InstructionPool::Packet::needDeleted() const {
        return _autoDelete == AutoDelete::PacketAndInstruction || _autoDelete == AutoDelete::PacketOnly;
    }

    void InstructionPool::Packet::setAutoDelete() {
        if (_autoDelete == AutoDelete::None)
            _autoDelete = AutoDelete::PacketOnly;
    }

    int InstructionPool::Packet::compareTo(const Packet &other) const {
        uint8_t x = this->_priority;
        uint8_t y = other._priority;
        if (x > y)
            return 1;
        else if (x < y)
            return -1;
        else
            return 0;
    }

    InstructionPool::LoopInstructions::LoopInstructions(int maxLength, bool autoDelete, bool hasPriority)
            : LoopPList<Packet>(maxLength, autoDelete) {
        _hasPriority = hasPriority;
    }

    void InstructionPool::LoopInstructions::copyTo(Packet **value) {
        LoopPList<Packet>::copyTo(value);
        if (_hasPriority)    // sort by priority
        {
            size_t c = count();
            if (c > 1) {
                PList<Packet>::sort(value, c);
            }
        }
    }

    InstructionPool::InstructionPool(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd,
                                     bool hasPriority) {
        if (dm == nullptr)
            throw ArgumentNullException("dm");
        if (cd == nullptr)
            throw ArgumentNullException("cd");
        if (dd == nullptr)
            throw ArgumentNullException("dd");

        _manager = dm;
        _cd = cd;
        _dd = dd;

        _device = nullptr;
        _channel = nullptr;

        _instructionThread = nullptr;

        _instructionsMutex.lock();
        _instructions = new LoopInstructions(10 * 1024, true, hasPriority);
        _instructionsMutex.unlock();

        _pause = false;
    }

    InstructionPool::~InstructionPool() {
        InstructionPool::stop();

        _cd = nullptr;
        _dd = nullptr;

        _instructionsMutex.lock();
        delete _instructions;
        _instructions = nullptr;
        _instructionsMutex.unlock();
    }

    void InstructionPool::start() {
        DriverManager *dm = manager();
        assert(dm);

        start(dm->getChannel(_cd), dm->getDevice(_dd));
    }

    void InstructionPool::start(Channel *channel, Device *device) {
        if (!channel)
            return;
        if (!device)
            return;

        _channel = channel;
        _device = device;

        if (_instructionThread == nullptr) {
            _instructionThread = new Timer(String::format("InstructionProc_%s", _dd->name().c_str()),
                                           1, &InstructionPool::instructionProc, this);
        }

        resume();
    }

    void InstructionPool::stop() {
        pause();

        reset();

        if (_instructionThread != nullptr) {
            _instructionThread->stop();
            delete _instructionThread;
            _instructionThread = nullptr;
        }

        _channel = nullptr;
        _device = nullptr;
    }

    bool InstructionPool::channelConnected() const {
        return _channel != nullptr && _channel->connected();
    }

    InstructionPool::Packet *
    InstructionPool::addInstruction(InstructionDescription *id, Packet::AutoDelete autoDelete, uint8_t priority) {
        if (_pause || channelConnected()) {
            return addInstructionInner(id, autoDelete, priority);
        } else {
            if (autoDelete == Packet::AutoDelete::PacketAndInstruction) {
                delete id;
            }
        }
        return nullptr;
    }

    InstructionPool::Packet *InstructionPool::addTickInstruction(InstructionDescription *id, const TimeSpan &timeout) {
        uint32_t tick;
        Packet *packet = nullptr;
        if (!_nameTicks.at(id->name(), tick) || TickTimeout::isTimeout(tick, timeout)) {
            packet = addInstruction(id);
            _nameTicks.add(id->name(), TickTimeout::getCurrentTickCount());
        }
        return packet;
    }

    void InstructionPool::addInstruction(const InstructionDescriptions &ids, Packet::AutoDelete autoDelete,
                                         uint8_t priority) {
        for (size_t i = 0; i < ids.count(); i++) {
            InstructionDescription *id = ids.at(i);
            addInstruction(id, autoDelete, priority);
        }
    }

    InstructionPool::Packet *
    InstructionPool::addInstruction(DeviceDescription *dd, InstructionDescription *id, Packet::AutoDelete autoDelete,
                                    uint8_t priority) {
        if (_pause || channelConnected()) {
            return addInstructionInner(dd, id, autoDelete, priority);
        } else {
            if (autoDelete == Packet::AutoDelete::PacketAndInstruction) {
                delete id;
            }
        }
        return nullptr;
    }

    void InstructionPool::addInstruction(DeviceDescription *dd, const InstructionDescriptions &ids,
                                         Packet::AutoDelete autoDelete, uint8_t priority) {
        for (size_t i = 0; i < ids.count(); i++) {
            InstructionDescription *id = ids.at(i);
            addInstruction(dd, id, autoDelete, priority);
        }
    }

    InstructionPool::Packet *
    InstructionPool::addInstructionInner(InstructionDescription *id, Packet::AutoDelete autoDelete, uint8_t priority) {
        if (id != nullptr && id->allowExecution() && _instructions != nullptr) {
            auto packet = new Packet(id, autoDelete, priority);
            Locker locker(&_instructionsMutex);
            _instructions->enqueue(packet);
            return packet;
        }
        return nullptr;
    }

    void InstructionPool::addInstructionInner(const InstructionDescriptions &ids, Packet::AutoDelete autoDelete,
                                              uint8_t priority) {
        for (size_t i = 0; i < ids.count(); i++) {
            InstructionDescription *id = ids.at(i);
            addInstructionInner(id, autoDelete, priority);
        }
    }

    InstructionPool::Packet *InstructionPool::addInstructionInner(DeviceDescription *dd, InstructionDescription *id,
                                                                  Packet::AutoDelete autoDelete, uint8_t priority) {
        if (id != nullptr && id->allowExecution() && _instructions != nullptr) {
            auto packet = new Packet(dd, id, autoDelete, priority);
            Locker locker(&_instructionsMutex);
            _instructions->enqueue(packet);
            return packet;
        }
        return nullptr;
    }

    void InstructionPool::addInstructionInner(DeviceDescription *dd, const InstructionDescriptions &ids,
                                              Packet::AutoDelete autoDelete, uint8_t priority) {
        for (size_t i = 0; i < ids.count(); i++) {
            InstructionDescription *id = ids.at(i);
            addInstructionInner(dd, id, autoDelete, priority);
        }
    }

    void InstructionPool::processInstructions() {
        // process the instructions, all of...
        if (_channel != nullptr) {
            _instructionsMutex.lock();
            size_t count = _instructions->count();
            if (count > 0) {
                auto packets = new Packet *[count];
                _instructions->copyTo(packets);
                _instructions->makeNull(false);
                _instructionsMutex.unlock();

                if (channelConnected()) {
                    for (size_t i = 0; i < count; i++) {
                        Packet *packet = packets[i];
                        processPacket(packet);
                    }
                } else {
//                    Debug::writeFormatLine("InstructionPool::processInstructions, channelConnected is false, device name: %s", _device->name().c_str());
                    for (size_t i = 0; i < count; i++) {
                        Packet *packet = packets[i];
                        if (packet->needDeleted()) {
                            delete packet;
                        } else {
                            packet->process();
                        }
                    }

                    errorHandle(true);
                }

                delete[] packets;
            } else {
                _instructionsMutex.unlock();
            }
        }
    }

    void InstructionPool::processPacket(Packet *packet) {
        if (_pause) {
            if (packet->needDeleted())
                delete packet;
        } else {
            InstructionContext *ic = nullptr;
            InstructionDescription *id = packet->getInstruction();
//                            Debug::writeFormatLine("execute an instruction, device name: '%s', instruction name: '%s'",
//                                                   packet->deviceDescription() == nullptr ?
//                                                   _dd->name().c_str() : packet->deviceDescription()->name().c_str(),
//                                                   id->name().c_str());
            if (!id->name().isNullOrEmpty()) {
                ic = packet->deviceDescription() == nullptr ?
                     executeInstruction(id) :
                     executeInstruction(packet->deviceDescription(), id);
                bool result = (ic != nullptr && !ic->hasException());
//                if(!result)
//                {
//                    Trace::writeFormatLine("InstructionPool::processPacket'%s', ic is %s, ic has '%s'",
//                                           channel()->name().c_str(),
//                                           ic != nullptr ? "not null" : "null",
//                                           (ic != nullptr && ic->hasException()) ? ic->exception()->message().c_str() : "not exception");
//                }

                DeviceDescription *dd = packet->deviceDescription() == nullptr ? _dd : packet->deviceDescription();
                errorHandle(dd, id, !result);

                if (packet->needDeleted()) {
                    delete packet;
                } else {
                    packet->process(ic);
                }
            } else {
                if (packet->needDeleted())
                    delete packet;
            }
        }
    }

    InstructionContext *InstructionPool::executeInstruction(InstructionDescription *id) {
        return _device != nullptr ? _device->executeInstruction(id) : nullptr;
    }

    InstructionContext *InstructionPool::executeInstruction(DeviceDescription *dd, InstructionDescription *id) {
        return _device != nullptr && _device->description() == dd ? executeInstruction(id) : nullptr;
    }

    InstructionContext *InstructionPool::executeInstructionSync(DeviceDescription *dd, InstructionDescription *id) {
#ifdef DEBUG
        Stopwatch sw(String::convert("InstructionPool::executeInstructionSync, name: %s", id->name().c_str()), 2000);
#endif
#ifdef __EMSCRIPTEN__
        if (!id->name().isNullOrEmpty())
        {
            bool result = true;
            InstructionContext* ic = executeInstruction(dd, id);
            result = (ic != nullptr && !ic->hasException());
            
            errorHandle(dd, id, !result);
            
            return ic;
        }
        return nullptr;
#else
        Packet *packet = addInstruction(dd, id, Packet::AutoDelete::None);
        if (packet != nullptr) {
            uint32_t timeout = dd->receiveDelay(id->context());
            TickTimeout::msdelay(timeout, isPacketFinished, packet);
            if (packet->isProcessed()) {
                InstructionContext *ic = packet->getInstructionContext();
                delete packet;
                return ic;
            } else {
                packet->setAutoDelete();
            }
        }
        return nullptr;
#endif
    }

    InstructionContext *InstructionPool::executeInstructionSync(InstructionDescription *id) {
        return executeInstructionSync(_dd, id);
    }

    void InstructionPool::instructionProc() {
        if (!_pause) {
            try {
                processInstructions();
            }
            catch (const exception &e) {
                String str = String::convert("InstructionPool::instructionProc error'%s'", e.what());
                Trace::writeLine(str, Trace::Error);
            }
        }
    }

    void InstructionPool::errorHandle(const DeviceDescription *dd, const InstructionDescription *id, bool error) {
    }

    void InstructionPool::errorHandle(bool error) {
        errorHandle(nullptr, nullptr, error);
    }

    const String &InstructionPool::deviceName() const {
        return _dd != nullptr ? _dd->name() : String::Empty;
    }

    const String &InstructionPool::channelName() const {
        return _cd != nullptr ? _cd->name() : String::Empty;
    }

    DriverManager *InstructionPool::manager() {
        return _manager;
    }

    ChannelContext *InstructionPool::context() const {
        return _cd != nullptr ? _cd->context() : nullptr;
    }

    ChannelDescription *InstructionPool::channelDescription() const {
        return _cd;
    }

    void InstructionPool::pause() {
        _pause = true;
    }

    void InstructionPool::resume() {
        _pause = false;
    }

    void InstructionPool::reset() {
        _instructionsMutex.lock();
        _instructions->clear();
        _instructionsMutex.unlock();
    }

    Channel *InstructionPool::channel() const {
        return _channel;
    }

    Device *InstructionPool::device() const {
        return _device;
    }

    bool InstructionPool::isPacketFinished(void *parameter) {
        if (parameter != nullptr) {
            auto packet = (InstructionPool::Packet *) parameter;
            return packet->isProcessed();
        }
        return false;
    }

    bool InstructionPool::containsDevice(const String &deviceName) const {
        return this->deviceName() == deviceName;
    }
}
