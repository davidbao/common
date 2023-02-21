//
//  Md5Provider.h
//  common
//
//  Created by baowei on 2015/11/12.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef MD5Provider_h
#define MD5Provider_h

#include "data/ByteArray.h"
#include "data/String.h"

using namespace Data;

namespace Crypto {
    class Md5Provider {
    public:
        static const int Length = 16;

        static bool computeHash(const String &data, ByteArray &output);

        static bool computeHash(const ByteArray &data, ByteArray &output);

        static bool computeHash(const String &data, String &output);

        static bool computeFileHash(const String &path, uint8_t output[Length]);

        static bool computeFileHash(const String &path, ByteArray &output);

        static bool computeFileHash(const String &path, String &output);
    };
}

#endif // MD5Provider_h
