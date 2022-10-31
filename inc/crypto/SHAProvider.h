//
//  SHAProvider.h
//  common
//
//  Created by baowei on 2021/5/21.
//  Copyright © 2021 com. All rights reserved.
//

#ifndef SHAProvider_h
#define SHAProvider_h

#include "data/ByteArray.h"

namespace Common
{
    class SHAProvider
    {
    public:
        enum KeySize
        {
            Key1 = 1,
            Key224 = 2,
            Key256 = 3,
            Key384 = 4,
            Key512 = 5,
            Default = Key1
        };

        static bool computeHash(const String& src, ByteArray& output, KeySize size = KeySize::Default);
        static bool computeHash(const ByteArray& src, ByteArray& output, KeySize size = KeySize::Default);
        static bool computeHash(const String& src, String& output, KeySize size = KeySize::Default);
        
    private:
        static bool computeHash(const uint8_t* src, uint length, ByteArray& output, KeySize size = KeySize::Default);
    };
}

#endif /* SHAProvider_h */
