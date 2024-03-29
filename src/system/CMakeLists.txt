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

set(SYSTEM_SRC
        Application.cpp
        BCDProvider.cpp
        CheckProvider.cpp
        Clipboard.cpp
        Console.cpp
        Delegate.cpp
        DynamicLoader.cpp
        Environment.cpp
        LocalTime.cpp
        Math.cpp
        NameService.cpp
        PoolService.cpp
        Property.cpp
        Random.cpp
        Regex.cpp
        Resources.cpp
        ServiceFactory.cpp
        )

if (WIN_OS)
    SET(SYSTEM_SRC
            ${SYSTEM_SRC}
            getopt.cpp
            )
elseif (WEB_OS)
    SET(SYSTEM_SRC
            ${SYSTEM_SRC}
            WebApplication.cpp
            WebStarter.cpp)
elseif (ANDROID_OS)
    SET(SYSTEM_SRC
            ${SYSTEM_SRC}
            PhoneApplication.cpp)
else ()
endif ()

add_library(system OBJECT ${SYSTEM_SRC})
