//
//  ProcessMutex.cpp
//  common
//
//  Created by baowei on 2016/4/29.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "diag/ProcessMutex.h"
#include "diag/Trace.h"

#ifdef WIN32
#include <Windows.h>
#else

#include <cerrno>
#include <unistd.h>
#include <semaphore.h>
#include <cstdio>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <memory.h>

#endif

namespace Diag {
    ProcessMutex::ProcessMutex(const String &name) {
        _mutexName = name;
#ifdef WIN32
        _mutex = CreateMutex(NULL, false, name.c_str());
#else
        _mutex = sem_open(name, O_RDWR | O_CREAT, 0644, 1);
#endif
    }

    ProcessMutex::~ProcessMutex() {
#ifdef WIN32
        CloseHandle(_mutex);
#else
        int ret = sem_close(_mutex);
        if (0 != ret) {
            printf("sem_close error %d\n", ret);
        }
        sem_unlink(_mutexName);
#endif
    }

    bool ProcessMutex::lock() {
#ifdef WIN32
        if (nullptr == _mutex)
        {
            return false;
        }
        
        DWORD nRet = WaitForSingleObject(_mutex, INFINITE);
        if (nRet != WAIT_OBJECT_0)
        {
            return false;
        }
#else
        int ret = sem_wait(_mutex);
        if (ret != 0) {
            return false;
        }
#endif
        return true;
    }

    bool ProcessMutex::unlock() {
#ifdef WIN32
        return ReleaseMutex(_mutex);
#else
        int ret = sem_post(_mutex);
        if (ret != 0) {
            return false;
        }
        return true;
#endif
    }

    bool ProcessMutex::exists(const String &name) {
#ifdef WIN32
        HANDLE hMutex = CreateMutex(0, 0, name);
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            // The mutex exists so this is the
            // the second instance so return.
            ReleaseMutex(hMutex);
            Debug::writeLine("Process already running!");
            return true;
        }
        return false;
#else
        struct flock fl{};

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 1;

        int fdlock;
        String fileName = String::convert("/tmp/oneproc_%s.lock", name.c_str());
        if ((fdlock = open(fileName.c_str(), O_WRONLY | O_CREAT, 0666)) == -1)
            return false;

        if (fcntl(fdlock, F_SETLK, &fl) == -1) {
            Debug::writeLine("Process already running!");
            return true;
        }

        return false;
#endif
    }
}
