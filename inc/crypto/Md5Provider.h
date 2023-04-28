//
//  Md5Provider.h
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef MD5Provider_h
#define MD5Provider_h

#include "crypto/Algorithm.h"

namespace Crypto {
    class Md5Provider : public HashAlgorithm {
    public:
        Md5Provider();

        int hashSize() const override;

    protected:
        const EVP_MD *type() const override;
    };
}

#endif // MD5Provider_h
