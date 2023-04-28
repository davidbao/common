//
//  SHAProviderTest.cpp
//  common
//
//  Created by baowei on 2023/4/27.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "crypto/SHAProvider.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Crypto;
using namespace IO;

static const String _plainText = "ABC/abc123,)_中文";

static const String _cypherTexts[] = {
        "5C13A7759583302367B2EDF75D979DADA15D0F17",
        "75380BB7C5B5176357F813AEF74AE788E77D1EAF07E9F6A28E5A27EC",
        "BD60FD3CCEA532406661B2EAB3B77AB80C4BC9B0ABE4D3785161ABD9B43B0444",
        "388F0A6926AE695D813D409907E21A76FD766D3D1D51B4F06D0ED5BA83C8EAB9C1622D7FDB6BBC7A0D07AB767488FEED",
        "CE8B4039A4D35D8133A18476D8002A21F364AD81551F6B656E454C3CADDFACF4893525D5FFFA6AE40B14F9ED5A3B2B9E1B46DA0B2CEEE8ADAFB9E63D2FDB0F27",
};

static const String _cypherFileTexts[] = {
        "0F101E099263C763C2AB187713256CEC37B2B473",
        "FC1F96E088A234186DE263C325A723E0530F8152ED5F582881377291",
        "6A6FDA30421C8E8D30D64970E443CDFE1C74FDF0764038ACE3A30F3949BD3172",
        "B341AC1E94907DE6C6208A5F0A5359DAD9C3530C3E15814314F0C1489A5B08E65ECF7A62D96A9A275E6D24B794A0DC7F",
        "A933FD8B13DA42F046E6B4DFFB81B94E8F1C6175D0C39C67E68F393B37529A2C59D25CF85C8307F57FDC0D09F3223139AEED4216B8BE73112A0A94DBEDDB243F",
};

bool testSHA(SHAProvider::KeySize keySize) {
    SHAProvider sm3(keySize);
    String cypherText;
    sm3.computeHash(_plainText, cypherText);
    String expect = _cypherTexts[keySize];
    if (cypherText != expect) {
        return false;
    }

    return true;
}

bool testSHA_file(SHAProvider::KeySize keySize) {
    SHAProvider sm3(keySize);
    String fileName = Path::combine(Path::getTempPath(), "test_sha_file.txt");
    FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    for (int i = 0; i < 100; ++i) {
        fs.writeText(_plainText);
    }
    fs.close();
    String cypherText;
    sm3.computeFileHash(fileName, cypherText);
    File::deleteFile(fileName);
    if (cypherText != _cypherFileTexts[keySize]) {
        return false;
    }
    return true;
}

int main() {
    static const SHAProvider::KeySize keySizes[] = {
            SHAProvider::Key1,
            SHAProvider::Key224,
            SHAProvider::Key256,
            SHAProvider::Key384,
            SHAProvider::Key512
    };

    for (auto keySize: keySizes) {
        if (!testSHA(keySize)) {
            return 1;
        }
        if (!testSHA_file(keySize)) {
            return 2;
        }
    }

    return 0;
}