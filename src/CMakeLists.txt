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

# link settings
link_directories(${EXTERNALS_PATH}/install/${OS_NAME}/lib)
link_directories(${EXTERNALS_PATH}/lib)

# object settings
add_library(common STATIC)

set(COMMON_BASE_SRC configuration crypto data diag exception http IO json net system thread xml yml)
foreach (SRC ${COMMON_BASE_SRC})
    add_subdirectory(${SRC})
endforeach ()
target_link_libraries(common ${COMMON_BASE_SRC})

if (COMMON_BUILD_COMMUNICATION)
    add_subdirectory(communication)
    target_link_libraries(common communication)
endif ()
if (COMMON_BUILD_DATABASE)
    add_subdirectory(database)
    target_link_libraries(common database)
endif ()
if (COMMON_BUILD_DRIVER OR COMMON_BUILD_COMMUNICATION)
    add_subdirectory(driver)
    target_link_libraries(common driver)
endif ()
if (COMMON_BUILD_MICROSERVICE)
    add_subdirectory(microservice)
    target_link_libraries(common microservice)
    add_subdirectory(rpc)
    target_link_libraries(common rpc)
endif ()

# include settings
install(TARGETS common ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc DESTINATION include)
