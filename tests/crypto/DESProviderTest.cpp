//
//  DESProviderTest.cpp
//  common
//
//  Created by baowei on 2023/4/27.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "crypto/DESProvider.h"
#include "IO/FileStream.h"

using namespace Crypto;
using namespace IO;

static const char *_plainText = "ABC/abc123,)_中文";
static const ByteArray _plainBuffer = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static const char *_password = "123.com";

static const ByteArray DES_Key = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22};
static const ByteArray DES_IV = {0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};

static const ByteArray DES3_Key = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22,
                                   0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
static const ByteArray DES3_IV = {0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};

bool testDES_StringKey() {
    {
        DESProvider test(_password);
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

bool testDES_Modes() {
    static const CypherMode modes[] = {CypherMode::CBC, CypherMode::ECB,
                                       CypherMode::OFB, CypherMode::CFB};
    for (auto mode: modes) {
        {
            DESProvider test(DES_Key, DES_IV);
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
            DESProvider test(DES_Key, DES_IV);
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
            DESProvider test(DES_Key, DES_IV);
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

bool testDES_Paddings() {
    static const PaddingMode paddings[] = {
                                           PaddingMode::PKCS7,
                                           PaddingMode::ISO7816_4,
                                           PaddingMode::ANSI923,
                                           PaddingMode::ISO10126,
                                           PaddingMode::ZERO};
    for (auto padding: paddings) {
        {
            DESProvider test(DES_Key, DES_IV);
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
            DESProvider test(DES_Key, DES_IV);
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
            DESProvider test(DES_Key, DES_IV);
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

bool testDES_Base64() {
    {
        DESProvider test(_password);
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

bool testDES3_StringKey() {
    {
        TripleDESProvider test(_password);
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

bool testDES3_Modes() {
    static const CypherMode modes[] = {CypherMode::CBC, CypherMode::ECB,
                                       CypherMode::OFB, CypherMode::CFB};
    for (auto mode: modes) {
        {
            TripleDESProvider test(DES3_Key, DES3_IV);
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
            TripleDESProvider test(DES3_Key, DES3_IV);
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
            TripleDESProvider test(DES3_Key, DES3_IV);
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

bool testDES3_Paddings() {
    static const PaddingMode paddings[] = {
            PaddingMode::PKCS7,
            PaddingMode::ISO7816_4,
            PaddingMode::ANSI923,
            PaddingMode::ISO10126,
            PaddingMode::ZERO};
    for (auto padding: paddings) {
        {
            TripleDESProvider test(DES3_Key, DES3_IV);
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
            TripleDESProvider test(DES3_Key, DES3_IV);
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
            TripleDESProvider test(DES3_Key, DES3_IV);
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

bool testDES3_Base64() {
    {
        TripleDESProvider test(_password);
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
    if (!testDES_StringKey()) {
        return 1;
    }
    if (!testDES_Modes()) {
        return 2;
    }
    if (!testDES_Paddings()) {
        return 3;
    }
    if (!testDES_Base64()) {
        return 4;
    }

    if (!testDES3_StringKey()) {
        return 11;
    }
    if (!testDES3_Modes()) {
        return 12;
    }
    if (!testDES3_Paddings()) {
        return 13;
    }
    if (!testDES3_Base64()) {
        return 14;
    }

    return 0;
}