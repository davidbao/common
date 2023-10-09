//
//  FileTest.cpp
//  common
//
//  Created by baowei on 2023/10/8.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/FileInfo.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "system/Math.h"

using namespace IO;
using namespace System;

String getTempFileName(const char *prefix = nullptr) {
    String tempFileName = Path::combine(Path::getTempPath(),
                                        String::format("%d%s.bin",
                                                       (int) (DateTime::now().total1970Milliseconds() / 1000),
                                                       prefix != nullptr ? prefix : ""));
    return tempFileName;
}

bool testConstructor() {
    {
        DateTime now = DateTime::now();
        String tempFileName = getTempFileName();
        FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeByte(1);
        fs.close();
        FileInfo fi(tempFileName);
        if (!fi.exists()) {
            return false;
        }
        if (fi.isReadOnly()) {
            File::deleteFile(tempFileName);
            return false;
        }
        if (!fi.isWritable()) {
            File::deleteFile(tempFileName);
            return false;
        }
        if (fi.size() != 1) {
            return false;
        }
        DateTime time = fi.modifiedTime();
//        printf("now: %s, time: %s\n", now.toString().c_str(), time.toString().c_str());
        if (Math::abs(now.total1970Milliseconds() / 1000 - time.total1970Milliseconds() / 1000) > 10) {
            return false;
        }

        File::deleteFile(tempFileName);
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    return 0;
}