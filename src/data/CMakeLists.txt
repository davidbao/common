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

SET(DATA_SRC
        ByteArray.cpp
        Convert.cpp
        Culture.cpp
        DateTime.cpp
        Enum.cpp
        NetType.cpp
        Point.cpp
        PositionCoord.cpp
        PrimitiveInterface.cpp
        ValueType.cpp
        Rectangle.cpp
        Size.cpp
        String.cpp
        StringArray.cpp
        StringMap.cpp
        TimeSpan.cpp
        TimeZone.cpp
        Uuid.cpp
        ValueType.cpp
        Variant.cpp
        Version.cpp
        WString.cpp
        )

add_library(data OBJECT ${DATA_SRC})
