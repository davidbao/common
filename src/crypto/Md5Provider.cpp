//
//  SmProvider.cpp
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright © 2022 com. All rights reserved.
//

#include "crypto/Md5Provider.h"
#include <openssl/evp.h>

namespace Crypto {
    bool Md5Provider::computeHash(const String &data, ByteArray &output) {
        ByteArray array((uint8_t *) data.c_str(), data.length());
        return computeHash(array, output);
    }

    bool Md5Provider::computeHash(const ByteArray &data, ByteArray &output) {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        uint8_t out[64] = {0};
        unsigned int out_size = 0;
        int result = EVP_Digest(data.data(), data.count(), out, &out_size, EVP_md5(), nullptr);
        EVP_MD_CTX_free(ctx);
        if (result) {
            output = ByteArray(out, out_size);
            return true;
        }
        return false;
    }

    bool Md5Provider::computeHash(const String &data, String &output) {
        ByteArray array;
        if (computeHash(data, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }

    bool Md5Provider::computeFileHash(const String &path, uint8_t output[Length]) {
        FILE *f;
        size_t n;
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        unsigned char buf[1024];

        if ((f = fopen(path, "rb")) == nullptr)
            return false;

        EVP_DigestInit(ctx, EVP_md5());

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

    bool Md5Provider::computeFileHash(const String &path, ByteArray &output) {
        uint8_t buffer[Length];
        bool result = computeFileHash(path, buffer);
        if (result) {
            output = ByteArray(buffer, Length);
        }
        return result;
    }

    bool Md5Provider::computeFileHash(const String &path, String &output) {
        ByteArray array;
        if (computeFileHash(path, array)) {
            output = array.toString("%02X", String::Empty);
            return true;
        }
        return false;
    }
}
