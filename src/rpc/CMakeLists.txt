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

set(RPC_SRC
        # rpc
        RpcClient.cpp
        RpcContext.cpp
        RpcInstruction.cpp
        RpcInstruction.h
        RpcInstructionContext.cpp
        RpcInvoker.cpp
        RpcServer.cpp
        )

add_library(rpc OBJECT ${RPC_SRC})
