//
//  TcpServer.cpp
//  common
//
//  Created by baowei on 2023/1/6.
//  Copyright © 2023 com. All rights reserved.
//

#include "IO/Path.h"
#include "system/Environment.h"
#include "system/Application.h"

using namespace IO;
using namespace System;

bool testChangeExtension() {
    {
        String goodFileName = "C:\\myDir\\myFile.com.extension";
        if (Path::changeExtension(goodFileName, ".old") != "C:\\myDir\\myFile.com.old") {
            return false;
        }
    }

    {
        String goodFileName = "C:\\myDir\\myFile.com.extension";
        if (Path::changeExtension(goodFileName, "") != "C:\\myDir\\myFile.com") {
            return false;
        }
    }

    {
        String badFileName = "C:\\myDir\\";
        if (Path::changeExtension(badFileName, ".old") != "C:\\myDir\\.old") {
            return false;
        }
    }

    {
        String goodFileName = "/usr/local/myDir/myFile.com.extension";
        if (Path::changeExtension(goodFileName, ".old") != "/usr/local/myDir/myFile.com.old") {
            return false;
        }
    }

    {
        String goodFileName = "/usr/local/myDir/myFile.com.extension";
        if (Path::changeExtension(goodFileName, "") != "/usr/local/myDir/myFile.com") {
            return false;
        }
    }

    {
        String badFileName = "/usr/local/myDir/";
        if (Path::changeExtension(badFileName, ".old") != "/usr/local/myDir/.old") {
            return false;
        }
    }

    return true;
}

