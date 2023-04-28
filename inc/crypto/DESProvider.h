//
//  DESProvider.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef DESProvider_h
#define DESProvider_h

#include "crypto/Algorithm.h"

namespace Crypto {
    class DESProvider : public SymmetricAlgorithm {
    public:
        DESProvider();

        explicit DESProvider(const ByteArray &key, const ByteArray &iv = ByteArray::Empty);

        explicit DESProvider(const String &key, const ByteArray &iv = ByteArray::Empty);

        int keySize() const override;

        int blockSize() const override;

    protected:
        const EVP_CIPHER *cipher(CypherMode mode) const override;

    private:
        void initKeySizes();
    };

    class TripleDESProvider : public SymmetricAlgorithm {
    public:
        TripleDESProvider();

        explicit TripleDESProvider(const ByteArray &key, const ByteArray &iv = ByteArray::Empty);

        explicit TripleDESProvider(const String &key, const ByteArray &iv = ByteArray::Empty);

        int keySize() const override;

        int blockSize() const override;

    protected:
        const EVP_CIPHER *cipher(CypherMode mode) const override;

    private:
        void initKeySizes();
    };
}

#endif // DESProvider_h
