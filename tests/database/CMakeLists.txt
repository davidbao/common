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

    foreach (item ${DATABASE_SRC})
        add_executable(${item} ${item})
        target_link_libraries(${item} ${COMMON_LIB})
        add_test(NAME ${item} COMMAND ${item})
    endforeach ()
endif ()
