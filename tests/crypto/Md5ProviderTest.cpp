//
//  Md5ProviderTest.cpp
//  common
//
//  Created by baowei on 2022/10/21.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "crypto/Md5Provider.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Crypto;

static const String _plainText = "ABC/abc123,)_中文";

bool testMd5() {
    Md5Provider sm3;
    String cypherText;
    sm3.computeHash(_plainText, cypherText);
    String expect = "0587236E6F70BFB675B3BBD33BA5C095";
    if(cypherText != expect) {
        return false;
    }

    return true;
}

bool testMd5_file() {
    Md5Provider sm3;
    String fileName = Path::combine(Path::getTempPath(), "test_md5_file.txt");
    FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    for (int i = 0; i < 100; ++i) {
        fs.writeText(_plainText);
    }
    fs.close();
    String cypherText;
    sm3.computeFileHash(fileName, cypherText);
    File::deleteFile(fileName);
    String expect = "463E0E11B9A09B5C33D82FD9BB351BBA";
    if(cypherText != expect) {
        return false;
    }
    return true;
}

int main() {
    if(!testMd5()) {
        return 1;
    }
    if(!testMd5_file()) {
        return 2;
    }
    return 0;
}