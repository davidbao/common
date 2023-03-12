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

cmake_minimum_required(VERSION 3.2.3)
project(common)

option(COMMON_STATIC_STDLIB "Option for build with static libstdc++" OFF)
option(COMMON_ENABLE_TEST "Option for build with test" ON)
option(COMMON_BUILD_DATABASE "Option for build with database" ON)
option(COMMON_BUILD_SQLITE "Option for build with sqlite" ON)
option(COMMON_BUILD_MYSQL "Option for build with mysql" ON)
option(COMMON_BUILD_MYSQL_STATIC "Option for build with mysql static lib" ON)
option(COMMON_BUILD_ORACLE "Option for build with oracle" ON)
option(COMMON_BUILD_KINGBASE "Option for build with Kingbase" OFF)
option(COMMON_BUILD_DRIVER "Option for build with driver" ON)
option(COMMON_BUILD_COMMUNICATION "Option for build with communication" ON)
option(COMMON_BUILD_MICROSERVICE "Option for build with microservice" ON)
option(COMMON_BUILD_NOTIFICATION "Option for build with microservice notification" ON)
option(COMMON_BUILD_HTTPCLIENT "Option for build with http client(use curl)" ON)
option(COMMON_BUILD_HTTPSERVER "Option for build with http server(use libevent)" ON)
option(COMMON_BUILD_REDISCLIENT "Option for build with redis client(use hiredis)" ON)
option(COMMON_BUILD_MQTTCLIENT "Option for build with mqtt client(use paho.mqtt.c)" ON)
option(COMMON_BUILD_OPCUACLIENT "Option for build with opcua client(use open62541)" ON)
option(COMMON_BUILD_IOPORT "Option for build with IO port" ON)

## build settings
set(COMMON_VERSION_MAJOR 3)
set(COMMON_VERSION_MINOR 1)
set(COMMON_VERSION_PATCH 0)
set(COMMON_VERSION ${COMMON_VERSION_MAJOR}.${COMMON_VERSION_MINOR}.${COMMON_VERSION_PATCH})

set(COMMON_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD ${COMMON_CXX_STANDARD})

