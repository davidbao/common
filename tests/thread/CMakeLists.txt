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
#        Locker.cpp
#        Mutex.cpp
#        Process.cpp
#        ProcessMutex.cpp
        TaskTimerTest.cpp
#        TickTimeout.cpp
        TimerTest.cpp
        )
if (NOT WEB_OS)
    set(THREAD_SRC ${THREAD_SRC} TaskTest.cpp ThreadTest.cpp)
endif ()

foreach (item ${THREAD_SRC})
    string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
    add_executable(${app} ${item})
    target_link_libraries(${app} ${COMMON_LIB})
    add_test(NAME ${app} COMMAND ${app})
endforeach ()
