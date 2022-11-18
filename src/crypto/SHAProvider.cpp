//
//  SHAProvider.cpp
//  common
//
//  Created by baowei on 2021/5/21.
//  Copyright © 2021 com. All rights reserved.
//

#include "crypto/SHAProvider.h"
#include <openssl/sha.h>

namespace Common
{
    bool SHAProvider::computeHash(const String& src, ByteArray& output, KeySize size)
    {
        if(src.isNullOrEmpty())
            return false;
        
        return computeHash((const uint8_t*)src.c_str(), src.length(), output, size);
    }
    bool SHAProvider::computeHash(const ByteArray& src, ByteArray& output, KeySize size)
    {
        if(src.isEmpty())
            return false;
        
        return computeHash(src.data(), src.count(), output, size);
    }
    bool SHAProvider::computeHash(const String& src, String& output, KeySize size)
    {
        ByteArray array;
        if(!SHAProvider::computeHash(src, array, size))
            return false;
        output = array.toString("%02X", String::Empty);
        return true;
    }

    bool SHAProvider::computeHash(const uint8_t* src, uint32_t length, ByteArray& output, KeySize size)
    {
        if(size == KeySize::Key1)
        {
            unsigned char digest[SHA_DIGEST_LENGTH];
            memset(digest, 0, sizeof(digest));
            SHA1(src, length, (unsigned char*)&digest);
            output = ByteArray(digest, sizeof(digest));
        }
        else if(size == KeySize::Key224)
        {
            unsigned char digest[SHA224_DIGEST_LENGTH];
            memset(digest, 0, sizeof(digest));
            SHA224(src, length, (unsigned char*)&digest);
            output = ByteArray(digest, sizeof(digest));
        }
        else if(size == KeySize::Key256)
        {
            unsigned char digest[SHA256_DIGEST_LENGTH];
            memset(digest, 0, sizeof(digest));
            SHA256(src, length, (unsigned char*)&digest);
            output = ByteArray(digest, sizeof(digest));
        }
        else if(size == KeySize::Key384)
        {
            unsigned char digest[SHA256_DIGEST_LENGTH];
            memset(digest, 0, sizeof(digest));
            SHA384(src, length, (unsigned char*)&digest);
            output = ByteArray(digest, sizeof(digest));
        }
        else if(size == KeySize::Key512)
        {
            unsigned char digest[SHA256_DIGEST_LENGTH];
            memset(digest, 0, sizeof(digest));
            SHA512(src, length, (unsigned char*)&digest);
            output = ByteArray(digest, sizeof(digest));
        }
        else
        {
            return false;
        }
        return true;
    }
}
