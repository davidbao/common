//
//  SHAProvider.cpp
//  common
//
//  Created by baowei on 2021/5/21.
//  Copyright (c) 2021 com. All rights reserved.
//

#include "crypto/SHAProvider.h"
#include <openssl/sha.h>

namespace Crypto {
    SHAProvider::SHAProvider(KeySize keySize) : _keySize(keySize) {
    }

    int SHAProvider::hashSize() const {
        static const int bytes[] = {SHA_DIGEST_LENGTH,
                                  SHA224_DIGEST_LENGTH,
                                  SHA256_DIGEST_LENGTH,
                                  SHA384_DIGEST_LENGTH,
                                  SHA512_DIGEST_LENGTH};
        return bytes[_keySize] * 8;
    }

    const EVP_MD *SHAProvider::type() const {
        static const EVP_MD *mds[] = {EVP_sha1(),
                                      EVP_sha224(),
                                      EVP_sha256(),
                                      EVP_sha384(),
                                      EVP_sha512()};
        return mds[_keySize];
    }
}