bool testCombine() {
#ifdef WIN32
    {
        String path1 = "C:\\myDir";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "C:\\myDir\\myFile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "C:\\myDir\\";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "C:\\myDir\\myFile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "myDir";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "myDir\\myFile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "C:\\myDir\\";
        String path2 = "myDir2";
        String path3 = "myFile.com.extension";
        if (Path::combine(path1, path2, path3) != "C:\\myDir\\myDir2\\myFile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "C:\\myDir\\";
        String path2 = "myDir2";
        String path3 = "myDir3";
        String path4 = "myFile.com.extension";
        if (Path::combine(path1, path2, path3, path4) != "C:\\myDir\\myDir2\\myDir3\\myFile.com.extension") {
            return false;
        }
    }

    {
        StringArray paths = {"C:\\myDir\\", "myDir2", "myDir3", "myFile.com.extension"};
        if (Path::combine(paths) != "C:\\myDir\\myDir2\\myDir3\\myFile.com.extension") {
            return false;
        }
    }
#else
    {
        String path1 = "/usr/local/myDir";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "/usr/local/myDir/myFile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "/usr/local/myDir/";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "/usr/local/myDir/myFile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "myDir";
        String path2 = "myFile.com.extension";
        if (Path::combine(path1, path2) != "myDir/myFile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "/usr/local/myDir";
        String path2 = "myDir2";
        String path3 = "myFile.com.extension";
        if (Path::combine(path1, path2, path3) != "/usr/local/myDir/myDir2/myFile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "/usr/local/myDir";
        String path2 = "myDir2";
        String path3 = "myDir3";
        String path4 = "myFile.com.extension";
        if (Path::combine(path1, path2, path3, path4) != "/usr/local/myDir/myDir2/myDir3/myFile.com.extension") {
            return false;
        }
    }

    {
        StringArray paths = {"/usr/local/myDir", "myDir2", "myDir3", "myFile.com.extension"};
        if (Path::combine(paths) != "/usr/local/myDir/myDir2/myDir3/myFile.com.extension") {
            return false;
        }
    }
#endif

    return true;
}

bool testEndsInDirectorySeparator() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\";
        if (!Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "C:\\myDir";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/";
        if (!Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "/usr/local/myDir";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }

#endif

    return true;
}

bool testGetDirectoryName() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\myDir2\\";
        if (Path::getDirectoryName(path) != "C:\\myDir") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myDir2";
        if (Path::getDirectoryName(path) != "C:\\myDir") {
            return false;
        }
    }

    {
        String path = "C:\\myDir";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:\\";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "abc";
        if (Path::getDirectoryName(path) != "abc") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/";
        if (Path::getDirectoryName(path) != "/usr/local") {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir";
        if (Path::getDirectoryName(path) != "/usr/local") {
            return false;
        }
    }

    {
        String path = "/usr";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "/usr/";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "/";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "abc";
        if (Path::getDirectoryName(path) != "abc") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetExtension() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\myFile.com.extension";
        if (Path::getExtension(path) != ".extension") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myDir\\";
        if (Path::getExtension(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/myFile.com.extension";
        if (Path::getExtension(path) != ".extension") {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir/";
        if (Path::getExtension(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetFileName() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\myFile.com.extension";
        if (Path::getFileName(path) != "myFile.com.extension") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myDir\\";
        if (Path::getFileName(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/myFile.com.extension";
        if (Path::getFileName(path) != "myFile.com.extension") {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir/";
        if (Path::getFileName(path) != "") {
            return false;
        }
    }
#endif

    {
        String path = "myFile.com.extension";
        if (Path::getFileName(path) != path) {
            return false;
        }
    }

    return true;
}

bool testGetFileNameWithoutExtension() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\myFile.com.extension";
        if (Path::getFileNameWithoutExtension(path) != "myFile.com") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myDir\\";
        if (Path::getFileNameWithoutExtension(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/myFile.com.extension";
        if (Path::getFileNameWithoutExtension(path) != "myFile.com") {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir/";
        if (Path::getFileNameWithoutExtension(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetFullPath() {
    {
        String path = "myDir";
        String fullPath = Path::getFullPath(path);
        String current = Environment::getCurrentDirectory();
        if (fullPath != Path::combine(current, path)) {
            return false;
        }
    }

    {
        String path = "myDir";
        String basePath = "/usr/local/myDir";
        String fullPath = Path::getFullPath(path, basePath);
        if (fullPath != Path::combine(basePath, path)) {
            return false;
        }
    }

    return true;
}

bool testChars() {
    {
        const Array<char, 41> &chars = Path::getInvalidFileNameChars();
        if (!(chars[0] == '\"' && chars[chars.count() - 1] == '/')) {
            return false;
        }
    }

    {
        const Array<char, 36> &chars = Path::getInvalidPathChars();
        if (!(chars[0] == '\"' && chars[chars.count() - 1] == 31)) {
            return false;
        }
    }

    return true;
}

bool testGetPathRoot() {
#ifdef WIN32
    {
        String path = "\\myDir\\";
        if (Path::getPathRoot(path) != "") {
            return false;
        }
    }

    {
        String path = "myFile.ext";
        if (Path::getPathRoot(path) != "") {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myFile.ext";
        if (Path::getPathRoot(path) != "C:\\") {
            return false;
        }
    }

    {
        String path = "\\\\myPC\\myDir\\myFile.ext";
        if (Path::getPathRoot(path) != "\\\\myPC\\") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir";
        if (Path::getPathRoot(path) != "/") {
            return false;
        }
    }

    {
        String path = "myFile.ext";
        if (Path::getPathRoot(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetRandomFileName() {
    {
        String fileName = Path::getRandomFileName();
        if (fileName.length() != 12) {
            return false;
        }

        ssize_t pos = fileName.find('.');
        if (pos != 8) {
            return false;
        }
    }

    return true;
}

bool testGetTempFileName() {
    {
        String tempName = Path::getTempFileName();
        if (tempName.isNullOrEmpty()) {
            return false;
        }
    }
    return true;
}

bool testGetPath() {
    {
        String appPath = Path::getAppPath();
        String startupPath = Application::startupPath();
        if (startupPath.find(appPath) < 0) {
            return false;
        }
    }

    {
        String homePath = Path::getHomePath();
        if (homePath.isNullOrEmpty()) {
            return false;
        }
    }

    {
        String docPath = Path::getDocumentPath();
        if (docPath.isNullOrEmpty()) {
            return false;
        }
    }

    {
        String docPath2 = Path::getDocumentPath("test");
        if (docPath2.isNullOrEmpty()) {
            return false;
        }
        if (docPath2.find("test") <= 0) {
            return false;
        }
    }

    {
        String tempPath = Path::getTempPath();
        if (tempPath.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testHasExtension() {
#ifdef WIN32
    {
        String path = "C:\\myDir\\myFile.ext";
        if (!Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myFile.ext\\";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "C:\\myDir\\myFile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "myDir\\myFile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir/myFile.ext";
        if (!Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir/myFile.ext/";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "/usr/local/myDir/myFile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "myDir/myFile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }
#endif

    return true;
}

bool testIsPathRooted() {
#ifdef WIN32
    {
        String path = "C:\\myDir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "\\myPC\\myDir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "myDir";
        if (Path::isPathRooted(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/myDir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "myDir";
        if (Path::isPathRooted(path)) {
            return false;
        }
    }
#endif

    return true;
}

int main() {
    if (!testChangeExtension()) {
        return 1;
    }
    if (!testCombine()) {
        return 2;
    }
    if (!testEndsInDirectorySeparator()) {
        return 3;
    }
    if (!testGetDirectoryName()) {
        return 4;
    }
    if (!testGetExtension()) {
        return 5;
    }
    if (!testGetFileName()) {
        return 6;
    }
    if (!testGetFileNameWithoutExtension()) {
        return 7;
    }
    if (!testGetFullPath()) {
        return 8;
    }
    if (!testChars()) {
        return 9;
    }
    if (!testGetPathRoot()) {
        return 10;
    }
    if (!testGetRandomFileName()) {
        return 11;
    }
    if (!testGetTempFileName()) {
        return 12;
    }
    if (!testGetPath()) {
        return 13;
    }
    if (!testHasExtension()) {
        return 14;
    }
    if (!testIsPathRooted()) {
        return 15;
    }

    return 0;
}