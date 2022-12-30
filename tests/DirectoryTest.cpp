//
//  DirectoryTest.cpp
//  common
//
//  Created by baowei on 2022/10/24.
//  Copyright © 2022 com. All rights reserved.
//

#include "IO/Directory.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "system/Application.h"

using namespace System;

static const String _path = Path::combine(Directory::getTempPath(), "directory_test");

void cleanUp() {
    if (Directory::exists(_path)) {
        Directory::deleteDirectory(_path);
    }
}

void setUp() {
    cleanUp();

    if (!Directory::exists(_path)) {
        Directory::createDirectory(_path);
    }
}

bool testCreateDirectory() {
    String path = Path::combine(_path, "create.level1");
    if (Directory::exists(path)) {
        return false;
    }
    if (!Directory::createDirectory(path)) {
        return false;
    }
    if (!Directory::exists(path)) {
        Debug::writeLine("3");
        return false;
    }

    String path2 = Path::combine(_path, "create.level1/level2/level3/level4");
    if (Directory::exists(path2)) {
        return false;
    }
    if (!Directory::createDirectory(path2)) {
        return false;
    }
    if (!Directory::exists(path2)) {
        return false;
    }

    String path3 = Path::combine(_path, "invalid_path:");
    if (Directory::exists(path3)) {
        return false;
    }
    bool result = Directory::createDirectory(path3);
    if (!(!result || (result && Directory::exists(path3)))) {
        return false;
    }

    return true;
}

bool testDeleteDirectory() {
    String path = Path::combine(_path, "delete.level1");
    if (!Directory::createDirectory(path)) {
        return false;
    }
    if (!Directory::deleteDirectory(path)) {
        return false;
    }

    String path2 = Path::combine(_path, "delete.level1/level2/level3/level4");
    if (Directory::exists(path2)) {
        return false;
    }
    if (Directory::deleteDirectory(path)) {
        return false;
    }

    return true;
}

bool testExists() {
    String path = Path::combine(_path, "exists.level1");
    if (!Directory::createDirectory(path)) {
        return false;
    }
    if (!Directory::exists(path)) {
        return false;
    }
    if (!Directory::deleteDirectory(path)) {
        return false;
    }
    if (Directory::exists(path)) {
        return false;
    }

    return true;
}

