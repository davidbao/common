//
//  SmProvider.cpp
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "crypto/SmProvider.h"
#include "system/Math.h"
#include "system/Random.h"
#include <openssl/ec.h>

using namespace System;

namespace Crypto {
    // https://github.com/GuoHuiChen/gmjs
    // js sm2 match.
    Sm2Provider::Sm2Provider() {
        generateKey(_publicKey, _privateKey);
    }

    Sm2Provider::Sm2Provider(const uint8_t publicKey[PublicKeyLength]) {
        memcpy(_publicKey, publicKey, PublicKeyLength);
        memset(_privateKey, 0, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const ByteArray &publicKey) {
        memcpy(_publicKey, publicKey.data(), Math::min(PublicKeyLength, (int) publicKey.count()));
        memset(_privateKey, 0, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const uint8_t publicKey[PublicKeyLength], const uint8_t privateKey[PrivateKeyLength]) {
        memcpy(_publicKey, publicKey, PublicKeyLength);
        memcpy(_privateKey, privateKey, PrivateKeyLength);
    }

    Sm2Provider::Sm2Provider(const ByteArray &publicKey, const ByteArray &privateKey) {
        memcpy(_publicKey, publicKey.data(), Math::min(PublicKeyLength, (int) publicKey.count()));
        memcpy(_privateKey, privateKey.data(), Math::min(PrivateKeyLength, (int) privateKey.count()));
    }

    Sm2Provider::Sm2Provider(const Sm2Provider &provider) {
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
        EVP_PKEY_CTX *ctx = nullptr;
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
        EC_KEY *key_pair = nullptr;
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

        EC_KEY *key_pair = nullptr;
        if (!(key_pair = EVP_PKEY_get0_EC_KEY(key))) {
            EVP_PKEY_free(key);
            return false;
        }
        const EC_GROUP *group = nullptr;
        if (!(group = EC_KEY_get0_group(key_pair))) {
            EVP_PKEY_free(key);
            return false;
        }
        BN_CTX *ctx = nullptr;
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

        BIGNUM *x_coordinate = nullptr, *y_coordinate = nullptr;
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
        const BIGNUM *priKey = nullptr;
        if (!(priKey = EC_KEY_get0_private_key(key))) {
            return false;
        }
        if (!(BN_num_bytes(priKey) == PrivateKeyLength && BN_bn2bin(priKey, privateKey) == PrivateKeyLength)) {
            return false;
        }
        return true;
    }

    bool Sm2Provider::getPublicKey(EC_KEY *key, uint8_t publicKey[PublicKeyLength]) {
        const EC_POINT *pub_key = nullptr;
        if (!(pub_key = EC_KEY_get0_public_key(key))) {
            return false;
        }
        const EC_GROUP *group = nullptr;
        BN_CTX *ctx = nullptr;
        BIGNUM *x_coordinate = nullptr, *y_coordinate = nullptr;
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

    bool Sm3Provider::computeHash(const String &data, ByteArray &output) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        return computeHash(array, output);
    }

    bool Sm3Provider::computeHash(const ByteArray &data, ByteArray &output) {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        uint8_t out[64] = {0};
        unsigned int out_size = 0;
        int result = EVP_Digest(data.data(), data.count(), out, &out_size, EVP_sm3(), nullptr);
        EVP_MD_CTX_free(ctx);
        if (result) {
            output = ByteArray(out, out_size);
            return true;
        }
        return false;
    }

    bool Sm3Provider::computeHash(const String &data, String &output) {
        ByteArray array;
        if (computeHash(data, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool Sm3Provider::computeFileHash(const String &path, uint8_t output[Length]) {
        FILE *f;
        size_t n;
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        unsigned char buf[1024];

        if ((f = fopen(path, "rb")) == nullptr)
            return false;

        EVP_DigestInit(ctx, EVP_sm3());

        while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
            EVP_DigestUpdate(ctx, buf, n);

        uint32_t len;
        EVP_DigestFinal(ctx, output, &len);
        EVP_MD_CTX_free(ctx);

        if (ferror(f) != 0) {
            fclose(f);
            return false;
        }

        fclose(f);
        return true;
    }

    bool Sm3Provider::computeFileHash(const String &path, ByteArray &output) {
        uint8_t buffer[Length];
        bool result = computeFileHash(path, buffer);
        if (result) {
            output = ByteArray(buffer, Length);
        }
        return result;
    }

    bool Sm3Provider::computeFileHash(const String &path, String &output) {
        ByteArray array;
        if (computeFileHash(path, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    Sm4Provider::Sm4Provider(const uint8_t key[Length], CypherMode mode) : _mode(mode) {
        memcpy(_key, key, Length);
        generateKey(_iv);
    }

    Sm4Provider::Sm4Provider(const uint8_t key[Length], const uint8_t iv[Length], CypherMode mode) : _mode(mode) {
        memcpy(_key, key, Length);
        memcpy(_iv, iv, Length);
    }

    Sm4Provider::Sm4Provider(const ByteArray &key, CypherMode mode) : _mode(mode) {
        memset(_key, 0, Length);
        memcpy(_key, key.data(), Math::min(Length, (int) key.count()));
        generateKey(_iv);
    }

    Sm4Provider::Sm4Provider(const ByteArray &key, const ByteArray &iv, CypherMode mode) : _mode(mode) {
        memset(_key, 0, Length);
        memcpy(_key, key.data(), Math::min(Length, (int) key.count()));
        memset(_iv, 0, Length);
        memcpy(_iv, key.data(), Math::min(Length, (int) iv.count()));
    }

    Sm4Provider::Sm4Provider(const String &key, CypherMode mode) : _mode(mode) {
        ByteArray keyArray;
        ByteArray::parseHexString(key, keyArray);
        memset(_key, 0, Length);
        memcpy(_key, keyArray.data(), Math::min(Length, (int) keyArray.count()));
        generateKey(_iv);
    }

    Sm4Provider::Sm4Provider(const String &key, const String &iv, CypherMode mode) : _mode(mode) {
        ByteArray keyArray;
        ByteArray::parseHexString(key, keyArray);
        memset(_key, 0, Length);
        memcpy(_key, keyArray.data(), Math::min(Length, (int) keyArray.count()));
        ByteArray ivArray;
        ByteArray::parseHexString(iv, ivArray);
        memset(_iv, 0, Length);
        memcpy(_iv, ivArray.data(), Math::min(Length, (int) ivArray.count()));
    }

    bool Sm4Provider::encrypt(const ByteArray &data, ByteArray &output) {
        return encrypt(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::decrypt(const ByteArray &data, ByteArray &output) {
        return decrypt(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::encrypt(const String &data, String &output) {
        return encrypt(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::decrypt(const String &data, String &output) {
        return decrypt(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::encryptToBase64(const String &data, String &output) {
        return encryptToBase64(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::decryptFromBase64(const String &data, String &output) {
        return decryptFromBase64(_key, _iv, data, output, _mode);
    }

    bool Sm4Provider::encrypt(const uint8_t key[Length], const ByteArray &data, ByteArray &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return encrypt(key, iv, data, output, mode);
    }

    bool
    Sm4Provider::encrypt(const uint8_t key[Length], const uint8_t iv[Length], const ByteArray &data, ByteArray &output,
                         CypherMode mode) {
        static const EVP_CIPHER *ciphers[] = {EVP_sm4_ecb(), EVP_sm4_cbc(), EVP_sm4_ofb(), EVP_sm4_ctr()};
        const EVP_CIPHER *cipher = ciphers[mode];

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }
        if (!EVP_CIPHER_CTX_init(ctx)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_EncryptInit_ex(ctx, cipher, nullptr, key, iv)) {
            return false;
        }
        if (!EVP_CIPHER_CTX_set_padding(ctx, 1)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        auto out = (uint8_t *) malloc(((data.count() >> 4) + 1) << 4);
        int currentLen = 0;
        if (!EVP_EncryptUpdate(ctx, out, &currentLen, data.data(), (int) data.count())) {
            EVP_CIPHER_CTX_free(ctx);
            free(out);
            return false;
        }
        int len = currentLen;
        if (!EVP_EncryptFinal_ex(ctx, out + len, &currentLen)) {
            EVP_CIPHER_CTX_free(ctx);
            free(out);
            return false;
        }
        len += currentLen;

        output = ByteArray(out, len);
        free(out);
        EVP_CIPHER_CTX_free(ctx);

        return true;
    }

    bool Sm4Provider::decrypt(const uint8_t key[Length], const ByteArray &data, ByteArray &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return decrypt(key, iv, data, output, mode);
    }

    bool
    Sm4Provider::decrypt(const uint8_t key[Length], const uint8_t iv[Length], const ByteArray &data, ByteArray &output,
                         CypherMode mode) {
        static const EVP_CIPHER *ciphers[] = {EVP_sm4_ecb(), EVP_sm4_cbc(), EVP_sm4_ofb(), EVP_sm4_ctr()};
        const EVP_CIPHER *cipher = ciphers[mode];

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }
        if (!EVP_CIPHER_CTX_init(ctx)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_DecryptInit_ex(ctx, cipher, nullptr, key, iv)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_CIPHER_CTX_set_padding(ctx, 1)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        uint8_t out[1024 + EVP_MAX_BLOCK_LENGTH];
        int currentLen = 0;
        if (!EVP_DecryptUpdate(ctx, out, &currentLen, data.data(), (int) data.count())) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        int len = currentLen;
        if (!EVP_DecryptFinal_ex(ctx, out + len, &currentLen)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        len += currentLen;

        output = ByteArray(out, len);
        EVP_CIPHER_CTX_free(ctx);

        return true;
    }

    bool Sm4Provider::encrypt(const uint8_t key[Length], const String &data, String &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return encrypt(key, iv, data, output, mode);
    }

    bool Sm4Provider::encrypt(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                              CypherMode mode) {
        ByteArray in((uint8_t *) data.c_str(), data.length());
        ByteArray out;
        if (encrypt(key, iv, in, out, mode)) {
            output = out.toString(ByteArray::HexFormat, String::Empty);
            return true;
        }
        return false;
    }

    bool Sm4Provider::decrypt(const uint8_t key[Length], const String &data, String &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return decrypt(key, iv, data, output, mode);
    }

    bool Sm4Provider::decrypt(const uint8_t key[Length], const uint8_t iv[Length], const String &data, String &output,
                              CypherMode mode) {
        ByteArray in;
        if (ByteArray::parse(data, in, String::Empty)) {
            ByteArray out;
            if (decrypt(key, iv, in, out, mode)) {
                output = String((const char *) out.data(), out.count());
                return true;
            }
        }
        return false;
    }

    bool Sm4Provider::encryptToBase64(const uint8_t key[Length], const String &data, String &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return encryptToBase64(key, iv, data, output, mode);
    }

    bool Sm4Provider::encryptToBase64(const uint8_t key[Length], const uint8_t iv[Length], const String &data,
                                      String &output, CypherMode mode) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encrypt(key, iv, array, buffer, mode)) {
            output = String::toBase64(buffer.data(), 0, buffer.count());
            return true;
        }
        return false;
    }

    bool
    Sm4Provider::decryptFromBase64(const uint8_t key[Length], const String &data, String &output, CypherMode mode) {
        uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
        generateKey(iv);
        return decryptFromBase64(key, iv, data, output, mode);
    }

    bool
    Sm4Provider::decryptFromBase64(const uint8_t key[Length], const uint8_t iv[Length], const String &data,
                                   String &output, CypherMode mode) {
        ByteArray in;
        if (String::fromBase64(data, in)) {
            ByteArray out;
            if (decrypt(key, iv, in, out, mode)) {
                output = String((const char *) out.data(), out.count());
                return true;
            }
        }
        return false;
    }

    void Sm4Provider::generateKey(uint8_t key[Length]) {
        ByteArray values;
        Random::getRandValues(Length, values);
        if (values.count() == Length) {
            memcpy(key, values.data(), Length);
        }
    }

    void Sm4Provider::generateKey(ByteArray &key) {
        ByteArray values;
        Random::getRandValues(Length, values);
        if (values.count() == Length) {
            key = values;
        }
    }
}
