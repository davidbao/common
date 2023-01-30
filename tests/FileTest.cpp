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

using namespace Data;

String getTempFileName(const char *prefix = nullptr) {
    String tempFileName = Path::combine(Path::getTempPath(),
                                        String::format("%d%s.bin",
                                                       (int) (DateTime::now().total1970Milliseconds() / 1000),
                                                       prefix != nullptr ? prefix : ""));
    return tempFileName;
}

bool testAppendLines() {
    {
        String tempFileName = getTempFileName();
        StringArray contents{"1", "2", "3", "4"};
        File::appendLines(tempFileName, contents);
        StringArray actual = File::readAllLines(tempFileName);
        if (contents != actual) {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    {
        String tempFileName = getTempFileName();
        StringArray contents{"1", "2", "3", "4"};
        File::appendLines(tempFileName, contents);

        StringArray contents2{"", "5", "6", "7", "8"};
        File::appendLines(tempFileName, contents2);
        StringArray actual = File::readAllLines(tempFileName);
        if (actual != StringArray{"1", "2", "3", "4", "5", "6", "7", "8"}) {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    return true;
}

bool testAppendText() {
    {
        String tempFileName = getTempFileName();
        File::appendText(tempFileName, "1234");
        String actual = File::readAllText(tempFileName);
        if (actual != "1234") {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    {
        String tempFileName = getTempFileName();
        File::appendText(tempFileName, "1234");

        File::appendText(tempFileName, "5678");
        String actual = File::readAllText(tempFileName);
        if (actual != "12345678") {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    return true;
}

bool testCopy() {
    {
        String tempFileName = getTempFileName("source");
        File::appendText(tempFileName, "1234");
        String tempFileName2 = getTempFileName("target");
        if (!File::copy(tempFileName, tempFileName2)) {
            File::deleteFile(tempFileName);
            File::deleteFile(tempFileName2);
            return false;
        }

        String actual = File::readAllText(tempFileName2);
        File::deleteFile(tempFileName);
        File::deleteFile(tempFileName2);
        if (actual != "1234") {
            return false;
        }
    }

    {
        String tempFileName = getTempFileName("source");
        File::appendText(tempFileName, "1234");
        String tempFileName2 = getTempFileName("target");
        if (!File::copy(tempFileName, tempFileName2, true)) {
            File::deleteFile(tempFileName);
            File::deleteFile(tempFileName2);
            return false;
        }

        String actual = File::readAllText(tempFileName2);
        File::deleteFile(tempFileName);
        File::deleteFile(tempFileName2);
        if (actual != "1234") {
            return false;
        }
    }

    {
        String tempFileName = getTempFileName();
        if (File::copy(String::Empty, tempFileName)) {
            File::deleteFile(tempFileName);
            return false;
        }
    }

    {
        String tempFileName = getTempFileName();
        if (File::copy(tempFileName, String::Empty)) {
            return false;
        }
    }

    {
        String tempFileName = getTempFileName("source");
        String tempFileName2 = getTempFileName("target");
        if (File::copy(tempFileName, tempFileName2)) {
            return false;
        }
    }

    {
        String tempFileName = getTempFileName("source");
        if (File::copy(tempFileName, tempFileName)) {
            return false;
        }
    }

    {
        String tempFileName = getTempFileName("source");
        File::appendText(tempFileName, "1234");
        String tempFileName2 = getTempFileName("target");
        File::appendText(tempFileName2, "5678");
        if (File::copy(tempFileName, tempFileName2, false)) {
            File::deleteFile(tempFileName);
            File::deleteFile(tempFileName2);
            return false;
        }

        File::deleteFile(tempFileName);
        File::deleteFile(tempFileName2);
    }

    return true;
}

bool testFileExits() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    bool result = File::exists(tempFileName);
    File::deleteFile(tempFileName);
    return result;
}

bool testFileDeleted() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if (!File::exists(tempFileName)) {
        return false;
    }
    if (!File::deleteFile(tempFileName)) {
        return false;
    }
    return !File::exists(tempFileName);
}

bool testFileMoved() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if (!File::exists(tempFileName)) {
        return false;
    }
    String tempFileName2 = getTempFileName("-moved");
    if (!File::move(tempFileName, tempFileName2)) {
        File::deleteFile(tempFileName);
        return false;
    }
    File::deleteFile(tempFileName2);
    return true;
}

bool testReadAllBytes() {
    {
        String tempFileName = getTempFileName();
        File::appendBytes(tempFileName, ByteArray{1, 2, 3, 4});
        ByteArray actual = File::readAllBytes(tempFileName);
        if (actual != ByteArray{1, 2, 3, 4}) {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    return true;
}

bool testFileRenamed() {
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if (!File::exists(tempFileName)) {
        return false;
    }
    String tempFileName2 = getTempFileName("-renamed");
    if (!File::rename(tempFileName, tempFileName2)) {
        File::deleteFile(tempFileName);
        return false;
    }
    File::deleteFile(tempFileName2);
    return true;
}

bool testSetFileModifyTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if (File::exists(tempFileName)) {
        if (!File::setModifyTime(tempFileName, time)) {
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
    if (File::exists(tempFileName)) {
        if (!File::setLastAccessTime(tempFileName, time)) {
            return false;
        }
        DateTime time2;
        File::getLastAccessTime(tempFileName, time2);
        result = time.toString() == time2.toString();
    }
    File::deleteFile(tempFileName);
    return result;
}

bool testGetLength() {
    {
        String tempFileName = getTempFileName();
        File::appendText(tempFileName, "1234");
        int64_t fileLength = File::getLength(tempFileName);
        if(fileLength != 4) {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    {
        String tempFileName = getTempFileName();
        File::appendText(tempFileName, "1234");
        int64_t fileLength;
        if (!File::getLength(tempFileName, fileLength)) {
            File::deleteFile(tempFileName);
            return false;
        }
        if(fileLength != 4) {
            File::deleteFile(tempFileName);
            return false;
        }
        File::deleteFile(tempFileName);
    }

    return true;
}

bool testSetCreationTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String tempFileName = getTempFileName();
    FileStream fs(tempFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.close();
    if (File::exists(tempFileName)) {
        if (!File::setCreationTime(tempFileName, time)) {
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
    if (!testAppendLines())
        return 1;

    if (!testAppendText())
        return 2;

    if (!testCopy())
        return 3;

    if (!testFileDeleted())
        return 4;

    if (!testFileExits())
        return 5;

#ifdef WIN32
    if(!testSetCreationTime())
        return 6;
#endif

    if (!testSetLastAccessTime())
        return 7;

    if (!testGetLength())
        return 8;

    if (!testSetFileModifyTime())
        return 9;

    if (!testFileMoved())
        return 10;

    if (!testReadAllBytes())
        return 11;

    if (!testFileRenamed())
        return 12;

    return 0;
}