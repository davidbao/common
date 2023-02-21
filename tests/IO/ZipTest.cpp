//
//  Zip.cpp
//  common
//
//  Created by baowei on 2023/1/23.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/Zip.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"

using namespace IO;

static const String _path = Path::combine(Path::getTempPath(), "zip_test");

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

bool testConstructor() {
    {
        Zip zip("");
        if (zip.isValid()) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();
        Zip zip(fileName);
        if (zip.isValid()) {
            File::deleteFile(fileName);
            return false;
        }
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compressFile(fileName, zipFile)) {
            File::deleteFile(fileName);
            return false;
        }

        Zip zip(zipFile);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        String actual;
        zip.read("test.bin", actual);
        zip.close();

        File::deleteFile(zipFile);
        File::deleteFile(fileName);

        if (actual != "abc") {
            return false;
        }
    }

    {
        Zip zip(ByteArray::Empty);
        if (zip.isValid()) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compressFile(fileName, zipFile)) {
            File::deleteFile(fileName);
            return false;
        }

        FileStream fs2(zipFile, FileMode::FileOpen, FileAccess::FileRead);
        ByteArray buffer;
        fs2.readToEnd(buffer);
        fs2.close();

        Zip zip(buffer);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        String actual;
        zip.read("test.bin", actual);
        zip.close();

        File::deleteFile(zipFile);
        File::deleteFile(fileName);

        if (actual != "abc") {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeBytes(ByteArray{1, 2, 3, 4});
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compressFile(fileName, zipFile)) {
            File::deleteFile(fileName);
            return false;
        }

        FileStream fs2(zipFile, FileMode::FileOpen, FileAccess::FileRead);
        ByteArray buffer;
        fs2.readToEnd(buffer);
        fs2.close();

        Zip zip(buffer);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        ByteArray actual;
        zip.read("test.bin", actual);
        zip.close();

        File::deleteFile(zipFile);
        File::deleteFile(fileName);

        if (actual != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    return true;
}

bool testStaticMethod() {
    {
        // Without password.
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compressFile(fileName, zipFile)) {
            File::deleteFile(fileName);
            return false;
        }

        Zip zip(zipFile);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        zip.close();

        String path = Path::combine(_path, "zip_extract");
        if (!Zip::extract(zipFile, path)) {
            return false;
        }
        String actualName = Path::combine(path, "test.bin");
        FileStream fs2(actualName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != "abc") {
            return false;
        }

        Directory::deleteDirectory(path);
        File::deleteFile(fileName);
        File::deleteFile(zipFile);
    }

    {
        // With password.
        String password = "123.com";
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compressFile(fileName, zipFile, password)) {
            File::deleteFile(fileName);
            return false;
        }

        Zip zip(zipFile);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        zip.close();

        String path = Path::combine(_path, "zip_extract");
        if (!Zip::extract(zipFile, path, password)) {
            return false;
        }
        String actualName = Path::combine(path, "test.bin");
        FileStream fs2(actualName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != "abc") {
            return false;
        }

        Directory::deleteDirectory(path);
        File::deleteFile(fileName);
        File::deleteFile(zipFile);
    }

    {
        // With password.
        String password = "123.com";
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        StringArray fileNames = {fileName};
        if (!Zip::compressFile(fileNames, zipFile, password)) {
            File::deleteFile(fileName);
            return false;
        }

        Zip zip(zipFile);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        zip.close();

        String path = Path::combine(_path, "zip_extract");
        if (!Zip::extract(zipFile, path, password)) {
            return false;
        }
        String actualName = Path::combine(path, "test.bin");
        FileStream fs2(actualName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != "abc") {
            return false;
        }

        Directory::deleteDirectory(path);
        File::deleteFile(fileName);
        File::deleteFile(zipFile);
    }

    {
        // With password.
        String password = "123.com";
        String cPath = Path::combine(_path, "zip_compress");
        if (!Directory::exists(cPath)) {
            Directory::createDirectory(cPath);
        }
        String fileName = Path::combine(cPath, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        fs.writeText("abc");
        fs.close();

        String zipFile = Path::combine(_path, "test.bin.zip");
        if (!Zip::compress(cPath, zipFile, password)) {
            File::deleteFile(fileName);
            return false;
        }

        Zip zip(zipFile);
        if (!zip.exists("test.bin")) {
            File::deleteFile(fileName);
            return false;
        }
        zip.close();

        String ePath = Path::combine(_path, "zip_extract");
        if (!Zip::extract(zipFile, ePath, password)) {
            return false;
        }
        String actualName = Path::combine(ePath, "test.bin");
        FileStream fs2(actualName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != "abc") {
            return false;
        }

        Directory::deleteDirectory(ePath);
        File::deleteFile(fileName);
        File::deleteFile(zipFile);
    }

    return true;
}

int main() {
    setUp();

    if (!testConstructor()) {
        cleanUp();
        return 1;
    }

    if (!testStaticMethod()) {
        cleanUp();
        return 2;
    }

    cleanUp();

    return 0;
}