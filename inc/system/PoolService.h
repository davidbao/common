#ifndef POOLSERVICE_H
#define POOLSERVICE_H

#include "data/TimeSpan.h"
#include "data/LoopPList.h"
#include "thread/Timer.h"

namespace System {
    class PoolBaseService {
    public:
        explicit PoolBaseService(const TimeSpan& interval = TimeSpan::fromSeconds(1));

        virtual ~PoolBaseService();

        virtual void start();

        virtual void stop();

        bool isStarted() const;

    protected:
        virtual void invoke();

        void processProc();

    private:
        Timer *_timer;
        TimeSpan _interval;
    };

    class IPoolEntry {
    public:
        IPoolEntry();

        virtual ~IPoolEntry();
    };

    class PoolService : public PoolBaseService {
    public:
        explicit PoolService(bool batch = false, const TimeSpan& interval = TimeSpan::fromSeconds(1), int maxLength = 1024);

        ~PoolService() override;

        virtual void process(const IPoolEntry *value);

        virtual void process(const IPoolEntry **value, size_t count);

        virtual void add(const IPoolEntry *value);

    protected:
        void invoke() override;

    protected:
        LoopPList<IPoolEntry> _values;
        Mutex _valuesMutex;

        bool _batch;
    };
}

#endif // POOLSERVICE_H
