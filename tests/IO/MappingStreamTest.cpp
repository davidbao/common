//
//  MappingStreamTest.cpp
//  common
//
//  Created by baowei on 2022/10/26.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "IO/MappingStream.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "exception/Exception.h"
#include "data/ValueType.h"

using namespace IO;
using namespace System;

static const String _path = Path::combine(Path::getTempPath(), "mapping_stream_test");
static const String _fileName = "test.bin";
static const String _fullFileName = Path::combine(_path, _fileName);
#ifdef PC_OS
static const int64_t _fileSize = 257L * 1024 * 1024 + 1;
#else
static const int64_t _fileSize = 1L * 1024 * 1024;
#endif // PC_OS

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
        MappingStream stream(_fullFileName, _fileSize);
        if (!stream.isOpen()) {
            return false;
        }
    }

    {
        MappingStream stream(_fullFileName);
        if (!stream.isOpen()) {
            return false;
        }
    }

#ifndef __EMSCRIPTEN__
    {
        try {
            MappingStream stream(String::Empty);
            return false;
        } catch (const ArgumentNullException& e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            MappingStream stream("abc");
            return false;
        } catch (const FileNotFoundException& e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            MappingStream stream(String::Empty, 0);
            return false;
        } catch (const ArgumentNullException& e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            MappingStream stream("abc", 0);
            return false;
        } catch (const ArgumentOutOfRangeException& e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }

    {
        try {
            MappingStream stream("abc", 2UL * 1024 * 1024 * 1024L + 1);
            return false;
        } catch (const ArgumentOutOfRangeException& e) {
            printf("exception: %s\n", e.message().c_str());
        }
    }
#endif  // __EMSCRIPTEN__

    return true;
}

bool testPositionAndLength() {
    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeText(expect);
        ms.close();

        MappingStream ms2(fileName);
        if (ms2.length() != _fileSize) {
            return false;
        }
        ms2.seek(1);

        String actual;
        if (!ms2.readToEnd(actual)) {
            return false;
        }
        ms2.close();

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
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeText(expect);
        ms.close();

        MappingStream ms2(fileName);
        String actual;
        if (!ms2.seek(2)) {
            return false;
        }
        if (!ms2.readToEnd(actual)) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "c") {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }

        if (ms2.seek(1 - _fileSize, SeekOrigin::SeekEnd) == -1) {
            return false;
        }
        if (!ms2.readToEnd(actual)) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "bc") {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }

        ms2.seek(0);
        ms2.seek(2, SeekOrigin::SeekCurrent);
        if (!ms2.readToEnd(actual)) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (actual != "c") {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }

        ms2.close();
        File::deleteFile(fileName);
    }

    return true;
}

bool testProperties() {
    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        ms.close();

        MappingStream ms2(fileName);
        if (ms2.isEnd()) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }
        String actual;
        if (!ms2.readToEnd(actual)) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }
        if (!ms2.isEnd()) {
            ms2.close();
            File::deleteFile(fileName);
            return false;
        }

        ms.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeText(expect);

        ms.setVersion("1.2");
        if (ms.version() != "1.2") {
            ms.close();
            File::deleteFile(fileName);
            return false;
        }

        ms.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeText(expect);

        Mutex *mutex = ms.mutex();
        if (mutex == nullptr) {
            return false;
        }

        ms.close();
        File::deleteFile(fileName);
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeText(expect);

        if (!ms.canWrite()) {
            ms.close();
            File::deleteFile(fileName);
            return false;
        }

        if (!ms.canRead()) {
            ms.close();
            File::deleteFile(fileName);
            return false;
        }

        if (!ms.canSeek()) {
            ms.close();
            File::deleteFile(fileName);
            return false;
        }

        ms.close();
        File::deleteFile(fileName);
    }

    return true;
}

