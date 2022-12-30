//
//  EthernetContext.h
//  common
//
//  Created by baowei on 15/7/27.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef EthernetContext_h
#define EthernetContext_h

#include "data/ValueType.h"
#include "net/NetType.h"

using namespace Data;
using namespace Net;

namespace Drivers
{
    class EthernetContext
    {
    public:
        EthernetContext(const Endpoint& endpoint)
        {
            _endpoint = endpoint;
        }
        inline const String& address() const
        {
            return _endpoint.address;
        }
        inline void setAddress(const String& address)
        {
            _endpoint.address = address;
        }
        
        inline int port() const
        {
            return _endpoint.port;
        }
        inline void setPort(int port)
        {
            _endpoint.port = port;
        }
        
        inline const Endpoint& endpoint() const
        {
            return _endpoint;
        }
        inline void setPort(Endpoint endpoint)
        {
            _endpoint = endpoint;
        }
        
    protected:
        Endpoint _endpoint;
    };
}

#endif  // EthernetContext_h
