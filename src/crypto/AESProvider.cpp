//
//  AESProvider.cpp
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "crypto/AESProvider.h"
//#include <openssl/aes.h>

namespace Crypto {
    AESProvider::AESProvider(KeySize keySize) : SymmetricAlgorithm(), _keySize(256) {
        initKeySizes(keySize);

        generateKey();
        generateIV();
    }

    AESProvider::AESProvider(KeySize keySize, const ByteArray &key, const ByteArray &iv)
            : SymmetricAlgorithm(), _keySize(256) {
        initKeySizes(keySize);

        if (!validKeySize((int) key.count() * 8)) {
            generateKey();
        } else {
            _key = key;
        }
        if (!validIVSize((int) iv.count() * 8)) {
            generateIV();
        } else {
            _iv = iv;
        }
    }

    AESProvider::AESProvider(KeySize keySize, const String &key, const ByteArray &iv)
            : SymmetricAlgorithm(), _keySize(256) {
        initKeySizes(keySize);

        if (key.isNullOrEmpty()) {
            generateKey();
        } else {
            _key = ByteArray((const uint8_t *) key.c_str(), key.length());
        }

        if (!validIVSize((int) iv.count() * 8)) {
            generateIV();
        } else {
            _iv = iv;
        }
    }

    int AESProvider::keySize() const {
        return _keySize;
    }

    int AESProvider::blockSize() const {
        return 128;
    }

    int AESProvider::feedbackSize() const {
        return 8;
    }

    const EVP_CIPHER *AESProvider::cipher(CypherMode mode) const {
        static const EVP_CIPHER *ciphers128[] = {EVP_aes_128_cbc(), EVP_aes_128_ecb(),
                                                 EVP_aes_128_ofb(), EVP_aes_128_cfb(), EVP_aes_128_ctr()};
        static const EVP_CIPHER *ciphers192[] = {EVP_aes_192_cbc(), EVP_aes_192_ecb(),
                                                 EVP_aes_192_ofb(), EVP_aes_192_cfb(), EVP_aes_192_ctr()};
        static const EVP_CIPHER *ciphers256[] = {EVP_aes_256_cbc(), EVP_aes_256_ecb(),
                                                 EVP_aes_256_ofb(), EVP_aes_256_cfb(), EVP_aes_256_ctr()};
        switch (_keySize) {
            case 128:
                return ciphers128[mode];
            case 192:
                return ciphers192[mode];
            case 256:
            default:
                return ciphers256[mode];
        }
    }

    void AESProvider::initKeySizes(KeySize keySize) {
        switch (keySize) {
            case Aes128:
                _keySize = 128;
                break;
            case Aes192:
                _keySize = 192;
                break;
            case Aes256:
            default:
                _keySize = 256;
                break;
        }

        static Vector<KeySizes> s_legalBlockSizes{KeySizes(128, 128, 0)};
        _legalBlockSizes = s_legalBlockSizes;
        static Vector<KeySizes> s_legalKeySizes{KeySizes(128, 256, 64)};
        _legalKeySizes = s_legalKeySizes;
    }
}
