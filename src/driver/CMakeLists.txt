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

set(DRIVER_SRC
        Context.cpp
        DriverDescription.cpp
        DriverManager.cpp
        channels/Channel.cpp
        channels/ChannelContext.cpp
        channels/ChannelDescription.cpp
        channels/Interactive.cpp
        channels/MockInteractive.cpp
        channels/TcpBackgroundReceiver.cpp
        channels/TcpBackgroundSender.cpp
        channels/TcpChannelContext.cpp
        channels/TcpInteractive.cpp
        channels/TcpServerChannelContext.cpp
        channels/TcpServerInteractive.cpp
        channels/UdpInteractive.cpp
        channels/UdpServerInteractive.cpp
        devices/Device.cpp
        devices/InstructionPool.cpp
        devices/Sampler.cpp
        instructions/Instruction.cpp
        )

if (NOT WEB_OS)
    set(DRIVER_SRC
            ${DRIVER_SRC}
            channels/BluetoothInteractive.cpp
            channels/BluetoothServerClient.cpp
            channels/BluetoothServerInteractive.cpp
            channels/ParallelChannelContext.cpp
            channels/ParallelInteractive.cpp
            channels/SerialChannelContext.cpp
            channels/SerialInteractive.cpp
            channels/SerialServerInteractive.cpp
            )
endif ()

add_library(driver OBJECT ${DRIVER_SRC})
