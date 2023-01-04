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

set(MICROSERVICE_SRC
        Actuator.cpp
        ClusterService.cpp
        ConsulService.cpp
        DataSourceService.cpp
        HttpService.cpp
        LoadBalancerClient.cpp
        NacosService.cpp
        RestTemplate.cpp
        ServiceDiscovery.cpp
        ServiceGovernance.cpp
        ServiceInstance.cpp
        ServiceRegister.cpp
        SummerApplication.cpp
        SummerStarter.cpp
        SsoService.cpp
        TcpService.cpp
        TcpTemplate.cpp
        )

if (COMMON_BUILD_NOTIFICATION)
    SET(MICROSERVICE_SRC "${MICROSERVICE_SRC}" NotificationService.cpp)
endif ()

add_library(microservice OBJECT ${MICROSERVICE_SRC})
