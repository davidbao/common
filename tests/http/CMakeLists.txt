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
    add_executable(${item} ${item})
    target_link_libraries(${item} ${COMMON_LIB})
    add_test(NAME ${item} COMMAND ${item})
endforeach ()
