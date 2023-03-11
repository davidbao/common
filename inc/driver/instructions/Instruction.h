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
#include "data/String.h"
#include "data/DataInterface.h"
#include "data/Convert.h"
#include "xml/XmlTextReader.h"
#include "xml/XmlTextWriter.h"
#include "json/JsonTextReader.h"
#include "json/JsonTextWriter.h"
#include "InstructionDescription.h"
#include "InstructionContext.h"

using namespace Data;
using namespace Net;

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

            LogContext(bool allowInformation = true, bool allowMessage = false, uint32_t logLength = DefaultLogLength);

            LogContext(const LogContext &value);

            ~LogContext() override;

            bool equals(const LogContext &other) const override;

            void evaluates(const LogContext &other) override;

            uint32_t logLength() const;

            void setLogLength(uint32_t length);

        private:
            uint32_t _logLength;

        private:
            static const uint32_t DefaultLogLength = 256;
            static const uint32_t MinLogLength = 1;
            static const uint32_t MaxLogLength = 128 * 1024;
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

//	typedef PList<Instruction> Instructions;
    class Instructions : public PList<Instruction> {
    public:
        Instructions(bool autoDelete = true, uint32_t capacity = PList<Instruction>::DefaultCapacity);

        void setReceiveInstruction(Instructions *instructions);
    };
}

#endif // Instruction_h
