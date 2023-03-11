//
//  SmProviderTest.cpp
//  common
//
//  Created by baowei on 2022/10/19.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "crypto/SmProvider.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Crypto;

static const String _plainText = "ABC/abc123,)_中文";

bool testSm2() {
    // encrypt & decrypt by random key.
    Sm2Provider sm2;
    String cypherText;
    if(!sm2.encrypt(_plainText, cypherText)) {
        return false;
    }
    String plainText2;
    if(!sm2.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // encrypt & decrypt by the specified key.
    ByteArray privateKey;
    ByteArray::parseHexString("0127E2238F923C4530CC2EC87B9C3839497CCB11FD519E4F13C211BC49D86DB9", privateKey);
    ByteArray publicKey;
    ByteArray::parseHexString("FABB215EA4483A3686040F07A9EFCC1E86C6851AE68DCE9F39AF704EC72B310FE810C83E68140557A4DA10390DDEDF3CF6C00EFF6DB6B6996171662AE010330B", publicKey);
    Sm2Provider sm2_2(publicKey, privateKey);

    if(!sm2_2.encrypt(_plainText, cypherText)) {
        return false;
    }
    if(!sm2_2.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // decrypt by the specified key and cypher.
    Sm2Provider sm2_3(publicKey, privateKey);
    String cypherText2 = "307B022067B6A99DBCB82C212875C7B3E299BCD3258FBFC30C9963121A6C42CC62C64206022074DD904BAF95400E9A28CC90BA0E9CD9393BDCCAB7C5B023C8D87598CB96EEA80420F5C9451F280F54356E468ECD0E3FCAC7DD3673326FA2B91C4EBB8469BC14EA970413E0EB88738E08F7C95FB23D903198FE24EDE74D";
    String plainText3;
    if(!sm2_3.decrypt(cypherText2, plainText3)) {
        return false;
    }
    if(_plainText != plainText3) {
        return false;
    }

    return true;
}

bool testSm2_generatePublicKey() {
    ByteArray publicKey, privateKey, publicKey2;
    if(!Sm2Provider::generateKey(publicKey, privateKey)) {
        return false;
    }
    if(!Sm2Provider::generatePublicKey(privateKey, publicKey2)) {
        return false;
    }
    if(publicKey != publicKey2) {
        return false;
    }
    return true;
}

bool testSm3() {
    String cypherText;
    Sm3Provider::computeHash(_plainText, cypherText);
    String expect = "44E82DDE406154A6F3AC0C9348D72E0977FAE7DDAA62B74FF2ABA06B551F9579";
    if(cypherText != expect) {
        return false;
    }

    return true;
}

bool testSm3_file() {
    String fileName = Path::combine(Path::getTempPath(), "test_sm3_file.txt");
    FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    for (int i = 0; i < 100; ++i) {
        fs.writeText(_plainText);
    }
    fs.close();
    String cypherText;
    Sm3Provider::computeFileHash(fileName, cypherText);
    File::deleteFile(fileName);
    String expect = "139A691C9692E8770D680960C2B17477E4F847C19334D63C894DE6ABB8D2CBBA";
    if(cypherText != expect) {
        return false;
    }
    return true;
}

bool testSm4_ecb() {
    // encrypt & decrypt by random key.
    ByteArray key;
    Sm4Provider::generateKey(key);
    String str = key.toString();
    Sm4Provider sm4(key, Sm4Provider::Ecb);
    String cypherText;
    if(!sm4.encrypt(_plainText, cypherText)) {
        return false;
    }
    String plainText2;
    if(!sm4.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // encrypt & decrypt by the specified key.
    static const uint8_t Key[] = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22, 0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
    Sm4Provider sm4_2(Key, Sm4Provider::Ecb);
    String plainText3, cypherText3;
    if(!sm4_2.encrypt(_plainText, cypherText3)) {
        return false;
    }
    if(!sm4_2.decrypt(cypherText3, plainText3)) {
        return false;
    }
    if(_plainText != plainText3) {
        return false;
    }

    // decrypt by the specified key and cypher.
    String plainText4;
    String cypherText4 = "9B3B0DE751C54FDD9D940A45FC6A5AB3E055AAAA89744AC33047073A9050C77B";
    if(!sm4_2.decrypt(cypherText4, plainText4)) {
        return false;
    }
    if(_plainText != plainText4) {
        return false;
    }

    // It has the same cypher in every time.
    String cypherText5, cypherText6;
    Sm4Provider sm4_3(Key, Sm4Provider::Cbc);
    if(!sm4_3.encrypt(_plainText, cypherText5)) {
        return false;
    }
    Sm4Provider sm4_3_2(Key, Sm4Provider::Cbc);
    if(!sm4_3_2.encrypt(_plainText, cypherText6)) {
        return false;
    }
    if(cypherText5 == cypherText6) {
        return false;
    }

    return true;
}

bool testSm4_cbc() {
    // encrypt & decrypt by random key.
    ByteArray key;
    Sm4Provider::generateKey(key);
    String str = key.toString();
    Sm4Provider sm4(key, Sm4Provider::Cbc);
    String cypherText;
    if(!sm4.encrypt(_plainText, cypherText)) {
        return false;
    }
    String plainText2;
    if(!sm4.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // encrypt & decrypt by the specified key.
    static const uint8_t Key[] = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22, 0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
    Sm4Provider sm4_2(Key, Sm4Provider::Cbc);
    String plainText3, cypherText3;
    if(!sm4_2.encrypt(_plainText, cypherText3)) {
        return false;
    }
    if(!sm4_2.decrypt(cypherText3, plainText3)) {
        return false;
    }
    if(_plainText != plainText3) {
        return false;
    }

    // decrypt by the specified key and cypher.
    String plainText4;
    String cypherText4 = "FC8817E28F9F4CCD5735AA9A3D8FE50FC7E8C8B70DE69C5CD3242891EC214119";
    static const uint8_t Iv[] = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22, 0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
    if(!Sm4Provider::decrypt(Key, Iv, cypherText4, plainText4, Sm4Provider::Cbc)) {
        return false;
    }
    if(_plainText != plainText4) {
        return false;
    }

    // It has not the same cypher in every time.
    String cypherText5, cypherText6;
    Sm4Provider sm4_3(Key, Sm4Provider::Cbc);
    if(!sm4_3.encrypt(_plainText, cypherText5)) {
        return false;
    }
    Sm4Provider sm4_3_2(Key, Sm4Provider::Cbc);
    if(!sm4_3_2.encrypt(_plainText, cypherText6)) {
        return false;
    }
    if(cypherText5 == cypherText6) {
        return false;
    }

    return true;
}
bool testSm4_ofb() {
    // encrypt & decrypt by random key.
    ByteArray key;
    Sm4Provider::generateKey(key);
    String str = key.toString();
    Sm4Provider sm4(key, Sm4Provider::Ofb);
    String cypherText;
    if(!sm4.encrypt(_plainText, cypherText)) {
        return false;
    }
    String plainText2;
    if(!sm4.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // encrypt & decrypt by the specified key.
    static const uint8_t Key[] = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22, 0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
    Sm4Provider sm4_2(Key, Sm4Provider::Ofb);
    String plainText3, cypherText3;
    if(!sm4_2.encrypt(_plainText, cypherText3)) {
        return false;
    }
    if(!sm4_2.decrypt(cypherText3, plainText3)) {
        return false;
    }
    if(_plainText != plainText3) {
        return false;
    }

    return true;
}

bool testSm4_ctr() {
    // encrypt & decrypt by random key.
    ByteArray key;
    Sm4Provider::generateKey(key);
    String str = key.toString();
    Sm4Provider sm4(key, Sm4Provider::Ctr);
    String cypherText;
    if(!sm4.encrypt(_plainText, cypherText)) {
        return false;
    }
    String plainText2;
    if(!sm4.decrypt(cypherText, plainText2)) {
        return false;
    }
    if(_plainText != plainText2) {
        return false;
    }

    // encrypt & decrypt by the specified key.
    static const uint8_t Key[] = {0xEB, 0xDE, 0x20, 0x20, 0x63, 0xCC, 0x63, 0x22, 0x66, 0x7A, 0xDF, 0x3F, 0xD9, 0xB2, 0xF8, 0xBF};
    Sm4Provider sm4_2(Key, Sm4Provider::Ctr);
    String plainText3, cypherText3;
    if(!sm4_2.encrypt(_plainText, cypherText3)) {
        return false;
    }
    if(!sm4_2.decrypt(cypherText3, plainText3)) {
        return false;
    }
    if(_plainText != plainText3) {
        return false;
    }

    return true;
}

int main() {
    if(!testSm2()) {
        printf("Failed to testSm2.\n");
        return 1;
    }
    if(!testSm2_generatePublicKey()) {
        printf("Failed to testSm2_generatePublicKey.\n");
        return 2;
    }
    if(!testSm3()) {
        printf("Failed to testSm3.\n");
        return 3;
    }
    if(!testSm3_file()) {
        printf("Failed to testSm3_file.\n");
        return 4;
    }
    if(!testSm4_ecb()) {
        printf("Failed to testSm4_ecb.\n");
        return 5;
    }
    if(!testSm4_cbc()) {
        printf("Failed to testSm4_cbc.\n");
        return 6;
    }
    if(!testSm4_ofb()) {
        printf("Failed to testSm4_ofb.\n");
        return 7;
    }
    if(!testSm4_ctr()) {
        printf("Failed to testSm4_ctr.\n");
        return 8;
    }

    return 0;
}