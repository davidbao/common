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
        )
if (COMMON_BUILD_HTTPCLIENT AND COMMON_BUILD_HTTPSERVER)
    set(HTTP_SRC ${HTTP_SRC}
            HttpContent.cpp
            HttpClient.cpp
            HttpServer.cpp
            )
    add_library(http OBJECT ${HTTP_SRC})
elseif (COMMON_BUILD_HTTPCLIENT)
    set(HTTP_SRC ${HTTP_SRC}
            HttpContent.cpp
            HttpClient.cpp
            )
    add_library(http OBJECT ${HTTP_SRC})
elseif (COMMON_BUILD_HTTPSERVER)
    set(HTTP_SRC ${HTTP_SRC}
            HttpContent.cpp
            HttpServer.cpp
            )
    add_library(http OBJECT ${HTTP_SRC})
else ()
endif ()
