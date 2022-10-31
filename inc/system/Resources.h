//
//  Resources.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef Resources_h
#define Resources_h

#include "data/ValueType.h"

namespace Common
{
	class Resources
	{
    public:
        static String getString(const String& name);

    private:
        static String getStaticString(const String& name);
	};
}

#endif // Resources_h
