//
//  SHAProvider.h
//  common
//
//  Created by baowei on 2021/5/21.
//  Copyright (c) 2021 com. All rights reserved.
//

#ifndef SHAProvider_h
#define SHAProvider_h

#include "data/ByteArray.h"
#include "data/String.h"

using namespace Data;

namespace Crypto {
    class SHAProvider {
    public:
        enum KeySize {
            Key1 = 1,
            Key224 = 2,
            Key256 = 3,
            Key384 = 4,
            Key512 = 5,
            Default = Key1
        };

        static bool computeHash(const String &src, ByteArray &output, KeySize size = KeySize::Default);

        static bool computeHash(const ByteArray &src, ByteArray &output, KeySize size = KeySize::Default);

        static bool computeHash(const String &src, String &output, KeySize size = KeySize::Default);

    private:
        static bool
        computeHash(const uint8_t *src, size_t length, ByteArray &output, KeySize size = KeySize::Default);
    };
}

#endif /* SHAProvider_h */