bool testReadWrite() {
    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeStr(expect);
        ms.close();

        MappingStream ms2(fileName);
        String actual = ms2.readStr();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeStr(expect, String::StreamLength2);
        ms.close();

        MappingStream ms2(fileName);
        String actual = ms2.readStr(String::StreamLength2);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeStr(expect, String::StreamLength4);
        ms.close();

        MappingStream ms2(fileName);
        String actual = ms2.readStr(String::StreamLength4);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        String expect = "abc";
        ms.writeFixedLengthStr(expect, 3);
        ms.close();

        MappingStream ms2(fileName);
        String actual = ms2.readFixedLengthStr(3);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = true;
        ms.writeBoolean(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readBoolean();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 3.1415926f;
        ms.writeFloat(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readFloat();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 3.1415926f;
        ms.writeFloat(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readFloat(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 3.1415926;
        ms.writeDouble(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readDouble();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 3.1415926;
        ms.writeDouble(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readDouble(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeInt64(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt64();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeInt64(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt64(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeInt48(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt48();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeInt48(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt48(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeUInt64(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt64();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeUInt64(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt64(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeUInt48(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt48();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234L;
        ms.writeUInt48(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt48(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt32(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt32();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt32(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt32(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt24(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt24();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt24(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt24(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt16(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt16();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeInt16(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt16(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt16(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt16(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12;
        ms.writeInt8(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt8();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12;
        ms.writeUInt8(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt8();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12;
        ms.writeByte(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readByte();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12;
        ms.writeByte(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readByte();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeBCDInt32(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt32();
        ms2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt32(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt32();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt32(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt32(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt24(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt24();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt24(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt24(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt24(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt24(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt16(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16();
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeUInt16(expect, false);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16(false);
        ms2.close();

        if (actual != expect) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeBCDUInt32(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt32();
        ms2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeBCDUInt16(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16();
        ms2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 1234;
        ms.writeBCDUInt16(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readUInt16();
        ms2.close();

        if (actual != 0x1234) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12;
        ms.writeBCDByte(expect);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readByte();
        ms2.close();

        if (actual != 0x12) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = 12345678;
        ms.writeBCDValue(expect, 8);
        ms.close();

        MappingStream ms2(fileName);
        auto actual = ms2.readInt64();
        ms2.close();

        if (actual != 0x12345678) {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        auto expect = ByteArray{1, 2, 3, 4, 5, 6, 7, 8};
        ms.writeBytes(expect);
        ms.close();

        MappingStream ms2(fileName);
        ByteArray actual = ms2.readBytes(8);
        ms2.seek(0);
        ByteArray actual2 = ms2.readBytes(8, 3);
        ms2.close();

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
        MappingStream ms(fileName, _fileSize);
        auto expect = String("12345678中文Abc");
        ms.writeText(expect);
        ms.close();

        MappingStream ms2(fileName);
        String actual = ms2.readText((int) expect.length());
        ms2.seek(0);
        String actual2 = ms2.readText((int) expect.length(), 3);
        ms2.close();

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
        MappingStream ms(fileName, _fileSize);
        for (int i = 1; i <= 8; ++i) {
            ms.writeLine(Int32(i).toString());
        }
        ms.close();

        MappingStream ms2(fileName);
        String actual;
        for (int i = 0; i < 8; ++i) {
            actual.append(ms2.readLine());
        }
        ms2.close();

        if (actual != "12345678") {
            return false;
        }

        if (!File::deleteFile(fileName)) {
            return false;
        }
    }

    {
        String fileName = Path::combine(_path, "test.bin");
        MappingStream ms(fileName, _fileSize);
        ms.seek(256L * 1024 * 1024 - 1);
        for (int i = 1; i <= 8; ++i) {
            ms.writeLine(Int32(i).toString());
        }
        ms.close();

        MappingStream ms2(fileName);
        ms2.seek(256L * 1024 * 1024 - 1);
        String actual;
        for (int i = 0; i < 8; ++i) {
            actual.append(ms2.readLine());
        }
        ms2.close();

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