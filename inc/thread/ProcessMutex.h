//
//  ProcessMutex.h
//  common
//
//  Created by baowei on 16/4/29.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef ProcessMutex_h
#define ProcessMutex_h

#include "data/ValueType.h"
#ifndef WIN32
#include <semaphore.h>
#endif

namespace Common
{
    class ProcessMutex
    {
    public:
        ProcessMutex(const String& name = String::Empty);
        ~ProcessMutex();
        
        bool lock();
        bool unlock();
        
    public:
        static bool exists(const String& name);
        
    private:
#if WIN32
        void* _mutex;
#else
        sem_t* _mutex;
#endif
        String _mutexName;
    };
}

#endif /* ProcessMutex_h */
