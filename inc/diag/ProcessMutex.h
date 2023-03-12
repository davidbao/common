//
//  ProcessMutex.h
//  common
//
//  Created by baowei on 2016/4/29.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef ProcessMutex_h
#define ProcessMutex_h

#include "data/String.h"

#ifndef WIN32

#include <semaphore.h>

#endif

using namespace Data;

namespace Diag {
    class ProcessMutex {
    public:
        explicit ProcessMutex(const String &name = String::Empty);

        ~ProcessMutex();

        bool lock();

        bool unlock();

    public:
        static bool exists(const String &name);

    private:
#ifdef WIN32
        void* _mutex;
#else
        sem_t *_mutex;
#endif
        String _mutexName;
    };
}

#endif /* ProcessMutex_h */
