//
//  FileTest.cpp
//  common
//
//  Created by baowei on 2022/10/2.
//  Copyright © 2022 com. All rights reserved.
//

#include "IO/File.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "data/DateTime.h"

using namespace Common;

String getTempFileName(const char* prefix = nullptr) {
    String tempFileName = Path::combine(Directory::getTempPath(),
                                        String::format("%d%s.bin",
                                                       (int)(DateTime::now().total1970Milliseconds() / 1000),
                                                       prefix != nullptr ? prefix : ""));
    return tempFileName;
}
bool testFileExits() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    bool result = File::exists(tempFileName);
    File::deleteFile(tempFileName);
    return result;
};

bool testFileDeleted() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(!File::exists(tempFileName)) {
        return false;
    }
    if(!File::deleteFile(tempFileName)) {
        return false;
    }
    return !File::exists(tempFileName);
};

bool testFileMoved() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(!File::exists(tempFileName)) {
        return false;
    }
    String tempFileName2 = getTempFileName("-moved");
    if(!File::move(tempFileName, tempFileName2)) {
        File::deleteFile(tempFileName);
        return false;
    }
    bool result = File::exists(tempFileName2);
    File::deleteFile(tempFileName2);
    return true;
};

bool testFileRenamed() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(!File::exists(tempFileName)) {
        return false;
    }
    String tempFileName2 = getTempFileName("-renamed");
    if(!File::rename(tempFileName, tempFileName2)) {
        File::deleteFile(tempFileName);
        return false;
    }
    bool result = File::exists(tempFileName2);
    File::deleteFile(tempFileName2);
    return true;
}

bool testSetFileModifyTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(File::exists(tempFileName)) {
        if(!File::setModifyTime(tempFileName, time)) {
            return false;
        }
        DateTime time2;
        File::getModifyTime(tempFileName, time2);
        String str = time.toString();
        String str2 = time2.toString();
        result = time.toString() == time2.toString();
    }
    File::deleteFile(tempFileName);
    return result;
}

bool testSetLastAccessTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(File::exists(tempFileName)) {
        if(!File::setLastAccessTime(tempFileName, time)) {
            return false;
        }
        DateTime time2;
        File::getLastAccessTime(tempFileName, time2);
        result = time.toString() == time2.toString();
    }
    File::deleteFile(tempFileName);
    return result;
}

bool testSetCreationTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if(File::exists(tempFileName)) {
        if(!File::setCreationTime(tempFileName, time)) {
            return false;
        }
        DateTime time2;
        File::getCreationTime(tempFileName, time2);
        result = time.toString() == time2.toString();
    }
    File::deleteFile(tempFileName);
    return result;
}

int main() {
    if(!testFileExits())
        return 1;
    if(!testFileDeleted())
        return 2;
    if(!testFileMoved())
        return 3;
    if(!testFileRenamed())
        return 4;
    if(!testSetFileModifyTime())
        return 5;
    if(!testSetLastAccessTime())
        return 6;
#ifdef WIN32
    if(!testSetCreationTime())
        return 7;
#endif
    return 0;
}