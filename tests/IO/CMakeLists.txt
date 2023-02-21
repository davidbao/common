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

set(IO_SRC
        DirectoryTest.cpp
#        DirectoryInfo.cpp
        FileTest.cpp
#        FileInfo.cpp
        FileStreamTest.cpp
        MappingStreamTest.cpp
        MemoryStreamTest.cpp
        MetricsTest.cpp
#        NamedPipeStream.cpp
        PathTest.cpp
#        Stream.cpp
        ZipTest.cpp
        )

if (COMMON_BUILD_IOPORT)
    SET(IO_SRC
            ${IO_SRC}
            SerialInfoTest.cpp
            )
endif ()

foreach (item ${IO_SRC})
    add_executable(${item} ${item})
    target_link_libraries(${item} ${COMMON_LIB})
    add_test(NAME ${item} COMMAND ${item})
endforeach ()
