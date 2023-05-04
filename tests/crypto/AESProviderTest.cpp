//
//  AESProviderTest.cpp
//  common
//
//  Created by baowei on 2023/4/28.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "crypto/AESProvider.h"
#include "IO/FileStream.h"

using namespace Crypto;
using namespace IO;

static const char *_plainText = "ABC/abc123,)_中文";
static const ByteArray _plainBuffer = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static const char *_password = "123.com";

static const ByteArray AES_Keys[3] = {
        {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22,
         0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF},
        {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22,
         0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF,
         0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22},
        {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22,
         0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF,
         0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22,
         0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF}
};
static const ByteArray AES_IV = {0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF,
                                 0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22};

bool testAES_StringKey(AESProvider::KeySize keySizes) {
    {
        AESProvider test(keySizes, _password);
        test.setMode(CypherMode::ECB);
        test.setPaddingMode(PaddingMode::PKCS7);

        String data;
        if (!test.encrypt(_plainText, data)) {
            return false;
        }
        String plain;
        if (!test.decrypt(data, plain)) {
            return false;
        }
        if (plain != _plainText) {
            return false;
        }
    }
    return true;
}

bool testAES_Modes(AESProvider::KeySize keySizes) {
    static const CypherMode modes[] = {CypherMode::CBC, CypherMode::ECB,
                                       CypherMode::OFB, CypherMode::CFB};
    for (auto mode: modes) {
        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setMode(mode);

            String data;
            if (!test.encrypt(_plainText, data)) {
                return false;
            }
            String plain;
            if (!test.decrypt(data, plain)) {
                return false;
            }
            if (plain != _plainText) {
                return false;
            }
        }

        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setMode(mode);

            ByteArray data;
            if (!test.encrypt(_plainBuffer, data)) {
                return false;
            }
            ByteArray plain;
            if (!test.decrypt(data, plain)) {
                return false;
            }
            if (plain != _plainBuffer) {
                return false;
            }
        }

        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setMode(mode);

            String data;
            if (!test.encryptToBase64(_plainText, data)) {
                return false;
            }
            String plain;
            if (!test.decryptFromBase64(data, plain)) {
                return false;
            }
            if (plain != _plainText) {
                return false;
            }
        }
    }

    return true;
}

bool testAES_Paddings(AESProvider::KeySize keySizes) {
    static const PaddingMode paddings[] = {
                                           PaddingMode::PKCS7,
                                           PaddingMode::ISO7816_4,
                                           PaddingMode::ANSI923,
                                           PaddingMode::ISO10126,
                                           PaddingMode::ZERO};
    for (auto padding: paddings) {
        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setPaddingMode(padding);

            String data;
            if (!test.encrypt(_plainText, data)) {
                return false;
            }
            String plain;
            if (!test.decrypt(data, plain)) {
                return false;
            }
            if (plain != _plainText) {
                return false;
            }
        }

        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setPaddingMode(padding);

            ByteArray data;
            if (!test.encrypt(_plainBuffer, data)) {
                return false;
            }
            ByteArray plain;
            if (!test.decrypt(data, plain)) {
                return false;
            }
            if (plain != _plainBuffer) {
                return false;
            }
        }

        {
            AESProvider test(keySizes, AES_Keys[keySizes], AES_IV);
            test.setPaddingMode(padding);

            String data;
            if (!test.encryptToBase64(_plainText, data)) {
                return false;
            }
            String plain;
            if (!test.decryptFromBase64(data, plain)) {
                return false;
            }
            if (plain != _plainText) {
                return false;
            }
        }
    }

    return true;
}

bool testAES_Base64(AESProvider::KeySize keySizes) {
    {
        AESProvider test(keySizes, _password);
        test.setMode(CypherMode::ECB);
        test.setPaddingMode(PaddingMode::PKCS7);

        String data;
        if (!test.encryptToBase64(_plainText, data)) {
            return false;
        }
        String plain;
        if (!test.decryptFromBase64(data, plain)) {
            return false;
        }
        if (plain != _plainText) {
            return false;
        }
    }
    return true;
}

int main() {
    static const AESProvider::KeySize keySizes[] = {
            AESProvider::Aes128,
            AESProvider::Aes192,
            AESProvider::Aes256};
    for (auto keySize: keySizes) {
        if (!testAES_StringKey(keySize)) {
            return 1;
        }
        if (!testAES_Modes(keySize)) {
            return 2;
        }
        if (!testAES_Paddings(keySize)) {
            return 3;
        }
        if (!testAES_Base64(keySize)) {
            return 4;
        }
    }

    return 0;
}