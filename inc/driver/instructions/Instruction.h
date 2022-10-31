//
//  Instruction.h
//  common
//
//  Created by baowei on 2016/10/13.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Instruction_h
#define Instruction_h

#include "data/ByteArray.h"
#include "data/ValueType.h"
#include "data/PrimitiveInterface.h"
#include "system/Convert.h"
#include "xml/XmlTextReader.h"
#include "xml/XmlTextWriter.h"
#include "json/JsonTextReader.h"
#include "json/JsonTextWriter.h"
#include "InstructionDescription.h"
#include "InstructionContext.h"

using namespace Common;

namespace Drivers {
    class Interactive;

    class Device;

    class DriverManager;

    class Instructions;

    class DeviceDescription;

    class Instruction {
    public:
        class LogContext : public IEquatable<LogContext>, public IEvaluation<LogContext> {
        public:
            bool allowInformation;
            bool allowMessage;

            LogContext(bool allowInformation = true, bool allowMessage = false, uint logLength = DefaultLogLength);

            LogContext(const LogContext &value);

            ~LogContext() override;

            bool equals(const LogContext &other) const override;

            void evaluates(const LogContext &other) override;

            uint logLength() const;

            void setLogLength(uint length);

        private:
            uint _logLength;

        private:
            static const uint DefaultLogLength = 256;
            static const uint MinLogLength = 1;
            static const uint MaxLogLength = 128 * 1024;
        };

        Instruction(InstructionDescription *id);

        virtual ~Instruction();

        InstructionDescription *description() const;

        InstructionContext *context() const;

        void setContext(InstructionContext *context);

        virtual InstructionContext *execute(Interactive *interactive, Device *device, InstructionContext *context,
                                            const ByteArray *buffer = NULL) = 0;

        virtual InstructionContext *executeAsync(Interactive *interactive, Device *device, InstructionContext *context);

        virtual bool match(const ByteArray *buffer, DeviceDescription *dd);

    protected:
        void setReceiveInstruction(Instruction *instruction);

        size_t logLength() const;

        void setLogLength(size_t length);

        virtual bool allowLogInformation() const;

        virtual bool allowLogMessage() const;

        void setAllowLog(bool allowInformation, bool allowMessage);

        void log(const ByteArray *buffer, bool send, const Endpoint &endpoint = Endpoint::Empty);

        void log(const ByteArray *buffer, size_t count, bool send, const Endpoint &endpoint = Endpoint::Empty);

        void log(const String &information);

        void logDirectly(const ByteArray *buffer, bool send, const Endpoint &endpoint = Endpoint::Empty);

        void logDirectly(const ByteArray *buffer, size_t count, bool send, const Endpoint &endpoint = Endpoint::Empty);

        void logDirectly(const String &information);

    protected:
        Instruction *_receiveInstruction;

    private:
        friend Device;
        friend Instructions;

        InstructionDescription *_description;

        bool _isExecuting;

        size_t _logLength;
        bool _allowLogInformation;
        bool _allowLogMessage;

    public:
        static const size_t DefaultLogLength = 256;
        static const size_t MinLogLength = 1;
        static const size_t MaxLogLength = 128 * 1024;
    };

//	typedef Vector<Instruction> Instructions;
    class Instructions : public Vector<Instruction> {
    public:
        Instructions(bool autoDelete = true, uint capacity = Vector<Instruction>::DefaultCapacity);

        void setReceiveInstruction(Instructions *instructions);
    };
}

#endif // Instruction_h
