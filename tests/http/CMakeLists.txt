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

set(HTTP_SRC
#        HttpContentTest.cpp
        )
if (COMMON_BUILD_HTTPCLIENT AND COMMON_BUILD_HTTPSERVER)
    set(HTTP_SRC ${HTTP_SRC}
            HttpClientTest.cpp
#            HttpServer.cpp
            )
elseif (COMMON_BUILD_HTTPCLIENT)
    set(HTTP_SRC ${HTTP_SRC}
            HttpClientTest.cpp
            )
elseif (COMMON_BUILD_HTTPSERVER)
    set(HTTP_SRC ${HTTP_SRC}
#            HttpServer.cpp
            )
else ()
endif ()

foreach (item ${HTTP_SRC})
    string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
    add_executable(${app} ${item})
    target_link_libraries(${app} ${COMMON_LIB})
    add_test(NAME ${app} COMMAND ${app})
endforeach ()
