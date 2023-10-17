//
//  DirectoryTest.cpp
//  common
//
//  Created by baowei on 2023/10/16.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/DirectoryInfo.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "system/Math.h"

using namespace IO;
using namespace System;

String getTempDirectoryName(const char *prefix = nullptr) {
    String tempDirectoryName = Path::combine(Path::getTempPath(),
                                        String::format("%d%s",
                                                       (int) (DateTime::now().total1970Milliseconds() / 1000),
                                                       prefix != nullptr ? prefix : ""));
    return tempDirectoryName;
}

bool testConstructor() {
    {
        DateTime now = DateTime::now();
        String tempDirectoryName = getTempDirectoryName();
        Directory::createDirectory(tempDirectoryName);
        DirectoryInfo di(tempDirectoryName);
        if (!di.exists()) {
            return false;
        }
        DateTime modifiedTime = di.modifiedTime();
//        printf("now: %s, modifiedTime: %s\n", now.toString().c_str(), modifiedTime.toString().c_str());
        if (Math::abs(now.total1970Milliseconds() / 1000 - modifiedTime.total1970Milliseconds() / 1000) > 10) {
            return false;
        }
        DateTime creationTime = di.creationTime();
//        printf("now: %s, creationTime: %s\n", now.toString().c_str(), creationTime.toString().c_str());
        if (Math::abs(now.total1970Milliseconds() / 1000 - creationTime.total1970Milliseconds() / 1000) > 10) {
            return false;
        }
        DateTime lastAccessTime = di.lastAccessTime();
//        printf("now: %s, lastAccessTime: %s\n", now.toString().c_str(), lastAccessTime.toString().c_str());
        if (Math::abs(now.total1970Milliseconds() / 1000 - lastAccessTime.total1970Milliseconds() / 1000) > 10) {
            return false;
        }
        Directory::deleteDirectory(tempDirectoryName);
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    return 0;
}