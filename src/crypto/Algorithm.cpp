//
//  Algorithm.cpp
//  common
//
//  Created by baowei on 2023/4/27.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "crypto/Algorithm.h"
#include "data/DateTime.h"
#include <openssl/rand.h>
#include <openssl/err.h>

namespace Crypto {
    SymmetricAlgorithm::SymmetricAlgorithm() :
            _mode(CypherMode::CBC), _padding(PaddingMode::PKCS7) {
    }

    int SymmetricAlgorithm::feedbackSize() const {
        return blockSize();
    }

    bool SymmetricAlgorithm::encrypt(const ByteArray &data, ByteArray &output) {
        return encrypt(_key, _iv, data, output, _mode);
    }

    bool SymmetricAlgorithm::decrypt(const ByteArray &data, ByteArray &output) {
        return decrypt(_key, _iv, data, output, _mode);
    }

    bool SymmetricAlgorithm::encrypt(const String &data, String &output) {
        return encrypt(_key, _iv, data, output, _mode);
    }

    bool SymmetricAlgorithm::decrypt(const String &data, String &output) {
        return decrypt(_key, _iv, data, output, _mode);
    }

    bool SymmetricAlgorithm::encryptToBase64(const String &data, String &output) {
        return encryptToBase64(_key, _iv, data, output, _mode);
    }

    bool SymmetricAlgorithm::decryptFromBase64(const String &data, String &output) {
        return decryptFromBase64(_key, _iv, data, output, _mode);
    }

    const ByteArray &SymmetricAlgorithm::key() const {
        return _key;
    }

    const ByteArray &SymmetricAlgorithm::IV() const {
        return _iv;
    }

