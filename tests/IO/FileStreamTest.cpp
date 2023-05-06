//  FileStreamTest.cpp
//  common
//
//  Created by baowei on 2023/1/19.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "data/ValueType.h"
#include "exception/Exception.h"

using namespace IO;
using namespace System;

static const String _path = Path::combine(Path::getTempPath(), "filestream_test");

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
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileAppend, FileAccess::FileWrite);
        fs2.writeText("def");
        fs2.close();

        FileStream fs3(fileName, FileMode::FileOpenOrCreate, FileAccess::FileRead);
        String actual;
        if (!fs3.readToEnd(actual)) {
            return false;
        }
        fs3.close();

        if (actual != "abcdef") {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileTruncate, FileAccess::FileWrite);
        fs2.writeText("def");
        fs2.close();

        FileStream fs3(fileName, FileMode::FileOpenOrCreate, FileAccess::FileRead);
        String actual;
        if (!fs3.readToEnd(actual)) {
            return false;
        }
        fs3.close();

        if (actual != "def") {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

#ifndef __EMSCRIPTEN__
    {
        try {
            FileStream fs("", FileMode::FileOpen, FileAccess::FileRead);
            return false;
        } catch (const ArgumentNullException &e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            String fileName = Path::combine(_path, "test.bin");
            FileStream fs(fileName, FileMode::FileOpen, FileAccess::FileRead);
            return false;
        } catch (const FileNotFoundException &e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            String path = Path::combine(_path, "test");
            String fileName = Path::combine(path, "test.bin");
            FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileRead);
            return false;
        } catch (const PathNotFoundException &e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

//    {
//        String fileName = Path::combine(_path, "test.bin");
//        {
//            FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileRead);
//            fs.close();
//        }
//
//        try {
//            FileStream fs(fileName, FileMode::FileCreateNew, FileAccess::FileReadWrite);
//            return false;
//        } catch (const IOException &e) {
//            printf("exception: %s\n", e.message().c_str());
//        }
//
//        if (!File::deleteFile(fileName)) {
//            return false;
//        }
//    }

    {
        try {
            String fileName = Path::combine(_path, "test.bin");
            FileStream fs(fileName, FileMode::FileTruncate, FileAccess::FileRead);
            return false;
        } catch (const FileNotFoundException &e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }
#endif

    return true;
}

bool testPositionAndLength() {
    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileRead);
        if (fs2.length() != 3) {
            return false;
        }
        fs2.seek(1);

        String actual;
        if (!fs2.readToEnd(actual)) {
            return false;
        }
        fs2.close();

        if (actual != "bc") {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    return true;
}

bool testSeek() {
    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileRead);
        String actual;
        if (!fs2.seek(2)) {
            return false;
        }
        if (!fs2.readToEnd(actual)) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "c") {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }

        if (fs2.seek(-2, SeekOrigin::SeekEnd) == -1) {
            return false;
        }
        if (!fs2.readToEnd(actual)) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "bc") {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }

        fs2.seek(0);
        fs2.seek(2, SeekOrigin::SeekCurrent);
        if (!fs2.readToEnd(actual)) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "c") {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }

        fs2.close();
        File::deleteFile(fileName);
    }

    return true;
}

bool testProperties() {
    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);
        if (fs.fileName() != fileName) {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }

        if (fs.fd() <= 0) {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileRead);
        if (fs2.isEnd()) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }
        String actual;
        if (!fs2.readToEnd(actual)) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (!fs2.isEnd()) {
            fs2.close();
            File::deleteFile(fileName);
            return false;
        }

        fs.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);

        fs.setVersion("1.2");
        if (fs.version() != "1.2") {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }

        fs.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);

        Mutex *mutex = fs.mutex();
        if (mutex == nullptr) {
            return false;
        }

        fs.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileWrite);
        String expect = "abc";
        fs.writeText(expect);

        if (!fs.canWrite()) {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }

        if (fs.canRead()) {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }

        if (!fs.canSeek()) {
            fs.close();
            File::deleteFile(fileName);
            return false;
        }

        fs.close();
        File::deleteFile(fileName);
    }

    return true;
}

