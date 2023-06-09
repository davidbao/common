#ifndef DEVICEDESCRIPTION_H
#define DEVICEDESCRIPTION_H

#include "../IContextProperty.h"
#include "driver/instructions/InstructionSet.h"
#include "driver/channels/ChannelDescription.h"

namespace Drivers {
    class DeviceContext : public Context {
    public:
        DeviceContext() {
            _sendTimeout = 3000;
            _receiveTimeout = 3000;

            _address = 0;
        }

        inline int address() const {
            return _address;
        }

        inline void setAddress(int address) {
            _address = address;
        }

        inline const String &addressStr() const {
            return _addressStr;
        }

        inline void setAddressStr(const String &addressStr) {
            _addressStr = addressStr;
        }

        inline uint32_t sendTimeout() const {
            return _sendTimeout;
        }

        inline void setSendTimeout(uint32_t timeout) {
            _sendTimeout = timeout;
        }

        inline void setSendTimeout(const TimeSpan &timeout) {
            _sendTimeout = (uint32_t) timeout.totalMilliseconds();
        }

        inline uint32_t receiveTimeout(const InstructionContext *ic = nullptr) const {
            if (ic != nullptr) {
                uint32_t timeout = ic->receiveTimeout();
                if (timeout > 0) {
                    return timeout;
                }
            }
            return _receiveTimeout;
        }

        inline uint32_t receiveDelay(const InstructionContext *ic = nullptr) const {
            return receiveTimeout(ic);
        }

        inline void setReceiveTimeout(uint32_t timeout) {
            if (timeout > 0)
                _receiveTimeout = timeout;
        }

        inline void setReceiveTimeout(const TimeSpan &timeout) {
            setReceiveTimeout((uint32_t) timeout.totalMilliseconds());
        }

        virtual DeviceContext *clone() const {
            auto context = new DeviceContext();
            context->copyFrom(this);
            return context;
        }

    protected:
        void copyFrom(const DeviceContext *context) {
            this->_address = context->_address;
            this->_addressStr = context->_addressStr;
            this->_sendTimeout = context->_sendTimeout;
            this->_receiveTimeout = context->_receiveTimeout;
        }

    protected:
        uint32_t _sendTimeout;
        uint32_t _receiveTimeout;

        int _address;
        String _addressStr;
    };

    class DeviceDescription : public IContextProperty {
    public:
        DeviceDescription(const String &name, ChannelDescription *cd, InstructionSet *is,
                          DeviceContext *context = nullptr) {
            _name = name;
            _channel = cd;
            _instructionSet = is;

            _context = context == nullptr ? new DeviceContext() : context;
        }

        ~DeviceDescription() override {
            if (_channel != nullptr) {
                delete _channel;
                _channel = nullptr;
            }
            if (_instructionSet != nullptr) {
                delete _instructionSet;
                _instructionSet = nullptr;
            }
            if (_context != nullptr) {
                delete _context;
                _context = nullptr;
            }
        }

        inline const String &name() const {
            return _name;
        }

        inline int address() const {
            return _context->address();
        }

        inline void setAddress(int address) {
            _context->setAddress(address);
        }

        inline const String &addressStr() const {
            return _context->addressStr();
        }

        inline void setAddressStr(const String &addressStr) {
            _context->setAddressStr(addressStr);
        }

        inline DeviceContext *context() const override {
            return _context;
        }

        inline ChannelDescription *getChannel() const {
            return _channel;
        }

        inline InstructionSet *instructionSet() const {
            return _instructionSet;
        }

        inline uint32_t sendTimeout() const {
            return _context->sendTimeout();
        }

        inline void setSendTimeout(uint32_t timeout) {
            _context->setSendTimeout(timeout);
        }

        inline void setSendTimeout(const TimeSpan &timeout) {
            _context->setSendTimeout(timeout);
        }

        inline uint32_t receiveTimeout(const InstructionContext *ic = nullptr) const {
            return _context->receiveTimeout(ic);
        }

        inline uint32_t receiveDelay(const InstructionContext *ic = nullptr) const {
            return _context->receiveDelay(ic) * 3;
        }

        inline void setReceiveTimeout(uint32_t timeout) {
            _context->setReceiveTimeout(timeout);
        }

        inline void setReceiveTimeout(const TimeSpan &timeout) {
            _context->setReceiveTimeout(timeout);
        }

    private:
        String _name;
        DeviceContext *_context;
        ChannelDescription *_channel;
        InstructionSet *_instructionSet;
    };

    typedef PList<DeviceDescription> DeviceDescriptions;
}
#endif // DEVICEDESCRIPTION_H
