//
//  Algorithm.h
//  common
//
//  Created by baowei on 2023/4/27.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef Algorithm_h
#define Algorithm_h

#include "data/Vector.h"
#include "data/ByteArray.h"
#include <openssl/ec.h>
#include <openssl/evp.h>

using namespace Data;

namespace Crypto {
    enum CypherMode {
        CBC = 0,
        ECB = 1,
        OFB = 2,
        CFB = 3,
        CTR = 4
    };

    enum PaddingMode {
        None = 0,
        PKCS7 = EVP_PADDING_PKCS7,
        ISO7816_4 = EVP_PADDING_ISO7816_4,
        ANSI923 = EVP_PADDING_ANSI923,
        ISO10126 = EVP_PADDING_ISO10126,
        ZERO = EVP_PADDING_ZERO
    };

    struct KeySizes : IEquatable<KeySizes> {
        int minSize;
        int maxSize;
        int skipSize;

        KeySizes() : KeySizes(0, 0, 0) {
        }

        KeySizes(int minSize, int maxSize, int skipSize) : minSize(minSize), maxSize(maxSize), skipSize(skipSize) {
        }

        KeySizes(const KeySizes &other) : minSize(other.minSize), maxSize(other.maxSize), skipSize(other.skipSize) {
        }

        bool equals(const KeySizes &other) const override {
            return minSize == other.minSize && maxSize == other.maxSize && skipSize == other.skipSize;
        }
    };

    class SymmetricAlgorithm {
    public:
        // The key size, in bits
        virtual int keySize() const = 0;

        // The block size, in bits.
        virtual int blockSize() const = 0;

        virtual int feedbackSize() const;

    public:
        bool encrypt(const ByteArray &data, ByteArray &output);

        bool decrypt(const ByteArray &data, ByteArray &output);

        bool encrypt(const String &data, String &output);

        bool decrypt(const String &data, String &output);

        bool encryptToBase64(const String &data, String &output);

        bool decryptFromBase64(const String &data, String &output);

        const ByteArray &key() const;

        const ByteArray &IV() const;

        CypherMode mode() const;

        void setMode(CypherMode mode);

        PaddingMode padding() const;

        void setPaddingMode(PaddingMode padding);

        void generateKey(ByteArray &key);

        void generateIV(ByteArray &iv);

    protected:
        virtual const EVP_CIPHER *cipher(CypherMode mode) const = 0;

    protected:
        SymmetricAlgorithm();

        bool encrypt(const ByteArray &key, const ByteArray &iv,
                     const ByteArray &data, ByteArray &output, CypherMode mode = CypherMode::CBC);

        bool decrypt(const ByteArray &key, const ByteArray &iv,
                     const ByteArray &data, ByteArray &output, CypherMode mode = CypherMode::CBC);

        bool encrypt(const ByteArray &key, const ByteArray &iv,
                     const String &data, String &output, CypherMode mode = CypherMode::CBC);

        bool decrypt(const ByteArray &key, const ByteArray &iv,
                     const String &data, String &output, CypherMode mode = CypherMode::CBC);

        bool encryptToBase64(const ByteArray &key, const ByteArray &iv,
                             const String &data, String &output, CypherMode mode);

        bool decryptFromBase64(const ByteArray &key, const ByteArray &iv,
                               const String &data, String &output, CypherMode mode);

        bool validKeySize(int bitLength) const;

        bool validIVSize(int bitLength) const;

        void generateKey();

        void generateIV();

    protected:
        ByteArray _key;
        ByteArray _iv;

        CypherMode _mode;
        PaddingMode _padding;

        Vector<KeySizes> _legalBlockSizes;
        Vector<KeySizes> _legalKeySizes;
    };

    class AsymmetricAlgorithm {

    };

    class HashAlgorithm {
    public:
        // The hash size, in bits
        virtual int hashSize() const = 0;

        bool computeHash(const String &data, ByteArray &output);

        bool computeHash(const ByteArray &data, ByteArray &output);

        bool computeHash(const String &data, String &output);

        bool computeFileHash(const String &path, ByteArray &output);

        bool computeFileHash(const String &path, String &output);

    protected:
        HashAlgorithm();

        virtual const EVP_MD *type() const = 0;
    };
}

#endif // Algorithm_h
