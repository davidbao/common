//
//  SHAProvider.h
//  common
//
//  Created by baowei on 2021/5/21.
//  Copyright (c) 2021 com. All rights reserved.
//

#ifndef SHAProvider_h
#define SHAProvider_h

#include "crypto/Algorithm.h"

namespace Crypto {
    class SHAProvider : public HashAlgorithm {
    public:
        enum KeySize {
            Key1 = 0,
            Key224 = 1,
            Key256 = 2,
            Key384 = 3,
            Key512 = 4,
            Default = Key256
        };

        SHAProvider(KeySize keySize = Default);

        int hashSize() const override;

    protected:
        const EVP_MD *type() const override;

    private:
        KeySize _keySize;
    };
}

#endif /* SHAProvider_h */
