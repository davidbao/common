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
        Directory.cpp
        DirectoryInfo.cpp
        File.cpp
        FileInfo.cpp
        FileStream.cpp
        MappingStream.cpp
        MemoryStream.cpp
        Metrics.cpp
        NamedPipeStream.cpp
        Path.cpp
        Stream.cpp
        Zip.cpp
        )

if (COMMON_BUILD_IOPORT)
    SET(IO_SRC
            ${IO_SRC}
            IOPort.cpp
            ParallelPort.cpp
            SerialInfo.cpp
            SerialPort.cpp
            )
endif ()

add_library(IO OBJECT ${IO_SRC})
