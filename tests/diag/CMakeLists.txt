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
        FileTraceListenerTest.cpp
        MemoryTraceListenerTest.cpp
#        StopMemory.cpp
#        Stopwatch.cpp
#        Trace.cpp
#        TraceListener.cpp
        )

foreach (item ${DIAG_SRC})
    string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
    add_executable(${app} ${item})
    target_link_libraries(${app} ${COMMON_LIB})
    add_test(NAME ${app} COMMAND ${app})
endforeach ()