    bool SymmetricAlgorithm::encrypt(const ByteArray &key, const ByteArray &iv,
                                     const ByteArray &data, ByteArray &output, CypherMode mode) {
        const EVP_CIPHER *cipher = this->cipher(mode);
        if (cipher == nullptr) {
            return false;
        }

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }
        if (!EVP_CIPHER_CTX_init(ctx)) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), iv.data())) {
            ERR_print_errors_fp(stderr);
            return false;
        }
        if (!EVP_CIPHER_CTX_set_padding(ctx, _padding)) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        auto out = (uint8_t *) malloc(((data.count() >> 4) + 1) << 4);
        int currentLen = 0;
        if (!EVP_EncryptUpdate(ctx, out, &currentLen, data.data(), (int) data.count())) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            free(out);
            return false;
        }
        int len = currentLen;
        if (!EVP_EncryptFinal_ex(ctx, out + len, &currentLen)) {
            ERR_print_errors_fp(stderr);
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

    bool SymmetricAlgorithm::decrypt(const ByteArray &key, const ByteArray &iv,
                                     const ByteArray &data, ByteArray &output, CypherMode mode) {
        const EVP_CIPHER *cipher = this->cipher(mode);
        if (cipher == nullptr) {
            return false;
        }

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            return false;
        }
        if (!EVP_CIPHER_CTX_init(ctx)) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_DecryptInit_ex(ctx, cipher, nullptr, key.data(), iv.data())) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        if (!EVP_CIPHER_CTX_set_padding(ctx, _padding)) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        uint8_t out[1024 + EVP_MAX_BLOCK_LENGTH];
        int currentLen = 0;
        if (!EVP_DecryptUpdate(ctx, out, &currentLen, data.data(), (int) data.count())) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        int len = currentLen;
        if (!EVP_DecryptFinal_ex(ctx, out + len, &currentLen)) {
            ERR_print_errors_fp(stderr);
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        len += currentLen;

        output = ByteArray(out, len);
        EVP_CIPHER_CTX_free(ctx);

        return true;
    }

    bool SymmetricAlgorithm::encrypt(const ByteArray &key, const ByteArray &iv,
                                     const String &data, String &output, CypherMode mode) {
        ByteArray in((uint8_t *) data.c_str(), data.length());
        ByteArray out;
        if (encrypt(key, iv, in, out, mode)) {
            output = out.toString(ByteArray::HexFormat, String::Empty);
            return true;
        }
        return false;
    }

    bool SymmetricAlgorithm::decrypt(const ByteArray &key, const ByteArray &iv,
                                     const String &data, String &output, CypherMode mode) {
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

    bool SymmetricAlgorithm::encryptToBase64(const ByteArray &key, const ByteArray &iv,
                                             const String &data, String &output, CypherMode mode) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        ByteArray buffer;
        if (encrypt(key, iv, array, buffer, mode)) {
            output = String::toBase64(buffer.data(), 0, buffer.count());
            return true;
        }
        return false;
    }

    bool SymmetricAlgorithm::decryptFromBase64(const ByteArray &key, const ByteArray &iv,
                                               const String &data, String &output, CypherMode mode) {
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

    bool SymmetricAlgorithm::validKeySize(int bitLength) const {
        const KeySizes &validSizes = _legalKeySizes;
        size_t i, j;

        if (validSizes.isEmpty())
            return false;
        for (i = 0; i < validSizes.count(); i++) {
            if (validSizes[i].skipSize == 0) {
                if (validSizes[i].minSize == bitLength) { // assume MinSize = MaxSize
                    return true;
                }
            } else {
                for (j = validSizes[i].minSize; j <= validSizes[i].maxSize;
                     j += validSizes[i].skipSize) {
                    if (j == bitLength) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool SymmetricAlgorithm::validIVSize(int bitLength) const {
        return blockSize() == bitLength;
    }

    void SymmetricAlgorithm::generateKey() {
        int keySize = this->keySize() / 8;
        String time = DateTime::now().toString();
        RAND_seed(time, (int) time.length());

        auto sBufOut = new uint8_t[keySize];
        RAND_bytes(sBufOut, keySize);
        _key = ByteArray(sBufOut, keySize);
        delete[] sBufOut;
    }

    void SymmetricAlgorithm::generateIV() {
        int IVSize = this->blockSize() / 8;
        String time = DateTime::now().toString();
        RAND_seed(time, (int) time.length());

        auto sBufOut = new uint8_t[IVSize];
        RAND_bytes(sBufOut, IVSize);
        _iv = ByteArray(sBufOut, IVSize);
        delete[] sBufOut;
    }

    CypherMode SymmetricAlgorithm::mode() const {
        return _mode;
    }

    void SymmetricAlgorithm::setMode(CypherMode mode) {
        _mode = mode;
    }

    PaddingMode SymmetricAlgorithm::padding() const {
        return _padding;
    }

    void SymmetricAlgorithm::setPaddingMode(PaddingMode padding) {
        _padding = padding;
    }

    HashAlgorithm::HashAlgorithm() {
    }

    bool HashAlgorithm::computeHash(const String &data, ByteArray &output) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        return computeHash(array, output);
    }

    bool HashAlgorithm::computeHash(const ByteArray &data, ByteArray &output) {
        const EVP_MD *type = this->type();
        if (type == nullptr) {
            return false;
        }

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        unsigned char buffer[EVP_MAX_MD_SIZE] = {0};
        unsigned int out_size = 0;
        int result = EVP_Digest(data.data(), data.count(), buffer, &out_size, type, nullptr);
        EVP_MD_CTX_free(ctx);
        if (result) {
            output = ByteArray(buffer, out_size);
            return true;
        }
        return false;
    }

    bool HashAlgorithm::computeHash(const String &data, String &output) {
        ByteArray array;
        if (computeHash(data, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool HashAlgorithm::computeFileHash(const String &path, ByteArray &output) {
        const EVP_MD *type = this->type();
        if (type == nullptr) {
            return false;
        }

        FILE *f;
        size_t n;
        unsigned char buf[1024];

        if ((f = fopen(path, "rb")) == nullptr)
            return false;

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        EVP_DigestInit(ctx, type);

        while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
            EVP_DigestUpdate(ctx, buf, n);

        unsigned char buffer[EVP_MAX_MD_SIZE] = {0};
        unsigned int len;
        EVP_DigestFinal(ctx, buffer, &len);
        EVP_MD_CTX_free(ctx);

        output = ByteArray(buffer, len);

        if (ferror(f) != 0) {
            fclose(f);
            return false;
        }

        fclose(f);
        return true;
    }

    bool HashAlgorithm::computeFileHash(const String &path, String &output) {
        ByteArray array;
        if (computeFileHash(path, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }
}
