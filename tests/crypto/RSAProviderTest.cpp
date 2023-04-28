//
//  RSAProviderTest.cpp
//  common
//
//  Created by baowei on 2023/4/26.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "crypto/RSAProvider.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Crypto;
using namespace IO;

static const char *_plainText = "ABC/abc123,)_中文";
static const ByteArray _plainBuffer = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};

static const char *_pubKey = "-----BEGIN PUBLIC KEY-----\n"
                             "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDYd93oKuhrz1Q+JVYtlZvfk29v\n"
                             "uxsfgmBN1QQjssKt+nmGBdUS697ZGhekbIxds5FCBtmpkI5vcf8ZZc2GEr2XyPWv\n"
                             "OAcNI8T0OYavGSe+yeFuMRvquOBJr06TFlAS61ffvr7YUbGzdfc4ev2Gkpb3aS6+\n"
                             "iSX0cl2L4en/Acg0dQIDAQAB\n"
                             "-----END PUBLIC KEY-----";
static const char *_priKey = "-----BEGIN RSA PRIVATE KEY-----\n"
                             "MIICXAIBAAKBgQDYd93oKuhrz1Q+JVYtlZvfk29vuxsfgmBN1QQjssKt+nmGBdUS\n"
                             "697ZGhekbIxds5FCBtmpkI5vcf8ZZc2GEr2XyPWvOAcNI8T0OYavGSe+yeFuMRvq\n"
                             "uOBJr06TFlAS61ffvr7YUbGzdfc4ev2Gkpb3aS6+iSX0cl2L4en/Acg0dQIDAQAB\n"
                             "AoGBAMXBvBi+GldjsbKUqYY7h4tZRhTs+npuwUxZrs5uISyQBzVJXr9KfOwZyj+M\n"
                             "ay+vPKT1XSRry7REkKjDOF+j/09dFgWwr1lagn9eLmCvJwDsKxwmR05+Ssq11RQV\n"
                             "skwVrmYK/B+eWkv3MR5v5l0pdJSomHp4hcQvK+T6BgTWRKwBAkEA8q6nsFNqR1jc\n"
                             "9GcVt6HML7mNr2HECKu2D8BCgyVRW1pt2l221AHfaE3AmxCY/BZTjZFLCv7xdYot\n"
                             "+duOtdhA2QJBAORY8Q/jwXe8kf/UwrZ1dbxPhU6KLGk4x507FSWK78Z4VQze0+9H\n"
                             "HOgMKeFF8gkWD7AvbjiB4LEXsXs7u5D+Tv0CQF3tFT/ceRTWIuaBA1nISHPsOBc6\n"
                             "GppUgymDevqxNdTKohvTC5DwjJwdA7/AY8Wlin8PTeEanS+l5HeA77IX2bkCQDk2\n"
                             "H4+/nKWHVX0nrXFRlKINunSnIeFrHHhEaVPyr4imdYi3+5mj05ZAFe4fPDkSkfOu\n"
                             "rorx333/VbRujtwRG3ECQAp3siJvrNAwQd+RX4BfG7f7e5D7v4vYmXwEAimN9tIH\n"
                             "SOaOz4GElaRZt083eUTUnlgJrxyXGCo4I9C2HFspy9s=\n"
                             "-----END RSA PRIVATE KEY-----";

bool testDefaultKey(RSAProvider::KeySize size) {
    {
        String data;
        if (!RSAProvider::encrypt(_plainText, data, size)) {
            return false;
        }
        String plain;
        if (!RSAProvider::decrypt(data, plain, size)) {
            return false;
        }
        if (plain != _plainText) {
            return false;
        }
    }

    {
        ByteArray data;
        if (!RSAProvider::encrypt(_plainBuffer, data, size)) {
            return false;
        }
        ByteArray plain;
        if (!RSAProvider::decrypt(data, plain, size)) {
            return false;
        }
        if (plain != _plainBuffer) {
            return false;
        }
    }

    {
        String data;
        if (!RSAProvider::encryptToBase64(_plainText, data, size)) {
            return false;
        }
        String plain;
        if (!RSAProvider::decryptFromBase64(data, plain, size)) {
            return false;
        }
        if (plain != _plainText) {
            return false;
        }
    }

    return true;
}

bool testKey() {
    {
        String data;
        if (!RSAProvider::encryptByPubKey(_pubKey, _plainText, data)) {
            return false;
        }
        String plain;
        if (!RSAProvider::decryptByPriKey(_priKey, data, plain)) {
            return false;
        }
        if (plain != _plainText) {
            return false;
        }
    }

    {
        ByteArray data;
        if (!RSAProvider::encryptByPubKey(_pubKey, _plainBuffer, data)) {
            return false;
        }
        ByteArray plain;
        if (!RSAProvider::decryptByPriKey(_priKey, data, plain)) {
            return false;
        }
        if (plain != _plainBuffer) {
            return false;
        }
    }

    return true;
}

bool testKeyFile() {
    String pubFileName = Path::getTempFileName("RSAProviderTest");
    String priFileName = Path::getTempFileName("RSAProviderTest");
    {
        FileStream fs(pubFileName, FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(_pubKey);
    }
    {
        FileStream fs(priFileName, FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(_priKey);
    }

    bool result = true;
    {
        String data;
        if (!RSAProvider::encryptByPubKeyFile(pubFileName, _plainText, data)) {
            result = false;
            goto cleanUp;
        }
        String plain;
        if (!RSAProvider::decryptByPriKeyFile(priFileName, data, plain)) {
            result = false;
            goto cleanUp;
        }
        if (plain != _plainText) {
            result = false;
            goto cleanUp;
        }
    }

    {
        ByteArray data;
        if (!RSAProvider::encryptByPubKeyFile(pubFileName, _plainBuffer, data)) {
            result = false;
            goto cleanUp;
        }
        ByteArray plain;
        if (!RSAProvider::decryptByPriKeyFile(priFileName, data, plain)) {
            result = false;
            goto cleanUp;
        }
        if (plain != _plainBuffer) {
            result = false;
            goto cleanUp;
        }
    }

cleanUp:
    if (File::exists(pubFileName)) {
        File::deleteFile(pubFileName);
    }
    if (File::exists(priFileName)) {
        File::deleteFile(priFileName);
    }

    return result;
}

int main() {
    if (!testDefaultKey(RSAProvider::Key512)) {
        return 1;
    }
    if (!testDefaultKey(RSAProvider::Key256)) {
        return 2;
    }
    if (!testDefaultKey(RSAProvider::Key1024)) {
        return 3;
    }
    if (!testDefaultKey(RSAProvider::Key2048)) {
        return 4;
    }

    if (!testKey()) {
        return 5;
    }

    if (!testKeyFile()) {
        return 6;
    }

    return 0;
}