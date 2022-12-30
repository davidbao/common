#ifndef POOLSERVICE_H
#define POOLSERVICE_H

#include "data/TimeSpan.h"
#include "data/LoopPList.h"
#include "thread/Thread.h"

namespace System {
    class PoolBaseService {
    public:
        PoolBaseService(TimeSpan interval = TimeSpan::fromSeconds(1));

        virtual ~PoolBaseService();

        virtual void start();

        virtual void stop();

        bool isStarted() const;

    protected:
        virtual void invoke();

        static void processProc(void *parameter);

        void processProcInner();

    private:
        Thread *_thread;
        TimeSpan _interval;
    };

    class IPoolEntry {
    public:
        IPoolEntry();

        virtual ~IPoolEntry();
    };

    class PoolService : public PoolBaseService {
    public:
        PoolService(bool batch = false, TimeSpan interval = TimeSpan::fromSeconds(1), int maxLength = 1024);

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
