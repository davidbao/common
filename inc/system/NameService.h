//
//  NameService.h
//  common
//
//  Created by baowei on 2018/9/21.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef NameService_h
#define NameService_h

#include "configuration/Configuration.h"

namespace Common
{
    class INameContainer
    {
    public:
        INameContainer();
        virtual ~INameContainer();
        
        virtual bool containName(const String& name) const = 0;
    };
    class NameService
    {
    public:
        static String createNameByType(const INameContainer* container, const String& typeName);
        static String createNameByName(const INameContainer* container, const String& name);
        
        static bool validate(const INameContainer* container, const String& name);
        static bool validate(const String& name);
        
    private:
        static String createNameInner(const String& name, int index = 1);
    };
}

#endif /* NameService_h */
