//
//  StopMemory.h
//  common
//
//  Created by baowei on 2020/12/14.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef StopMemory_h
#define StopMemory_h

#include "data/String.h"

using namespace Data;

namespace Diag {
    class StopMemory {
    public:
        explicit StopMemory(int64_t deadMemory = 0);

        explicit StopMemory(const String &info, int64_t deadMemory = 0);

        ~StopMemory();

        void reStart();

        void start(int64_t deadMemory = 0);

        void stop(bool showInfo = true);

        void setInfo(const String &info);

        const String &info() const;

        int64_t used() const;

        int64_t deadMemory() const;

        bool isRunning() const;

    public:
        static void logUsed();

    private:
        int64_t _deadMemory;
        int64_t _startMemory;
        int64_t _endMemory;
        String _info;
    };
}

#endif /* StopMemory_h */
