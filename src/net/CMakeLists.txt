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

set(NET_SRC
        Dns.cpp
        EthernetInfo.cpp
        NetInterface.cpp
        NetType.cpp
        Receiver.cpp
        Sender.cpp
        TcpClient.cpp
        TcpServer.cpp
        UdpClient.cpp
        UdpServer.cpp
        )

if (COMMON_BUILD_REDISCLIENT)
    SET(NET_SRC "${NET_SRC}" RedisClient.cpp)
endif ()
if (COMMON_BUILD_MQTTCLIENT)
    SET(NET_SRC "${NET_SRC}" MqttClient.cpp)
endif ()
if (COMMON_BUILD_OPCUACLIENT)
    SET(NET_SRC "${NET_SRC}" OpcuaContext.cpp OpcuaClient.cpp)
endif ()

if (NOT WEB_OS)
    SET(NET_SRC "${NET_SRC}" BluetoothClient.cpp BluetoothServer.cpp)
endif()

ADD_LIBRARY(net OBJECT ${NET_SRC})
