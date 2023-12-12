//
//  SmProvider.cpp
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "crypto/SmProvider.h"
#include "system/Math.h"

using namespace System;

namespace Crypto {
    // https://github.com/GuoHuiChen/gmjs
    // js sm2 match.
    Sm2Provider::Sm2Provider() : _publicKey{0}, _privateKey{0} {
        generateKey(_publicKey, _privateKey);
    }

    Sm2Provider::Sm2Provider(const uint8_t publicKey[PublicKeyLength]) : _publicKey{0}, _privateKey{0} {
        memcpy(_publicKey, publicKey, PublicKeyLength);
        memset(_privateKey, 0, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const ByteArray &publicKey) : _publicKey{0}, _privateKey{0} {
        memcpy(_publicKey, publicKey.data(), Math::min(PublicKeyLength, (int) publicKey.count()));
        memset(_privateKey, 0, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const uint8_t publicKey[PublicKeyLength],
                             const uint8_t privateKey[PrivateKeyLength]) : _publicKey{0}, _privateKey{0} {
        memcpy(_publicKey, publicKey, PublicKeyLength);
        memcpy(_privateKey, privateKey, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const ByteArray &publicKey, const ByteArray &privateKey) : _publicKey{0}, _privateKey{0} {
        memcpy(_publicKey, publicKey.data(), Math::min(PublicKeyLength, (int) publicKey.count()));
        memcpy(_privateKey, privateKey.data(), Math::min(PrivateKeyLength, (int) privateKey.count()));
    }

    Sm2Provider::Sm2Provider(const Sm2Provider &provider) : _publicKey{0}, _privateKey{0} {
        memcpy(_publicKey, provider._publicKey, PublicKeyLength);
        memcpy(_privateKey, provider._privateKey, PrivateKeyLength);
    }

    bool Sm2Provider::createPubEVP_PKEY(const uint8_t publicKey[PublicKeyLength], EVP_PKEY *&key) {
        return createEVP_PKEY(publicKey, nullptr, key);
    }

    bool Sm2Provider::createPriEVP_PKEY(const uint8_t privateKey[PrivateKeyLength], EVP_PKEY *&key) {
        return createEVP_PKEY(nullptr, privateKey, key);
    }

    bool Sm2Provider::createEVP_PKEY(EVP_PKEY *&key) {
        return createEVP_PKEY(nullptr, nullptr, key);
    }

    bool
    Sm2Provider::createEVP_PKEY(const uint8_t publicKey[PublicKeyLength], const uint8_t privateKey[PrivateKeyLength],
                                EVP_PKEY *&key) {
        bool keygen = publicKey == nullptr && privateKey == nullptr;
        EC_KEY *key_pair = EC_KEY_new_by_curve_name(NID_sm2);
        if (key_pair == nullptr) {
            return false;
        }
        if (publicKey != nullptr) {
            BIGNUM *qx = BN_bin2bn(publicKey, CoordinateLength, nullptr);
            BIGNUM *qy = BN_bin2bn(publicKey + CoordinateLength, CoordinateLength, nullptr);
            if (!EC_KEY_set_public_key_affine_coordinates(key_pair, qx, qy)) {
                EC_KEY_free(key_pair);
                BN_free(qx);
                BN_free(qy);
                return false;
            }
            BN_free(qx);
            BN_free(qy);
        }
        if (privateKey != nullptr) {
            BIGNUM *d = BN_bin2bn(privateKey, PrivateKeyLength, nullptr);
            if (!EC_KEY_set_private_key(key_pair, d)) {
                EC_KEY_free(key_pair);
                BN_free(d);
                return false;
            }
            BN_free(d);
        }
        EVP_PKEY_CTX *ctx;
        if (!(ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr))) {
            EC_KEY_free(key_pair);
            return false;
        }
        if (!EVP_PKEY_paramgen_init(ctx)) {
            EC_KEY_free(key_pair);
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        if (!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_sm2)) {
            EC_KEY_free(key_pair);
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        if (!keygen) {
            if (!EVP_PKEY_paramgen(ctx, &key)) {
                EC_KEY_free(key_pair);
                EVP_PKEY_CTX_free(ctx);
                return false;
            }
            if (!EVP_PKEY_set1_EC_KEY(key, key_pair)) {
                EC_KEY_free(key_pair);
                EVP_PKEY_CTX_free(ctx);
                return false;
            }
        } else {
            if (!EVP_PKEY_keygen_init(ctx)) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(key);
                return false;
            }
            if (!EVP_PKEY_keygen(ctx, &key)) {
                EVP_PKEY_CTX_free(ctx);
                EVP_PKEY_free(key);
                return false;
            }
        }
        EVP_PKEY_CTX_free(ctx);
        EC_KEY_free(key_pair);
        return true;
    }

    bool Sm2Provider::encrypt(const ByteArray &data, ByteArray &output) {
        EVP_PKEY *key = nullptr;
        if (!createPubEVP_PKEY(_publicKey, key)) {
            return false;
        }
        if (!EVP_PKEY_set_alias_type(key, EVP_PKEY_SM2)) {
            EVP_PKEY_free(key);
            return false;
        }
        EVP_PKEY_CTX *ctx;
        if (!(ctx = EVP_PKEY_CTX_new(key, nullptr))) {
            EVP_PKEY_free(key);
            return false;
        }
        EVP_PKEY_free(key);
        if (!EVP_PKEY_encrypt_init(ctx)) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        size_t ciphertext_len;
        if (!EVP_PKEY_encrypt(ctx, nullptr, &ciphertext_len, data.data(), data.count())) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        uint8_t *ciphertext;
        if (!(ciphertext = (uint8_t *) malloc(ciphertext_len))) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        if (!EVP_PKEY_encrypt(ctx, ciphertext, &ciphertext_len, data.data(), data.count())) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        output = ByteArray(ciphertext, ciphertext_len);
        free(ciphertext);
        EVP_PKEY_CTX_free(ctx);

        return true;
    }

    bool Sm2Provider::decrypt(const ByteArray &data, ByteArray &output) {
        EVP_PKEY *key = nullptr;
        if (!createPriEVP_PKEY(_privateKey, key)) {
            return false;
        }
        if (!EVP_PKEY_set_alias_type(key, EVP_PKEY_SM2)) {
            EVP_PKEY_free(key);
            return false;
        }
        EVP_PKEY_CTX *ctx;
        if (!(ctx = EVP_PKEY_CTX_new(key, nullptr))) {
            EVP_PKEY_free(key);
            return false;
        }
        EVP_PKEY_free(key);
        if (!EVP_PKEY_decrypt_init(ctx)) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        size_t plaintext_len;
        if (!EVP_PKEY_decrypt(ctx, nullptr, &plaintext_len, data.data(), data.count())) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        uint8_t *plaintext;
        if (!(plaintext = (uint8_t *) malloc(plaintext_len))) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }
        if (!EVP_PKEY_decrypt(ctx, plaintext, &plaintext_len, data.data(), data.count())) {
            EVP_PKEY_CTX_free(ctx);
            free(plaintext);
            return false;
        }
        output = ByteArray(plaintext, plaintext_len);
        free(plaintext);
        EVP_PKEY_CTX_free(ctx);

        return true;
    }

    bool Sm2Provider::encrypt(const String &data, String &output) {
        ByteArray in((uint8_t *) data.c_str(), data.length());
        ByteArray out;
        if (encrypt(in, out)) {
            output = out.toHexString();
            return true;
        }
        return false;
    }

    bool Sm2Provider::decrypt(const String &data, String &output) {
        ByteArray in;
        if (ByteArray::parseHexString(data, in)) {
            ByteArray out;
            if (decrypt(in, out)) {
                output = String((const char *) out.data(), out.count());
                return true;
            }
        }
        return false;
    }

    bool Sm2Provider::encryptToBase64(const String &data, String &output) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encrypt(array, buffer)) {
            output = String::toBase64(buffer.data(), 0, buffer.count());
            return true;
        }
        return false;
    }

    bool Sm2Provider::decryptFromBase64(const String &data, String &output) {
        ByteArray in;
        if (String::fromBase64(data, in)) {
            ByteArray out;
            if (decrypt(in, out)) {
                output = String((const char *) out.data(), out.count());
                return true;
            }
        }
        return false;
    }

    bool Sm2Provider::generateKey(uint8_t publicKey[PublicKeyLength], uint8_t privateKey[PrivateKeyLength]) {
        EVP_PKEY *key = nullptr;
        if (!createEVP_PKEY(key)) {
            return false;
        }
        EC_KEY *key_pair;
        if (!(key_pair = EVP_PKEY_get0_EC_KEY(key))) {
            EVP_PKEY_free(key);
            return false;
        }

        if (!getPrivateKey(key_pair, privateKey)) {
            EVP_PKEY_free(key);
            return false;
        }

        if (!getPublicKey(key_pair, publicKey)) {
            EVP_PKEY_free(key);
            return false;
        }

        EVP_PKEY_free(key);
        return true;
    }

    bool Sm2Provider::generateKey(ByteArray &publicKey, ByteArray &privateKey) {
        uint8_t pubKey[PublicKeyLength];
        uint8_t priKey[PrivateKeyLength];
        if (generateKey(pubKey, priKey)) {
            publicKey = ByteArray(pubKey, PublicKeyLength);
            privateKey = ByteArray(priKey, PrivateKeyLength);
            return true;
        }
        return false;
    }

    bool
    Sm2Provider::generatePublicKey(const uint8_t privateKey[PrivateKeyLength], uint8_t publicKey[PublicKeyLength]) {
        EVP_PKEY *key = nullptr;
        if (!createPriEVP_PKEY(privateKey, key)) {
            return false;
        }

        EC_KEY *key_pair;
        if (!(key_pair = EVP_PKEY_get0_EC_KEY(key))) {
            EVP_PKEY_free(key);
            return false;
        }
        const EC_GROUP *group;
        if (!(group = EC_KEY_get0_group(key_pair))) {
            EVP_PKEY_free(key);
            return false;
        }
        BN_CTX *ctx;
        if (!(ctx = BN_CTX_new())) {
            EVP_PKEY_free(key);
            return false;
        }
        BN_CTX_start(ctx);

        EC_POINT *pub_key = EC_POINT_new(group);
        BIGNUM *pri_key = BN_bin2bn(privateKey, PrivateKeyLength, nullptr);
        if (!EC_POINT_mul(group, pub_key, pri_key, nullptr, nullptr, ctx)) {
            BN_free(pri_key);
            EC_POINT_free(pub_key);
            return false;
        }
        BN_free(pri_key);

        BIGNUM *x_coordinate, *y_coordinate;
        x_coordinate = BN_CTX_get(ctx);
        y_coordinate = BN_CTX_get(ctx);
        if (!(EC_POINT_get_affine_coordinates(group, pub_key, x_coordinate, y_coordinate, ctx))) {
            BN_free(pri_key);
            EC_POINT_free(pub_key);
            return false;
        }
        uint8_t x[CoordinateLength], y[CoordinateLength];
        if (!(BN_num_bytes(x_coordinate) == CoordinateLength && BN_bn2bin(x_coordinate, x) == CoordinateLength &&
              BN_num_bytes(y_coordinate) == CoordinateLength && BN_bn2bin(y_coordinate, y) == CoordinateLength)) {
            EVP_PKEY_free(key);
            BN_CTX_end(ctx);
            BN_CTX_free(ctx);
            return false;
        }
        memcpy(publicKey, x, CoordinateLength);
        memcpy(publicKey + CoordinateLength, y, CoordinateLength);

        EC_POINT_free(pub_key);
        EVP_PKEY_free(key);
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);

        return true;
    }

    bool Sm2Provider::generatePublicKey(const ByteArray &privateKey, ByteArray &publicKey) {
        if (privateKey.count() != PrivateKeyLength) {
            return false;
        }
        uint8_t pubKey[PublicKeyLength];
        if (generatePublicKey(privateKey.data(), pubKey)) {
            publicKey = ByteArray(pubKey, PublicKeyLength);
            return true;
        }
        return false;
    }

    Sm2Provider Sm2Provider::fromPrivateKey(const ByteArray &privateKey) {
        ByteArray publicKey;
        Sm2Provider::generatePublicKey(privateKey, publicKey);
        return {publicKey, privateKey};
    }

    bool Sm2Provider::getPrivateKey(EC_KEY *key, uint8_t privateKey[PrivateKeyLength]) {
        const BIGNUM *priKey;
        if (!(priKey = EC_KEY_get0_private_key(key))) {
            return false;
        }
        if (!(BN_num_bytes(priKey) == PrivateKeyLength && BN_bn2bin(priKey, privateKey) == PrivateKeyLength)) {
            return false;
        }
        return true;
    }

    bool Sm2Provider::getPublicKey(EC_KEY *key, uint8_t publicKey[PublicKeyLength]) {
        const EC_POINT *pub_key;
        if (!(pub_key = EC_KEY_get0_public_key(key))) {
            return false;
        }
        const EC_GROUP *group;
        BN_CTX *ctx;
        BIGNUM *x_coordinate, *y_coordinate;
        if (!(group = EC_KEY_get0_group(key))) {
            return false;
        }
        if (!(ctx = BN_CTX_new())) {
            return false;
        }
        BN_CTX_start(ctx);
        x_coordinate = BN_CTX_get(ctx);
        y_coordinate = BN_CTX_get(ctx);
        if (!(EC_POINT_get_affine_coordinates(group, pub_key, x_coordinate, y_coordinate, ctx))) {
            BN_CTX_end(ctx);
            BN_CTX_free(ctx);
            return false;
        }
        uint8_t x[CoordinateLength], y[CoordinateLength];
        if (!(BN_num_bytes(x_coordinate) == CoordinateLength && BN_bn2bin(x_coordinate, x) == CoordinateLength &&
              BN_num_bytes(y_coordinate) == CoordinateLength && BN_bn2bin(y_coordinate, y) == CoordinateLength)) {
            BN_CTX_end(ctx);
            BN_CTX_free(ctx);
            return false;
        }
        memcpy(publicKey, x, CoordinateLength);
        memcpy(publicKey + CoordinateLength, y, CoordinateLength);

        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
        return true;
    }

    Sm3Provider::Sm3Provider() = default;

    int Sm3Provider::hashSize() const {
        return 32 * 8;
    }

    const EVP_MD *Sm3Provider::type() const {
        return EVP_sm3();
    }

    Sm4Provider::Sm4Provider() : SymmetricAlgorithm() {
        initKeySizes();

        generateKey();
        generateIV();
    }

    Sm4Provider::Sm4Provider(const ByteArray &key, const ByteArray &iv) : SymmetricAlgorithm() {
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

    Sm4Provider::Sm4Provider(const String &key, const ByteArray &iv) : SymmetricAlgorithm() {
        initKeySizes();

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

    int Sm4Provider::keySize() const {
        return 16 * 8;
    }

    int Sm4Provider::blockSize() const {
        return 16 * 8;
    }

    const EVP_CIPHER *Sm4Provider::cipher(CypherMode mode) const {
        static const EVP_CIPHER *ciphers[] = {EVP_sm4_cbc(), EVP_sm4_ecb(), EVP_sm4_ofb(), EVP_sm4_cfb(), EVP_sm4_ctr()};
        return ciphers[mode];
    }

    void Sm4Provider::initKeySizes() {
        static Vector<KeySizes> s_legalBlockSizes{KeySizes(128, 128, 0)};
        _legalBlockSizes = s_legalBlockSizes;
        static Vector<KeySizes> s_legalKeySizes{KeySizes(128, 128, 0)};
        _legalKeySizes = s_legalKeySizes;
    }
}
