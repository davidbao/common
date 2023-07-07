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

if (${COMMON_BUILD_DATABASE})
    SET(DATABASE_SRC
            DataTableTest.cpp
            SqlSelectFilterTest.cpp
            )

    if (${COMMON_BUILD_SQLITE})
        set(DATABASE_SRC ${DATABASE_SRC} SqliteClientTest.cpp)
    endif ()

    if (${COMMON_BUILD_MYSQL})
        set(DATABASE_SRC ${DATABASE_SRC} MysqlClientTest.cpp)
    endif ()

#    if (${COMMON_BUILD_ORACLE})
#        set(DATABASE_SRC ${DATABASE_SRC} OracleClientTest.cpp)
#    endif ()

    if (${COMMON_BUILD_KINGBASE})
        set(DATABASE_SRC ${DATABASE_SRC} KingbaseClientTest.cpp)
    endif ()

    if (${COMMON_BUILD_DM6})
        set(DATABASE_SRC ${DATABASE_SRC} Dm6ClientTest.cpp)
    endif ()

    foreach (item ${DATABASE_SRC})
        string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
        add_executable(${app} ${item})
        target_link_libraries(${app} ${COMMON_LIB})
        add_test(NAME ${app} COMMAND ${app})
    endforeach ()
endif ()
