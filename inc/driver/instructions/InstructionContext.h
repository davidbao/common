//
//  InstructionContext.h
//  common
//
//  Created by baowei on 2015/9/13.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef InstructionContext_h
#define InstructionContext_h

#include "exception/Exception.h"
#include "driver/Context.h"

using namespace Data;
using namespace System;

namespace Drivers {
    class InstructionContext : public Context {
    public:
        InstructionContext();

        ~InstructionContext() override;

        void setException(Exception *exception);

        void clearException();

        Exception *exception() const;

        bool hasException() const;

        void setTimeStamp(uint32_t stamp);

        uint32_t getTimeStamp() const;

        void setQualityStamp(uint16_t stamp);

        uint16_t getQualityStamp() const;

        uint32_t receiveTimeout() const;

        void setReceiveTimeout(uint32_t timeout);

        virtual bool allowExecution() const;

    private:
        Exception *_exception;
        uint32_t _timestamp;
        uint16_t _qualitystamp;
        uint32_t _receiveTimeout;

        static const int Good = 0;
        static const int Bad = 1;
    };
}

#endif // InstructionContext_h
