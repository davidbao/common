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

if (COMMON_ENABLE_TEST)
    set(CMAKE_CXX_STANDARD ${COMMON_CXX_STANDARD})

    # include settings
    include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include)
    include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include/libxml2)
    include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include/libiconv)
    include_directories(${EXTERNALS_PATH}/include)
    include_directories(${CMAKE_SOURCE_DIR}/inc)

    # link settings
    if (WIN_OS)
        link_directories(${EXTERNALS_PATH}/install/${OS_NAME}/lib/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_BUILD_TYPE})
        link_directories(${EXTERNALS_PATH}/lib/${OS_NAME}/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_BUILD_TYPE})
    else ()
        link_directories(${EXTERNALS_PATH}/install/${OS_NAME}/lib)
        link_directories(${EXTERNALS_PATH}/lib/${OS_NAME})
    endif ()

    # lib settings
    if (WIN32 OR MSVC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    elseif (UNIX)
        # Only search static lib.
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif ()
    set(EXTERNALS_LIB
            zip${CMAKE_FIND_LIBRARY_SUFFIXES}
            yaml-cpp${CMAKE_FIND_LIBRARY_SUFFIXES}
            bz2${CMAKE_FIND_LIBRARY_SUFFIXES}
            xml2${CMAKE_FIND_LIBRARY_SUFFIXES}
            z${CMAKE_FIND_LIBRARY_SUFFIXES}
            ssl${CMAKE_FIND_LIBRARY_SUFFIXES}
            crypto${CMAKE_FIND_LIBRARY_SUFFIXES}
            json${CMAKE_FIND_LIBRARY_SUFFIXES})

    if (COMMON_STATIC_STDLIB)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} stdc++${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif ()

    if (MAC_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv lzma)
    elseif (LINUX_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} uuid pthread dl rt lzma)
    elseif (ARM_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} uuid${CMAKE_FIND_LIBRARY_SUFFIXES} pthread dl)
    elseif (ANDROID_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv uuid log)
    elseif (WIN_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv Shlwapi ws2_32 Rpcrt4 shell32 advapi32 Iphlpapi Crypt32 Wldap32)
    elseif (WEB_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB})
    endif ()

    if (COMMON_BUILD_HTTPCLIENT)
        set(EXTERNALS_LIB curl${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
    endif ()

    if (COMMON_BUILD_HTTPSERVER)
        set(EXTERNALS_LIB
                event_openssl${CMAKE_FIND_LIBRARY_SUFFIXES}
                event${CMAKE_FIND_LIBRARY_SUFFIXES}
                ${EXTERNALS_LIB}
                )
        if (NOT WIN_OS)
            set(EXTERNALS_LIB
                    event_pthreads${CMAKE_FIND_LIBRARY_SUFFIXES}
                    ${EXTERNALS_LIB}
                    )
        endif ()
    endif ()

    if (COMMON_BUILD_MQTTCLIENT)
        set(EXTERNALS_LIB
                ${EXTERNALS_LIB}
                paho-mqtt3a-static${CMAKE_FIND_LIBRARY_SUFFIXES}
                paho-mqtt3as-static${CMAKE_FIND_LIBRARY_SUFFIXES}
                paho-mqtt3c-static${CMAKE_FIND_LIBRARY_SUFFIXES}
                paho-mqtt3cs-static${CMAKE_FIND_LIBRARY_SUFFIXES}
                )
    endif()

    if (COMMON_BUILD_DATABASE)
        if (COMMON_BUILD_SQLITE)
            set(EXTERNALS_LIB sqlite3${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
        endif ()
        if (COMMON_BUILD_MYSQL)
            if (COMMON_BUILD_MYSQL_STATIC)
                set(EXTERNALS_LIB mysqlclient${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
            else ()
                set(EXTERNALS_LIB mysqlclient ${EXTERNALS_LIB})
            endif ()

        endif ()
        if (COMMON_BUILD_ORACLE)
        endif ()
        if (COMMON_BUILD_KINGBASE)
            if (WIN_OS)
                set(EXTERNALS_LIB kci ${EXTERNALS_LIB})
            else ()
                set(EXTERNALS_LIB kci crypto ${EXTERNALS_LIB})
            endif ()
        endif ()
        if (COMMON_BUILD_DM6)
            if (COMMON_BUILD_DM6_STATIC)
                if (WIN_OS)
                    set(EXTERNALS_LIB dmapi${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
                else ()
                    set(EXTERNALS_LIB dmapi${CMAKE_FIND_LIBRARY_SUFFIXES} crypto ${EXTERNALS_LIB})
                endif ()
            else ()
                set(EXTERNALS_LIB dmapi ${EXTERNALS_LIB})
            endif ()
        endif ()
        if (COMMON_BUILD_DM7)
            if (COMMON_BUILD_DM7_STATIC)
                set(EXTERNALS_LIB dmdpi${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
            else ()
                set(EXTERNALS_LIB dmdpi ${EXTERNALS_LIB})
            endif ()
        endif ()
    endif ()

    set(COMMON_LIB common ${EXTERNALS_LIB})
    #    message(STATUS "COMMON_LIB: " ${COMMON_LIB})

    if (MAC_OS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Foundation -framework SystemConfiguration")
    endif ()

    if (WEB_OS)
    endif ()

    # application settings
    set(TEST_BASE_SRC configuration crypto data diag exception http IO json net system thread xml yml)
    if (COMMON_BUILD_COMMUNICATION)
        set(TEST_BASE_SRC ${TEST_BASE_SRC} communication)
    endif ()
    if (COMMON_BUILD_DATABASE)
        set(TEST_BASE_SRC ${TEST_BASE_SRC} database)
    endif ()
    if (COMMON_BUILD_DRIVER OR COMMON_BUILD_COMMUNICATION)
        set(TEST_BASE_SRC ${TEST_BASE_SRC} driver)
    endif ()
    if (COMMON_BUILD_MICROSERVICE)
        set(TEST_BASE_SRC ${TEST_BASE_SRC} microservice)
        set(TEST_BASE_SRC ${TEST_BASE_SRC} rpc)
    endif ()

    foreach (SRC ${TEST_BASE_SRC})
        add_subdirectory(${SRC})
    endforeach ()
endif ()
