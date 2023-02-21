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
#        Actuator.cpp
#        ClusterService.cpp
#        ConsulService.cpp
#        DataSourceService.cpp
#        HttpService.cpp
#        LoadBalancerClient.cpp
#        NacosService.cpp
#        RestTemplate.cpp
#        ServiceDiscovery.cpp
#        ServiceGovernance.cpp
#        ServiceInstance.cpp
#        ServiceRegister.cpp
        SummerApplicationTest.cpp
#        SummerStarter.cpp
#        SsoService.cpp
#        TcpService.cpp
#        TcpTemplate.cpp
        )

if (COMMON_BUILD_NOTIFICATION)
#    SET(MICROSERVICE_SRC "${MICROSERVICE_SRC}" NotificationService.cpp)
endif ()

foreach (item ${MICROSERVICE_SRC})
    add_executable(${item} ${item})
    target_link_libraries(${item} ${COMMON_LIB})
    add_test(NAME ${item} COMMAND ${item})
endforeach ()
