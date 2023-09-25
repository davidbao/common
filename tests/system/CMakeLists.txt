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
        ActionTest.cpp
        ApplicationTest.cpp
#        BCDUtilities.cpp
        CheckProviderTest.cpp
#        Clipboard.cpp
#        Console.cpp
        DelegateTest.cpp
#        DynamicLoader.cpp
        EnvironmentTest.cpp
#        LocalTime.cpp
        MathTest.cpp
#        NameService.cpp
#        PoolService.cpp
#        Property.cpp
        RandomTest.cpp
        RegexTest.cpp
        ResourcesTest.cpp
        ServiceFactoryTest.cpp
        )

#if (WIN_OS)
#    SET(SYSTEM_SRC
#            ${SYSTEM_SRC}
#            getopt.cpp
#            )
#elseif (WEB_OS)
#    SET(SYSTEM_SRC
#            ${SYSTEM_SRC}
#            WebApplication.cpp
#            WebStarter.cpp)
#elseif (ANDROID_OS)
#    SET(SYSTEM_SRC
#            ${SYSTEM_SRC}
#            PhoneApplication.cpp)
#else ()
#endif ()

foreach (item ${SYSTEM_SRC})
    string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
    add_executable(${app} ${item})
    target_link_libraries(${app} ${COMMON_LIB})
    add_test(NAME ${app} COMMAND ${app})
endforeach ()