bool testGetFiles() {
    String path1 = Path::combine(_path, "gets.level1");
    String path2 = Path::combine(_path, "gets.level1/level2");
    String path3 = Path::combine(_path, "gets.level1/level2/level3");
    if (!Directory::createDirectory(path3)) {
        return false;
    }
    FileStream fs1(Path::combine(path1, "file1"), FileMode::FileCreate, FileAccess::FileWrite);
    fs1.close();
    FileStream fs2(Path::combine(path2, "file2"), FileMode::FileCreate, FileAccess::FileWrite);
    fs2.close();
    FileStream fs3(Path::combine(path3, "file3"), FileMode::FileCreate, FileAccess::FileWrite);
    fs3.close();

    StringArray files1;
    Directory::getFiles(path1, "*", SearchOption::TopDirectoryOnly, files1);
    if (!(files1.count() == 1 && Path::getFileName(files1[0]) == "file1")) {
        return false;
    }
    StringArray files1_2;
    Directory::getFiles(path1, "*", SearchOption::AllDirectories, files1_2);
    if (files1_2.count() != 3) {
        return false;
    }
    for (int i = 0; i < files1_2.count(); ++i) {
        String temp = Path::getFileName(files1_2[i]);
        bool contains = false;
        for (int j = 1; j <= 3; ++j) {
            if (temp == String::format("file%d", j)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            return false;
        }
    }

    StringArray files2;
    Directory::getFiles(path2, "*", SearchOption::TopDirectoryOnly, files2);
    if (!(files2.count() == 1 && Path::getFileName(files2[0]) == "file2")) {
        return false;
    }
    StringArray files2_2;
    Directory::getFiles(path2, "*", SearchOption::AllDirectories, files2_2);
    if (files2_2.count() != 2) {
        return false;
    }
    for (int i = 0; i < files2_2.count(); ++i) {
        String temp = Path::getFileName(files2_2[i]);
        bool contains = false;
        for (int j = 2; j <= 3; ++j) {
            if (temp == String::format("file%d", j)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            return false;
        }
    }

    StringArray files3;
    Directory::getFiles(path3, "*", SearchOption::AllDirectories, files3);
    if (!(files3.count() == 1 && Path::getFileName(files3[0]) == "file3")) {
        return false;
    }

    return true;
}

bool testGetDirectories() {
    String path0 = Path::combine(_path, "level0");
    String path1 = Path::combine(_path, "level0/level1");
    String path2 = Path::combine(_path, "level0/level1/level2");
    String path3 = Path::combine(_path, "level0/level1/level2/level3");
    Directory::deleteDirectory(path0);
    if (!Directory::createDirectory(path3)) {
        return false;
    }

    StringArray directories1;
    Directory::getDirectories(path0, "*", SearchOption::TopDirectoryOnly, directories1);
    if (!(directories1.count() == 1 && Path::getFileName(directories1[0]) == "level1")) {
        return false;
    }
    StringArray directories1_2;
    Directory::getDirectories(path0, "*", SearchOption::AllDirectories, directories1_2);
    if (directories1_2.count() != 3) {
        return false;
    }
    for (int i = 0; i < directories1_2.count(); ++i) {
        String temp = Path::getFileName(directories1_2[i]);
        bool contains = false;
        for (int j = 1; j <= 3; ++j) {
            if (temp == String::format("level%d", j)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            return false;
        }
    }

    StringArray directories2;
    Directory::getDirectories(path1, "*", SearchOption::TopDirectoryOnly, directories2);
    if (!(directories2.count() == 1 && Path::getFileName(directories2[0]) == "level2")) {
        return false;
    }
    StringArray directories2_2;
    Directory::getDirectories(path1, "*", SearchOption::AllDirectories, directories2_2);
    if (directories2_2.count() != 2) {
        return false;
    }
    for (int i = 0; i < directories2_2.count(); ++i) {
        String temp = Path::getFileName(directories2_2[i]);
        bool contains = false;
        for (int j = 2; j <= 3; ++j) {
            if (temp == String::format("level%d", j)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            return false;
        }
    }

    StringArray directories3;
    Directory::getDirectories(path2, "*", SearchOption::AllDirectories, directories3);
    if (!(directories3.count() == 1 && Path::getFileName(directories3[0]) == "level3")) {
        return false;
    }

    return true;
}

bool testCopy() {
    return true;
}

bool testRename() {
    String path = Path::combine(_path, "rename.level1");
    Directory::createDirectory(path);
    if (!Directory::exists(path)) {
        return false;
    }
    String path2 = Path::combine(_path, "rename.level1.renamed");
    if (!Directory::rename(path, path2)) {
        return false;
    }
    if (Directory::exists(path)) {
        return false;
    }
    if (!Directory::exists(path2)) {
        return false;
    }

    return true;
}

bool testGetPath() {
    String appPath = Directory::getAppPath();
    String startupPath = Application::startupPath();
    if (startupPath.find(appPath) < 0) {
        return false;
    }

    String homePath = Directory::getHomePath();
    if (homePath.isNullOrEmpty()) {
        return false;
    }

    String docPath = Directory::getDocumentPath();
    if (docPath.isNullOrEmpty()) {
        return false;
    }
    String docPath2 = Directory::getDocumentPath("test");
    if (docPath2.isNullOrEmpty()) {
        return false;
    }
    if (docPath2.find("test") <= 0) {
        return false;
    }

    String tempPath = Directory::getTempPath();
    if (tempPath.isNullOrEmpty()) {
        return false;
    }

    return true;
}

bool testCurrentDirectory() {
    String path = Directory::getCurrentDirectory();
    if (path.isNullOrEmpty()) {
        return false;
    }

    String newPath = Path::combine(_path, "current.level1");
    Directory::createDirectory(newPath);
    if (!Directory::setCurrentDirectory(newPath)) {
        return false;
    }
    String path2 = Directory::getCurrentDirectory();
    if (path2.find(newPath) < 0) {
        return false;
    }
    Directory::setCurrentDirectory(path);

    return true;
}

bool testSetFileModifyTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String path = Path::combine(_path, "time_directory");
    Directory::createDirectory(path);
    if(Directory::exists(path)) {
        if(!Directory::setModifyTime(path, time)) {
            return false;
        }
        DateTime time2;
        Directory::getModifyTime(path, time2);
        String str = time.toString();
        String str2 = time2.toString();
        result = time.toString() == time2.toString();
        Directory::deleteDirectory(path);
    }
    return result;
}

bool testSetLastAccessTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String path = Path::combine(_path, "time_directory");
    Directory::createDirectory(path);
    if(Directory::exists(path)) {
        if(!Directory::setLastAccessTime(path, time)) {
            return false;
        }
        DateTime time2;
        Directory::getLastAccessTime(path, time2);
        result = time.toString() == time2.toString();
        Directory::deleteDirectory(path);
    }
    return result;
}

bool testSetCreationTime() {
    bool result = false;
    DateTime time = DateTime::now();
    String path = Path::combine(_path, "time_directory");
    Directory::createDirectory(path);
    if(Directory::exists(path)) {
        if(!Directory::setCreationTime(path, time)) {
            return false;
        }
        DateTime time2;
        Directory::getCreationTime(path, time2);
        result = time.toString() == time2.toString();
        Directory::deleteDirectory(path);
    }
    return result;
}

int main() {
    int result = 0;

    setUp();

    if (!testCreateDirectory()) {
        result = 1;
    }

    if (!testDeleteDirectory()) {
        result = 2;
    }

    if (!testExists()) {
        result = 3;
    }

    if (!testGetFiles()) {
        result = 4;
    }

    if (!testGetDirectories()) {
        result = 5;
    }

    if (!testCopy()) {
        result = 6;
    }

    if (!testRename()) {
        result = 7;
    }

    if (!testGetPath()) {
        result = 8;
    }

    if (!testCurrentDirectory()) {
        result = 9;
    }

    if(!testSetFileModifyTime())
        result = 10;
    if(!testSetLastAccessTime())
        result = 11;
#ifdef WIN32
    if(!testSetCreationTime())
        result = 12;
#endif

    cleanUp();

    if(result) {
        Debug::writeFormatLine("result: %d", result);
    }

    return result;
}