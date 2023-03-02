#*******************************************************************************
#  Copyright (c) 2022 com.
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the EaseSuite Public License v1.0
#  and EaseSuite Distribution License v1.0 which accompany this distribution.
#
#  Contributors:
#     Baowei - initial version
#*******************************************************************************

# Note: on OS X you should install XCode and the associated command-line tools

set(THREAD_SRC
        # thread
        Locker.cpp
        Mutex.cpp
        Process.cpp
        ProcessMutex.cpp
        TaskTimer.cpp
        Thread.cpp
        ThreadPool.cpp
        TickTimeout.cpp
        Timer.cpp
        )

if (NOT WEB_OS)
    set(THREAD_SRC ${THREAD_SRC} Thread.cpp ThreadPool.cpp)
endif ()

add_library(thread OBJECT ${THREAD_SRC})
