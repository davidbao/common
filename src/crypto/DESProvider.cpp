//
//  DESProvider.cpp
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "crypto/DESProvider.h"
#include <openssl/des.h>

namespace Crypto {
    DESProvider::DESProvider() : SymmetricAlgorithm() {
        initKeySizes();

        generateKey();
        generateIV();
    }

    DESProvider::DESProvider(const ByteArray &key, const ByteArray &iv) : SymmetricAlgorithm() {
        initKeySizes();

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

    DESProvider::DESProvider(const String &key, const ByteArray &iv) : SymmetricAlgorithm() {
        initKeySizes();

        DES_cblock cblock;
        DES_string_to_key(key, &cblock);
        _key = ByteArray(cblock, sizeof(cblock));

        if (!validIVSize((int) iv.count() * 8)) {
            generateIV();
        } else {
            _iv = iv;
        }
    }

    int DESProvider::keySize() const {
        return 64;
    }

    int DESProvider::blockSize() const {
        return 64;
    }

    const EVP_CIPHER *DESProvider::cipher(CypherMode mode) const {
        static const EVP_CIPHER *ciphers[] = {EVP_des_cbc(), EVP_des_ecb(), EVP_des_ofb(), EVP_des_cfb(), nullptr};
        return ciphers[mode];
    }

    void DESProvider::initKeySizes() {
        static KeySizes s_legalBlockSizes{KeySize(64, 64, 0)};
        _legalBlockSizes = s_legalBlockSizes;
        static KeySizes s_legalKeySizes{KeySize(64, 64, 0)};
        _legalKeySizes = s_legalKeySizes;
    }

    TripleDESProvider::TripleDESProvider() : SymmetricAlgorithm() {
        initKeySizes();

        generateKey();
        generateIV();
    }

    TripleDESProvider::TripleDESProvider(const ByteArray &key, const ByteArray &iv) : SymmetricAlgorithm() {
        initKeySizes();

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

    TripleDESProvider::TripleDESProvider(const String &key, const ByteArray &iv) : SymmetricAlgorithm() {
        initKeySizes();

        DES_cblock cblock1, cblock2;
        DES_string_to_2keys(key, &cblock1, &cblock2);
        _key.addRange(cblock1, sizeof(cblock1));
        _key.addRange(cblock2, sizeof(cblock2));

        if (!validIVSize((int) iv.count() * 8)) {
            generateIV();
        } else {
            _iv = iv;
        }
    }

    int TripleDESProvider::keySize() const {
        return 3 * 64;
    }

    int TripleDESProvider::blockSize() const {
        return 64;
    }

    const EVP_CIPHER *TripleDESProvider::cipher(CypherMode mode) const {
        static const EVP_CIPHER *ciphers[] = {EVP_des_ede3_cbc(), EVP_des_ede3_ecb(),
                                              EVP_des_ede3_ofb(), EVP_des_ede3_cfb(), nullptr};
        return ciphers[mode];
    }

    void TripleDESProvider::initKeySizes() {
        static KeySizes s_legalBlockSizes{KeySize(64, 64, 0)};
        _legalBlockSizes = s_legalBlockSizes;
        static KeySizes s_legalKeySizes{KeySize(2 * 64, 3 * 64, 64)};
        _legalKeySizes = s_legalKeySizes;
    }
}
