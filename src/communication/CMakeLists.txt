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

SET(COMMUNICATION_SRC
        BaseCommConfig.cpp
        BaseCommContext.cpp
        BaseCommService.cpp
        ClientInstruction.cpp
        ClientService.cpp
        CommInstructionSet.cpp
        CommSampler.cpp
        ServerInstruction.cpp
        ServerService.cpp
        ServerServices.cpp
        )

add_library(communication OBJECT ${COMMUNICATION_SRC})

