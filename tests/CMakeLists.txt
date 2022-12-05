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

    if (MAC_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv)
    elseif (LINUX_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} uuid pthread dl)
    elseif (ARM_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} uuid${CMAKE_FIND_LIBRARY_SUFFIXES} pthread dl)
    elseif (ANDROID_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv uuid log)
    elseif (WIN_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB} iconv Shlwapi ws2_32 Rpcrt4 shell32 advapi32 Iphlpapi Crypt32 Wldap32)
    elseif (WEB_OS)
        set(EXTERNALS_LIB ${EXTERNALS_LIB})
    endif ()

    if (${COMMON_BUILD_HTTPCLIENT})
        set(EXTERNALS_LIB curl${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
    endif()

    if (${COMMON_BUILD_HTTPSERVER})
        set(EXTERNALS_LIB
                event_openssl${CMAKE_FIND_LIBRARY_SUFFIXES}
                event${CMAKE_FIND_LIBRARY_SUFFIXES}
                ${EXTERNALS_LIB}
                )
    endif()

    if (${COMMON_BUILD_DATABASE})
        if (${COMMON_BUILD_SQLITE})
            set(EXTERNALS_LIB sqlite3${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
        endif ()
        if (${COMMON_BUILD_MYSQL})
            set(EXTERNALS_LIB mysqlclient${CMAKE_FIND_LIBRARY_SUFFIXES} ${EXTERNALS_LIB})
        endif ()
        if (${COMMON_BUILD_ORACLE})
        endif ()
    endif ()

    set(COMMON_LIB common ${EXTERNALS_LIB})
#    message(STATUS "COMMON_LIB: " ${COMMON_LIB})

    # application settings
    set(TEST_SRC
            # configuration
            ConfigServiceTest
            # crypto
            SmProviderTest Md5ProviderTest
            # data
            ArrayTest ByteArrayTest DateTimeTest DictionaryTest ListTest LoopListTest LoopVectorTest MapTest RectangleTest
            SizeTest StringTest StringArrayTest StringMapTest PListTest PointTest NetTypeTest TypeInfoTest TimeSpanTest
            TimeZoneTest UuidTest ValueTypeTest VariantTest VectorTest VersionTest WStringTest
            # diag
            FileTraceListenerTest
            # IO
            FileTest DirectoryTest MappingStreamTest
            # json
            JsonNodeTest
            # system
            RandomTest ResourcesTest EnvironmentTest
            # xml
            XmlDocumentTest XmlTextReaderTest)

    if (${COMMON_BUILD_HTTPCLIENT} AND ${COMMON_BUILD_HTTPSERVER})
        set(TEST_SRC ${TEST_SRC} HttpClientTest)
    endif ()
    if (${COMMON_BUILD_MICROSERVICE})
        set(TEST_SRC ${TEST_SRC} SummerApplicationTest)
    endif ()

    if (${COMMON_BUILD_DATABASE})
        set(TEST_SRC ${TEST_SRC} DataTableTest SqlSelectFilterTest)
        if (${COMMON_BUILD_SQLITE})
            set(TEST_SRC ${TEST_SRC} SqliteClientTest)
        endif ()
        if (${COMMON_BUILD_MYSQL})
            set(TEST_SRC ${TEST_SRC} MysqlClientTest)
        endif ()
        if (${COMMON_BUILD_ORACLE})
        endif ()
    endif ()

    if (MAC_OS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Foundation -framework SystemConfiguration")
    endif ()

    set(CMAKE_CXX_FLAGS_TEMP ${CMAKE_CXX_FLAGS})
    foreach (item ${TEST_SRC})
        add_executable(${item} ${item}.cpp)
        target_link_libraries(${item} ${COMMON_LIB})
        add_test(NAME ${item} COMMAND ${item})
    endforeach ()
endif ()
