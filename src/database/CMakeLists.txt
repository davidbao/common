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
            DataTable.cpp
            DbClient.cpp
            SqlSelectFilter.cpp
            )

    if (${COMMON_BUILD_SQLITE})
        set(DATABASE_SRC ${DATABASE_SRC} SqliteClient.cpp)
    endif ()

    if (${COMMON_BUILD_MYSQL})
        set(DATABASE_SRC ${DATABASE_SRC} MysqlClient.cpp)
    endif ()

    if (${COMMON_BUILD_ORACLE})
        set(DATABASE_SRC ${DATABASE_SRC} OracleClient.cpp)
    endif ()

    if (${COMMON_BUILD_KINGBASE})
        set(DATABASE_SRC ${DATABASE_SRC} KingbaseClient.cpp)
    endif ()

    add_library(database OBJECT ${DATABASE_SRC})
endif ()
