//
//  Mutex.h
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef Mutex_h
#define Mutex_h

#include <mutex>

using namespace std;

namespace Threading {
    class IMutex {
    public:
        IMutex();

        virtual ~IMutex();

        virtual void lock() = 0;

        virtual bool tryLock() = 0;

        virtual void unlock() = 0;
    };

    class Mutex : public IMutex {
    public:
        Mutex();

        ~Mutex() override;

        void lock() override;

        bool tryLock() override;

        void unlock() override;

    private:
        mutex *_mutex;
    };

    class RecursiveMutex : public IMutex {
    public:
        RecursiveMutex();

        ~RecursiveMutex() override;

        void lock() override;

        bool tryLock() override;

        void unlock() override;

    private:
        recursive_mutex *_mutex;
    };
}

#endif // Mutex_h
