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

set(DATA_SRC
        ArrayTest.cpp
        ByteArrayTest.cpp
        ConvertTest.cpp
        DateTimeTest.cpp
        DictionaryTest.cpp
        ListTest.cpp
        LoopListTest.cpp
        LoopVectorTest.cpp
        MapTest.cpp
        PListTest.cpp
        PointTest.cpp
        RectangleTest.cpp
        SizeTest.cpp
        StringArrayTest.cpp
        StringMapTest.cpp
        StringTest.cpp
        TimeSpanTest.cpp
        TimeZoneTest.cpp
        TypeInfoTest.cpp
        UuidTest.cpp
        ValueTypeTest.cpp
        VariantTest.cpp
        VectorTest.cpp
        VersionTest.cpp
        WStringTest.cpp
        )

foreach (item ${DATA_SRC})
    string(REGEX MATCH "[0-9A-Za-z_]*" app ${item})
    add_executable(${app} ${item})
    target_link_libraries(${app} ${COMMON_LIB})
    add_test(NAME ${app} COMMAND ${app})
endforeach ()