message(STATUS "CMake version: " ${CMAKE_VERSION})
message(STATUS "CMake system name: " ${CMAKE_SYSTEM_NAME})
message(STATUS "CMake build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "CMake cxx standard: " ${CMAKE_CXX_STANDARD})
message(STATUS "Common lib version: " ${COMMON_VERSION})

set(CMAKE_VERBOSE_MAKEFILE ON)

# declare some tags.
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(OS_NAME emscripten)
    set(WEB_OS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OS_NAME mac)
    set(MAC_OS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
#    message("-- CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
    if (CMAKE_CXX_COMPILER_VERSION STREQUAL "7.5.0")
        set(OS_NAME arm.gcc7.5.0)
    else()
        set(OS_NAME arm)
    endif()
    set(ARM_OS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_NAME linux)
    set(LINUX_OS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(OS_NAME android)
    set(ANDROID_OS ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_NAME win)
    set(WIN_OS ON)
else ()
    set(OS_NAME ${CMAKE_SYSTEM_NAME})
endif ()

# Disable some options on web.
if (WEB_OS)
    set(COMMON_BUILD_MICROSERVICE OFF)
    set(COMMON_BUILD_DATABASE OFF)
    set(COMMON_BUILD_HTTPCLIENT OFF)
    set(COMMON_BUILD_HTTPSERVER OFF)
    set(COMMON_BUILD_REDISCLIENT OFF)
    set(COMMON_BUILD_IOPORT OFF)
endif ()

# Disable some options on android.
if (ANDROID_OS)
    set(COMMON_BUILD_MICROSERVICE OFF)
    set(COMMON_BUILD_MYSQL OFF)
    set(COMMON_BUILD_ORACLE OFF)
    set(COMMON_BUILD_KINGBASE OFF)
    set(COMMON_BUILD_REDISCLIENT OFF)
endif ()

# Disable some options on arm.
if (ARM_OS)
    set(COMMON_BUILD_MICROSERVICE OFF)
    set(COMMON_BUILD_MYSQL OFF)
    set(COMMON_BUILD_ORACLE OFF)
    set(COMMON_BUILD_KINGBASE OFF)
    set(COMMON_BUILD_REDISCLIENT OFF)
endif ()

# show options.
message(STATUS "COMMON_STATIC_STDLIB: " ${COMMON_STATIC_STDLIB})
message(STATUS "COMMON_ENABLE_TEST: " ${COMMON_ENABLE_TEST})
message(STATUS "COMMON_BUILD_DATABASE: " ${COMMON_BUILD_DATABASE})
message(STATUS "COMMON_BUILD_SQLITE: " ${COMMON_BUILD_SQLITE})
message(STATUS "COMMON_BUILD_MYSQL:" ${COMMON_BUILD_MYSQL})
message(STATUS "COMMON_BUILD_MYSQL_STATIC:" ${COMMON_BUILD_MYSQL_STATIC})
message(STATUS "COMMON_BUILD_ORACLE:" ${COMMON_BUILD_ORACLE})
message(STATUS "COMMON_BUILD_KINGBASE:" ${COMMON_BUILD_KINGBASE})
message(STATUS "COMMON_BUILD_DRIVER: " ${COMMON_BUILD_DRIVER})
message(STATUS "COMMON_BUILD_COMMUNICATION: " ${COMMON_BUILD_COMMUNICATION})
message(STATUS "COMMON_BUILD_MICROSERVICE: " ${COMMON_BUILD_MICROSERVICE})
message(STATUS "COMMON_BUILD_NOTIFICATION: " ${COMMON_BUILD_NOTIFICATION})
message(STATUS "COMMON_BUILD_HTTPCLIENT: " ${COMMON_BUILD_HTTPCLIENT})
message(STATUS "COMMON_BUILD_HTTPSERVER: " ${COMMON_BUILD_HTTPSERVER})
message(STATUS "COMMON_BUILD_REDISCLIENT: " ${COMMON_BUILD_REDISCLIENT})
message(STATUS "COMMON_BUILD_MQTTCLIENT: " ${COMMON_BUILD_MQTTCLIENT})
message(STATUS "COMMON_BUILD_OPCUACLIENT: " ${COMMON_BUILD_OPCUACLIENT})
message(STATUS "COMMON_BUILD_IOPORT: " ${COMMON_BUILD_IOPORT})

# set externals path.
set(EXTERNALS_PATH ${CMAKE_SOURCE_DIR}/externals)

# include files.
if (WEB_OS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__EMSCRIPTEN__")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EMSCRIPTEN__")
elseif (WIN_OS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCURL_STATICLIB -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS -wd4996 -wd4244 -wd4819 -wd4200")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCURL_STATICLIB -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS -wd4996 -wd4244 -wd4819 -wd4200")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099 /ignore:4286")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099 /ignore:4286")
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /ignore:4099 /ignore:4286")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
else (${CMAKE_SYSTEM_NAME} STREQUAL "Release")
endif ()

if (COMMON_BUILD_NOTIFICATION)
    ADD_DEFINITIONS(-DHAS_NOTIFICATION=1)
endif ()

if (COMMON_BUILD_KINGBASE)
    ADD_DEFINITIONS(-DHAS_DB_KINGBASE=1)
endif ()

# output path.
if (WIN_OS)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/${OS_NAME}/${CMAKE_VS_PLATFORM_NAME})
else ()
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/${OS_NAME}/${CMAKE_BUILD_TYPE})
endif ()
#message(STATUS "LIBRARY_OUTPUT_PATH: " ${LIBRARY_OUTPUT_PATH})

# src path.
add_subdirectory(src)

# test.
if (COMMON_ENABLE_TEST)
    enable_testing()
    add_subdirectory(tests)
endif ()
