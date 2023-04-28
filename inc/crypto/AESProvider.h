//
//  AESProvider.h
//  common
//
//  Created by baowei on 2023/4/28.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef AESProvider_h
#define AESProvider_h

#include "crypto/Algorithm.h"

namespace Crypto {
    class AESProvider : public SymmetricAlgorithm {
    public:
        enum Bits {
            Aes128 = 0,
            Aes192,
            Aes256
        };

        explicit AESProvider(Bits bits = Aes256);

        explicit AESProvider(Bits bits, const ByteArray &key, const ByteArray &iv = ByteArray::Empty);

        explicit AESProvider(Bits bits, const String &key, const ByteArray &iv = ByteArray::Empty);

        int keySize() const override;

        int blockSize() const override;

        int feedbackSize() const override;

    protected:
        const EVP_CIPHER *cipher(CypherMode mode) const override;

    private:
        void initKeySizes(Bits bits);

    private:
        int _keySize;
    };
}

#endif // AESProvider_h
