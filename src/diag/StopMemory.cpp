//
//  StopMemory.cpp
//  common
//
//  Created by baowei on 2020/12/14.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifdef DEBUG
#include "diag/StopMemory.h"
#include "IO/Metrics.h"
#include "diag/Trace.h"

using namespace Diag;

namespace Diag {
    StopMemory::StopMemory(int64_t deadMemory) : StopMemory(String::Empty, deadMemory) {
    }

    StopMemory::StopMemory(const String &info, int64_t deadMemory) : _deadMemory(deadMemory), _startMemory(0),
                                                                     _endMemory(0), _info(info) {
        start(deadMemory);
    }

    StopMemory::~StopMemory() {
        stop();
    }

    void StopMemory::reStart() {
        start(_deadMemory);
    }

    void StopMemory::start(int64_t deadMemory) {
        _deadMemory = deadMemory;
        _startMemory = MemoryStat::virtualSize();
        _endMemory = 0;
    }

    void StopMemory::stop(bool showInfo) {
        if (_endMemory == 0) {
            _endMemory = MemoryStat::virtualSize();

            if (showInfo && !_info.isNullOrEmpty()) {
                int64_t used = this->used();
                if (used >= _deadMemory) {
                    if (used < 1024) {
                        // < 1K
                        Trace::verb(String::format("%s, used: %ld", _info.c_str(), used));
                    } else if (used < 1024 * 1024) {
                        // >= 1K && < 1M
                        Trace::verb(String::format("%s, used: %.1fK", _info.c_str(), (double) used / 1024.0));
                    } else if (used < 1024 * 1024 * 1024) {
                        // >= 1M && < 1G
                        Trace::verb(String::format("%s, used: %.1fM", _info.c_str(), (double) used / 1024.0 / 1024.0));
                    } else {
                        // > 1G
                        Trace::verb(String::format("%s, used: %.2fG", _info.c_str(),
                                                   (double) used / 1024.0 / 1024.0 / 1024.0));
                    }
                }
            }
        }
    }

    void StopMemory::setInfo(const String &info) {
        _info = info;
    }

    const String &StopMemory::info() const {
        return _info;
    }

    int64_t StopMemory::used() const {
        return _endMemory - _startMemory;
    }

    int64_t StopMemory::deadMemory() const {
        return _deadMemory;
    }

    bool StopMemory::isRunning() const {
        return _endMemory == 0;
    }

    void StopMemory::logUsed() {
        MemoryStat::logUsed();
    }
}
#endif  // DEBUG