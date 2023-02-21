//
//  Resources.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Resources_h
#define Resources_h

#include "data/String.h"

using namespace Data;

namespace System {
    class Resources {
    public:
        static String getString(const String &name);

    private:
        static String getStaticString(const String &name);
    };
}

#endif // Resources_h
