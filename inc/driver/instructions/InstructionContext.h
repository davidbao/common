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

using namespace Common;

namespace Drivers {
    class InstructionContext : public Context {
    public:
        InstructionContext();

        ~InstructionContext() override;

        void setException(Exception *exception);

        void clearException();

        Exception *exception() const;

        bool hasException() const;

        void setTimeStamp(uint stamp);

        uint getTimeStamp() const;

        void setQualityStamp(ushort stamp);

        ushort getQualityStamp() const;

        uint receiveTimeout() const;

        void setReceiveTimeout(uint timeout);

        virtual bool allowExecution() const;

    private:
        Exception *_exception;
        uint _timestamp;
        ushort _qualitystamp;
        uint _receiveTimeout;

        static const int Good = 0;
        static const int Bad = 1;
    };
}

#endif // InstructionContext_h
