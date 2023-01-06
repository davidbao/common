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

# include settings
include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include)
include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include/libxml2)
include_directories(${EXTERNALS_PATH}/install/${OS_NAME}/include/libiconv)
include_directories(${EXTERNALS_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/inc)

# object settings
set(COMMON_BASE_SRC configuration crypto data diag exception http IO json net system thread xml yml)
if (COMMON_BUILD_COMMUNICATION)
    set(COMMON_BASE_SRC ${COMMON_BASE_SRC} communication)
endif ()
if (COMMON_BUILD_DATABASE)
    set(COMMON_BASE_SRC ${COMMON_BASE_SRC} database)
endif ()
if (COMMON_BUILD_DRIVER OR COMMON_BUILD_COMMUNICATION)
    set(COMMON_BASE_SRC ${COMMON_BASE_SRC} driver)
endif ()
if (COMMON_BUILD_MICROSERVICE)
    set(COMMON_BASE_SRC ${COMMON_BASE_SRC} microservice)
    set(COMMON_BASE_SRC ${COMMON_BASE_SRC} rpc)
endif ()

set(COMMON_OBJECTS)
foreach (SRC ${COMMON_BASE_SRC})
    add_subdirectory(${SRC})
    set(COMMON_OBJECTS ${COMMON_OBJECTS} $<TARGET_OBJECTS:${SRC}>)
endforeach ()

add_library(common STATIC ${COMMON_OBJECTS})

# install settings
install(TARGETS common ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc DESTINATION include)
