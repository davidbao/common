//
//  RSAProvider.h
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef RSAProvider_h
#define RSAProvider_h

#include "data/ByteArray.h"
#include "data/String.h"

using namespace Data;

namespace Crypto {
    class RSAProvider {
    public:
        enum KeySize {
            Key512 = 1,
            Key256 = 2,
            Key1024 = 3,
            Key2048 = 4,
            Default = Key512
        };

        static bool encrypt(const ByteArray &data, ByteArray &out, KeySize size = KeySize::Default);

        static bool decrypt(const ByteArray &data, ByteArray &out, KeySize size = KeySize::Default);

        static bool encrypt(const String &data, String &out, KeySize size = KeySize::Default);

        static bool decrypt(const String &data, String &out, KeySize size = KeySize::Default);

        static bool encryptToBase64(const String &data, String &out, KeySize size = KeySize::Default);

        static bool decryptFromBase64(const String &data, String &out, KeySize size = KeySize::Default);

        static bool encryptByPubKeyFile(const String &pemFileName, const String &data, String &out);

        static bool decryptByPriKeyFile(const String &pemFileName, const String &data, String &out);

    private:
        static const char *defaultPubKey(KeySize size);

        static const char *defaultPriKey(KeySize size);

    private:
        static const char *DefaultPriKey;
        static const char *DefaultPubKey;

        static const char *Default256PriKey;
        static const char *Default256PubKey;

        static const char *Default1024PriKey;
        static const char *Default1024PubKey;

        static const char *Default2048PriKey;
        static const char *Default2048PubKey;
    };
}

#endif // RSAProvider_h
