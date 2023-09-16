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

set(DIAG_SRC
        # diag
        FileTraceListener.cpp
        MemoryTraceListener.cpp
        Process.cpp
        ProcessMutex.cpp
        Stopwatch.cpp
        Trace.cpp
        TraceListener.cpp
        )

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    SET(DIAG_SRC "${DIAG_SRC}" StopMemory.cpp)
endif ()

add_library(diag OBJECT ${DIAG_SRC})
