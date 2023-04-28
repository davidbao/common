//
//  Md5Provider.cpp
//  common
//
//  Created by baowei on 2022/2/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "crypto/Md5Provider.h"

namespace Crypto {
    Md5Provider::Md5Provider() = default;

    int Md5Provider::hashSize() const {
        return 128;
    }

    const EVP_MD *Md5Provider::type() const {
        return EVP_md5();
    }
}
