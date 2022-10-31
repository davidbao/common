//
//  SmProvider.h
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef SmProvider_h
#define SmProvider_h

#include "data/ByteArray.h"
#include <openssl/evp.h>

namespace Common {
    class Sm2Provider {
    public:
        static const int PublicKeyLength = 64;
        static const int PrivateKeyLength = 32;
        static const int CoordinateLength = PublicKeyLength / 2;

    public:
        Sm2Provider();

        Sm2Provider(const uint8_t publicKey[PublicKeyLength]);

        Sm2Provider(const ByteArray &publicKey);

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

    class Sm3Provider {
    public:
        static const int Length = 32;

        Sm3Provider();

        static bool computeHash(const String &data, ByteArray &output);

        static bool computeHash(const ByteArray &data, ByteArray &output);

        static bool computeHash(const String &data, String &output);

        static bool computeFileHash(const String &path, uint8_t output[Length]);

        static bool computeFileHash(const String &path, ByteArray &output);

        static bool computeFileHash(const String &path, String &output);
    };

    class Sm4Provider {
    public:
        enum CypherMode {
            Ecb = 0,
            Cbc = 1,
            Ofb = 2,
            Ctr = 3
        };
        static const int Length = 16;

        Sm4Provider(const uint8_t key[Length], CypherMode mode = CypherMode::Ecb);

        Sm4Provider(const uint8_t key[Length], const uint8_t iv[Length], CypherMode mode = CypherMode::Ecb);

        Sm4Provider(const ByteArray &key, CypherMode mode = CypherMode::Ecb);

        Sm4Provider(const ByteArray &key, const ByteArray &iv, CypherMode mode = CypherMode::Ecb);

        Sm4Provider(const String &key, CypherMode mode = CypherMode::Ecb);

        Sm4Provider(const String &key, const String &iv, CypherMode mode = CypherMode::Ecb);

        bool encrypt(const ByteArray &data, ByteArray &output);

        bool decrypt(const ByteArray &data, ByteArray &output);

        bool encrypt(const String &data, String &output);

        bool decrypt(const String &data, String &output);

        bool encryptToBase64(const String &data, String &output);

        bool decryptFromBase64(const String &data, String &output);

    public:
        static bool
        encrypt(const uint8_t key[Length], const ByteArray &data, ByteArray &output, CypherMode mode = CypherMode::Ecb);

        static bool
        encrypt(const uint8_t key[Length], const uint8_t iv[Length], const ByteArray &data, ByteArray &output,
                CypherMode mode = CypherMode::Ecb);

        static bool
        decrypt(const uint8_t key[Length], const ByteArray &data, ByteArray &output, CypherMode mode = CypherMode::Ecb);

        static bool
        decrypt(const uint8_t key[Length], const uint8_t iv[Length], const ByteArray &data, ByteArray &output,
                CypherMode mode = CypherMode::Ecb);

        static bool
        encrypt(const uint8_t key[Length], const String &data, String &output, CypherMode mode = CypherMode::Ecb);

        static bool encrypt(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                            CypherMode mode = CypherMode::Ecb);

        static bool
        decrypt(const uint8_t key[Length], const String &data, String &output, CypherMode mode = CypherMode::Ecb);

        static bool decrypt(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                            CypherMode mode = CypherMode::Ecb);

        static bool encryptToBase64(const uint8_t key[Length], const String &data, String &output,
                                    CypherMode mode = CypherMode::Ecb);

        static bool
        encryptToBase64(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                        CypherMode mode = CypherMode::Ecb);

        static bool decryptFromBase64(const uint8_t key[Length], const String &data, String &output,
                                      CypherMode mode = CypherMode::Ecb);

        static bool
        decryptFromBase64(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                          CypherMode mode = CypherMode::Ecb);

        static void generateKey(uint8_t key[Length]);

        static void generateKey(ByteArray &key);

    private:
        uint8_t _key[Length];
        uint8_t _iv[Length];
        CypherMode _mode;
    };
}

#endif /* SmProvider_h */
