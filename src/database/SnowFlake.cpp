//
//  SnowFlake.cpp
//  common
//
//  Created by baowei on 2023/7/22.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/SnowFlake.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"

using namespace Threading;

namespace Database {
// Twitter snowflake
    /*-------high----------------------------------------64 bits-------------------------------------------low-------|
    |----------------------------------------------------------------------------------------------------------------|
    |   0  | 0000000000 0000000000 0000000000 0000000000 0 |         00000       |       00000      |   000000000000 |
    |unused|             41 bits timestamp                 |5 bits data center ID| 5 bits worker ID | 12 bits seq ID |
    |----------------------------------------------------------------------------------------------------------------|
    |unused|             41 bits timestamp                 |5 bits data center ID| 5 bits worker ID | 12 bits seq ID |
    |---------------------------------------------------------------------------------------------------------------*/
    // https://www.cnblogs.com/Keeping-Fit/p/15025402.html
    uint64_t SnowFlake::generateId(int dataCenterId, int workerId) {
        constexpr int SEQUENCE_BITS = 12;
        constexpr int WORKER_ID_BITS = 5;
        constexpr int DATA_CENTER_ID_BITS = 5;

        constexpr int SEQUENCE_ID_SHIFT = 0;
        constexpr int WORK_ID_SHIFT = SEQUENCE_BITS;
        constexpr int DATA_CENTER_ID_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS;
        constexpr int TIMESTAMP_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS + DATA_CENTER_ID_BITS;

        constexpr uint64_t MAX_WORKER_ID = (1 << WORKER_ID_BITS) - 1;
        constexpr uint64_t MAX_DATA_CENTER_ID = (1 << DATA_CENTER_ID_BITS) - 1;

        constexpr uint64_t SEQUENCE_MASK = (1 << SEQUENCE_BITS) - 1;

        if (dataCenterId < 0 || MAX_DATA_CENTER_ID < dataCenterId) {
            return 0;
        }
        if (workerId < 0 || MAX_WORKER_ID < workerId) {
            return 0;
        }

        static Mutex mutex;
        Locker locker(&mutex);

        static uint32_t startTick = TickTimeout::getCurrentTickCount();
        static uint64_t startMilliseconds = (uint64_t) DateTime::total2010Milliseconds(DateTime::now());
        static uint64_t lastTimestamp = 0;
        static uint64_t sequenceId = 0;

        uint32_t currentTick = TickTimeout::getCurrentTickCount();
        if (currentTick <= startTick) {
            startTick = TickTimeout::getCurrentTickCount();
            startMilliseconds = (uint64_t) DateTime::total2010Milliseconds(DateTime::now());
            currentTick = startTick;
        }

        uint64_t timestamp = startMilliseconds + (currentTick - startTick);
        if (timestamp == lastTimestamp) {
            sequenceId = (sequenceId + 1) & SEQUENCE_MASK;

            if (0 == sequenceId) {
                Thread::msleep(1);
                timestamp++;
            }
        } else {
            sequenceId = 0;
        }
        lastTimestamp = timestamp;
        return (timestamp << TIMESTAMP_SHIFT)
               | (dataCenterId << DATA_CENTER_ID_SHIFT)
               | (workerId << WORK_ID_SHIFT)
               | (sequenceId << SEQUENCE_ID_SHIFT);
    }

    uint64_t SnowFlake::generateId(int workerId) {
        return generateId(0, workerId);
    }

    uint64_t SnowFlake::generateId() {
        return generateId(0, 0);
    }
}
