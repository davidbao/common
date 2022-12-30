//
//  Instruction.cpp
//  common
//
//  Created by baowei on 2016/10/13.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "driver/instructions/Instruction.h"
#include "diag/Trace.h"
#include "system/Singleton.h"
#include "system/Math.h"
#include "driver/DriverManager.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    InstructionContext::InstructionContext() {
        _exception = nullptr;
        _qualitystamp = Bad;
        _timestamp = 0;
        _receiveTimeout = 0;
    }

    InstructionContext::~InstructionContext() {
        clearException();
    }

    void InstructionContext::setException(Exception *exception) {
        clearException();
        _exception = exception;
    }

    void InstructionContext::clearException() {
        if (_exception != nullptr) {
            delete _exception;
            _exception = nullptr;
        }
    }

    Exception *InstructionContext::exception() const {
        return _exception;
    }

    bool InstructionContext::hasException() const {
        return _exception != nullptr;
    }

    void InstructionContext::setTimeStamp(uint32_t stamp) {
        _timestamp = stamp;
    }

    uint32_t InstructionContext::getTimeStamp() const {
        return _timestamp;
    }

    void InstructionContext::setQualityStamp(uint16_t stamp) {
        _qualitystamp = stamp;
    }

    uint16_t InstructionContext::getQualityStamp() const {
        return _qualitystamp;
    }

    uint32_t InstructionContext::receiveTimeout() const {
        return _receiveTimeout;
    }

    void InstructionContext::setReceiveTimeout(uint32_t timeout) {
        _receiveTimeout = timeout;
    }

    bool InstructionContext::allowExecution() const {
        return true;
    }

    InstructionSet::InstructionSet() {
    }

    InstructionSet::~InstructionSet() {
    }

    bool InstructionSet::receive(Device *device, Channel *channel, ByteArray *buffer, int order) {
        return false;
    }

    InstructionSet *InstructionSet::clone() const {
        throw NotImplementedException("It must be override.");
    }

    bool InstructionSet::recombine(const ByteArray &buffer, PList<ByteArray> &buffers) {
        buffers.add(new ByteArray(buffer));
        return true;
    }

    InstructionDescription::InstructionDescription(const String &name, InstructionContext *context,
                                                   bool autoDeleteContext) {
        _name = name;
        _context = context;
        _autoDeleteContext = autoDeleteContext;
    }

    InstructionDescription::~InstructionDescription() {
        if (_autoDeleteContext) {
            if (_context != nullptr) {
                delete _context;
                _context = nullptr;
            }
        }
    }

    const String &InstructionDescription::name() const {
        return _name;
    }

    InstructionContext *InstructionDescription::context() const {
        return _context;
    }

    void InstructionDescription::setContext(InstructionContext *context) {
        _context = context;
    }

    bool InstructionDescription::allowExecution() const {
        if (_context == nullptr)
            return true;
        return _context->allowExecution();
    }

    Instruction::LogContext::LogContext(bool allowInformation, bool allowMessage, uint32_t logLength) : allowInformation(
            allowInformation), allowMessage(allowMessage), _logLength(logLength) {
    }

    Instruction::LogContext::LogContext(const LogContext &value) {
        evaluates(value);
    }

    Instruction::LogContext::~LogContext() {
    }

    bool Instruction::LogContext::equals(const LogContext &other) const {
        return allowInformation == other.allowInformation &&
               allowMessage == other.allowMessage &&
               _logLength == other._logLength;
    }

    void Instruction::LogContext::evaluates(const LogContext &other) {
        allowInformation = other.allowInformation;
        allowMessage = other.allowMessage;
        _logLength = other._logLength;
    }

    uint32_t Instruction::LogContext::logLength() const {
        return _logLength;
    }

    void Instruction::LogContext::setLogLength(uint32_t length) {
        if (length >= MinLogLength && length <= MaxLogLength) {
            _logLength = length;
        }
    }

    Instruction::Instruction(InstructionDescription *id) {
        _description = id;
        _receiveInstruction = nullptr;

        _isExecuting = false;

        _logLength = DefaultLogLength;
        _allowLogInformation = true;
        _allowLogMessage = false;
    }

    Instruction::~Instruction() {
        delete _description;
        _description = nullptr;
        _receiveInstruction = nullptr;
    }

    InstructionDescription *Instruction::description() const {
        return _description;
    }

    InstructionContext *Instruction::context() const {
        return _description != nullptr ? _description->context() : nullptr;
    }

    void Instruction::setContext(InstructionContext *context) {
        if (_description != nullptr) {
            _description->setContext(context);
        }
    }

    bool Instruction::match(const ByteArray *buffer, DeviceDescription *dd) {
        throw NotImplementedException("It must override this method.");
    }

    void Instruction::setReceiveInstruction(Instruction *instruction) {
        assert(instruction);
        _receiveInstruction = instruction;
    }

    size_t Instruction::logLength() const {
        return _logLength;
    }

    void Instruction::setLogLength(size_t length) {
        if (length >= MinLogLength && length <= MaxLogLength) {
            _logLength = length;
        }
    }

    bool Instruction::allowLogInformation() const {
        return _allowLogInformation;
    }

    bool Instruction::allowLogMessage() const {
        return _allowLogMessage;
    }

    void Instruction::setAllowLog(bool allowInformation, bool allowMessage) {
        _allowLogInformation = allowInformation;
        _allowLogMessage = allowMessage;
    }

    void Instruction::log(const ByteArray *buffer, bool send, const Endpoint &endpoint) {
        if (buffer != nullptr)
            log(buffer, buffer->count(), send, endpoint);
    }

    void Instruction::log(const ByteArray *buffer, size_t count, bool send, const Endpoint &endpoint) {
        if (!allowLogMessage())
            return;

        logDirectly(buffer, count, send, endpoint);
    }

    void Instruction::log(const String &information) {
        if (!allowLogInformation())
            return;

        logDirectly(information);
    }

    void Instruction::logDirectly(const ByteArray *buffer, bool send, const Endpoint &endpoint) {
        logDirectly(buffer, buffer->count(), send, endpoint);
    }

    void Instruction::logDirectly(const ByteArray *buffer, size_t count, bool send, const Endpoint &endpoint) {
        if (buffer != nullptr && buffer->count() > 0 && count > 0) {
            if (count > buffer->count())
                count = buffer->count();

            String str;
            if (count > logLength()) {
                ByteArray array(buffer->data(), Math::min(logLength(), count));
                str = array.toString();
            } else {
                str = buffer->toString();
            }

            String info;
            if (endpoint.isEmpty()) {
                info = send ? String::convert("send: %s", str.c_str()) : String::convert("recv: %s", str.c_str());
            } else {
                info = send ?
                       String::convert("send(%s:%d): %s", endpoint.address.c_str(), endpoint.port, str.c_str()) :
                       String::convert("recv(%s:%d): %s", endpoint.address.c_str(), endpoint.port, str.c_str());
            }
            Trace::debug(info);
        }
    }

    void Instruction::logDirectly(const String &information) {
        if (!information.isNullOrEmpty()) {
            String info;
            size_t count = information.length();
            if (count > logLength()) {
                info = information.substr(0, count);
            } else {
                info = information;
            }

            Trace::debug(info);
        }
    }

    InstructionContext *
    Instruction::executeAsync(Interactive *interactive, Device *device, InstructionContext *context) {
        return nullptr;
    }

    Instructions::Instructions(bool autoDelete, uint32_t capacity) : PList<Instruction>(autoDelete, capacity) {
    }

    void Instructions::setReceiveInstruction(Instructions *instructions) {
        for (size_t i = 0; i < count(); i++) {
            Instruction *si = at(i);
            for (size_t j = 0; j < instructions->count(); j++) {
                Instruction *ri = instructions->at(j);
                if (ri->description()->name() == si->description()->name()) {
                    si->setReceiveInstruction(ri);
                    break;
                }
            }
        }
    }
}
