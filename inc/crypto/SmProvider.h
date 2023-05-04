//
//  SmProvider.h
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef SmProvider_h
#define SmProvider_h

#include "crypto/Algorithm.h"

using namespace Data;

namespace Crypto {
    class Sm2Provider {
    public:
        static const int PublicKeyLength = 64;
        static const int PrivateKeyLength = 32;
        static const int CoordinateLength = PublicKeyLength / 2;

    public:
        Sm2Provider();

        explicit Sm2Provider(const uint8_t publicKey[PublicKeyLength]);

        explicit Sm2Provider(const ByteArray &publicKey);

        Sm2Provider(const uint8_t publicKey[PublicKeyLength], const uint8_t privateKey[PrivateKeyLength]);

        Sm2Provider(const ByteArray &publicKey, const ByteArray &privateKey);

        Sm2Provider(const Sm2Provider &provider);

        bool encrypt(const ByteArray &data, ByteArray &output);

        bool decrypt(const ByteArray &data, ByteArray &output);

        bool encrypt(const String &data, String &output);

        bool decrypt(const String &data, String &output);

        bool encryptToBase64(const String &data, String &output);

        bool decryptFromBase64(const String &data, String &output);

    public:
        static bool generateKey(uint8_t publicKey[PublicKeyLength], uint8_t privateKey[PrivateKeyLength]);

        static bool generateKey(ByteArray &publicKey, ByteArray &privateKey);

        static bool generatePublicKey(const uint8_t privateKey[PrivateKeyLength], uint8_t publicKey[PublicKeyLength]);

        static bool generatePublicKey(const ByteArray &privateKey, ByteArray &publicKey);

        static Sm2Provider fromPrivateKey(const ByteArray &privateKey);

    private:
        static bool createPubEVP_PKEY(const uint8_t publicKey[PublicKeyLength], EVP_PKEY *&key);

        static bool createPriEVP_PKEY(const uint8_t privateKey[PrivateKeyLength], EVP_PKEY *&key);

        static bool createEVP_PKEY(EVP_PKEY *&key);

        static bool createEVP_PKEY(const uint8_t publicKey[PublicKeyLength], const uint8_t privateKey[PrivateKeyLength],
                                   EVP_PKEY *&key);

        static bool getPrivateKey(EC_KEY *key, uint8_t privateKey[PrivateKeyLength]);

        static bool getPublicKey(EC_KEY *key, uint8_t publicKey[PublicKeyLength]);

    private:
        uint8_t _privateKey[PrivateKeyLength];
        uint8_t _publicKey[PublicKeyLength];
    };

    class Sm3Provider : public HashAlgorithm {
    public:
        Sm3Provider();

        int hashSize() const override;

    protected:
        const EVP_MD *type() const override;
    };

    class Sm4Provider : public SymmetricAlgorithm {
    public:
        Sm4Provider();

        explicit Sm4Provider(const ByteArray &key, const ByteArray &iv = ByteArray::Empty);

        explicit Sm4Provider(const String &key, const ByteArray &iv = ByteArray::Empty);

        int keySize() const override;

        int blockSize() const override;

    protected:
        const EVP_CIPHER *cipher(CypherMode mode) const override;

    private:
        void initKeySizes();
    };
}

#endif /* SmProvider_h */