bool testReadWrite() {
    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        String expect = "abc";
        fs.writeStr(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual = fs2.readStr();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        String expect = "abc";
        fs.writeStr(expect, String::StreamLength2);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual = fs2.readStr(String::StreamLength2);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        String expect = "abc";
        fs.writeStr(expect, String::StreamLength4);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual = fs2.readStr(String::StreamLength4);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        String expect = "abc";
        fs.writeFixedLengthStr(expect, 3);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual = fs2.readFixedLengthStr(3);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = true;
        fs.writeBoolean(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readBoolean();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 3.1415926f;
        fs.writeFloat(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readFloat();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 3.1415926f;
        fs.writeFloat(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readFloat(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 3.1415926;
        fs.writeDouble(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readDouble();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 3.1415926;
        fs.writeDouble(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readDouble(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeInt64(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt64();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeInt64(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt64(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeInt48(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt48();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeInt48(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt48(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeUInt64(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt64();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeUInt64(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt64(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeUInt48(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt48();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234L;
        fs.writeUInt48(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt48(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt32(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt32();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt32(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt32(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt24(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt24();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt24(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt24(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt16(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt16();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeInt16(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt16(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt16(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt16(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12;
        fs.writeInt8(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt8();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12;
        fs.writeUInt8(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt8();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12;
        fs.writeByte(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readByte();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12;
        fs.writeByte(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readByte();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeBCDInt32(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt32();
        fs2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt32(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt32();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt32(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt32(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt24(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt24();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt24(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt24(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt24(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt24(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt16(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16();
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeUInt16(expect, false);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16(false);
        fs2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeBCDUInt32(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt32();
        fs2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeBCDUInt16(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16();
        fs2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 1234;
        fs.writeBCDUInt16(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readUInt16();
        fs2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12;
        fs.writeBCDByte(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readByte();
        fs2.close();

        if (actual != 0x12) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = 12345678;
        fs.writeBCDValue(expect, 8);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        auto actual = fs2.readInt64();
        fs2.close();

        if (actual != 0x12345678) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = ByteArray{1, 2, 3, 4, 5, 6, 7, 8};
        fs.writeBytes(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        ByteArray actual = fs2.readBytes(8);
        fs2.seek(0);
        ByteArray actual2 = fs2.readBytes(8, 3);
        fs2.close();

        if (actual != expect) {
            return false;
        }
        if (actual2 != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        auto expect = String("12345678中文Abc");
        fs.writeText(expect);
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual = fs2.readText((int) expect.length());
        fs2.seek(0);
        String actual2 = fs2.readText((int) expect.length(), 3);
        fs2.close();

        if (actual != expect) {
            return false;
        }
        if (actual2 != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        FileStream fs(fileName, FileMode::FileOpenOrCreate, FileAccess::FileReadWrite);
        for (int i = 1; i <= 8; ++i) {
            fs.writeLine(Int32(i).toString());
        }
        fs.close();

        FileStream fs2(fileName, FileMode::FileOpen, FileAccess::FileReadWrite);
        String actual;
        for (int i = 0; i < 8; ++i) {
            actual.append(fs2.readLine());
        }
        fs2.close();

        if (actual != "12345678") {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    return true;
}

int main() {
    setUp();

    if (!testConstructor()) {
        cleanUp();
        return 1;
    }

    if (!testPositionAndLength()) {
        cleanUp();
        return 2;
    }

    if (!testSeek()) {
        cleanUp();
        return 3;
    }

    if (!testProperties()) {
        cleanUp();
        return 4;
    }

    if (!testReadWrite()) {
        cleanUp();
        return 5;
    }

    cleanUp();

    return 0;
}