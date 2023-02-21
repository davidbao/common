//
//  Stream.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Stream_h
#define Stream_h

#include "data/String.h"
#include "data/ByteArray.h"
#include "data/Version.h"
#include "thread/Mutex.h"

using namespace Data;

namespace IO {
    enum SeekOrigin {
        // These constants match Win32's FILE_BEGIN, FILE_CURRENT, and FILE_END
        SeekBegin = SEEK_SET,
        SeekCurrent = SEEK_CUR,
        SeekEnd = SEEK_END,
    };

    class Stream {
    public:
        virtual ~Stream();

        virtual ssize_t write(const uint8_t *array, off_t offset, size_t count) = 0;

        virtual ssize_t read(uint8_t *array, off_t offset, size_t count) = 0;

        virtual off_t position() const = 0;

        virtual size_t length() const = 0;

        virtual off_t seek(off_t offset, SeekOrigin origin) = 0;

        virtual void flush() = 0;

        virtual void close() = 0;

        off_t seek(off_t offset);

        virtual bool canWrite() const;

        virtual bool canRead() const;

        virtual bool canSeek() const;

        bool isEnd() const;

        void writeStr(const String &str, String::StreamLength lengthCount = String::StreamLength1);

        String readStr(String::StreamLength lengthCount = String::StreamLength1);

        void writeFixedLengthStr(const String &str, int length);

        String readFixedLengthStr(int length);

        void writeBoolean(bool value);

        bool readBoolean();

        void writeFloat(float value, bool bigEndian = true);

        float readFloat(bool bigEndian = true);

        void writeDouble(double value, bool bigEndian = true);

        double readDouble(bool bigEndian = true);

        void writeInt64(int64_t value, bool bigEndian = true);

        int64_t readInt64(bool bigEndian = true);

        void writeInt48(int64_t value, bool bigEndian = true);

        int64_t readInt48(bool bigEndian = true);

        void writeUInt64(uint64_t value, bool bigEndian = true);

        uint64_t readUInt64(bool bigEndian = true);

        void writeUInt48(uint64_t value, bool bigEndian = true);

        uint64_t readUInt48(bool bigEndian = true);

        void writeInt32(int value, bool bigEndian = true);

        int readInt32(bool bigEndian = true);

        void writeInt24(int value, bool bigEndian = true);

        int readInt24(bool bigEndian = true);

        void writeInt16(int16_t value, bool bigEndian = true);

        int16_t readInt16(bool bigEndian = true);

        void writeUInt16(uint16_t value, bool bigEndian = true);

        uint16_t readUInt16(bool bigEndian = true);

        void writeInt8(int8_t value);

        int8_t readInt8();

        void writeUInt8(uint8_t value);

        uint8_t readUInt8();

        void writeByte(uint8_t value);

        uint8_t readByte();

        void writeBCDInt32(int32_t value);

        int32_t readBCDInt32();

        void writeUInt32(uint32_t value, bool bigEndian = true);

        uint32_t readUInt32(bool bigEndian = true);

        void writeUInt24(uint32_t value, bool bigEndian = true);

        uint32_t readUInt24(bool bigEndian = true);

        void writeBCDUInt32(uint32_t value);

        uint32_t readBCDUInt32();

        void writeBCDByte(uint8_t value);

        uint8_t readBCDByte();

        void writeBCDUInt16(uint16_t value);

        uint16_t readBCDUInt16();

        void writeBCDInt16(int16_t value);

        int16_t readBCDInt16();

        void writeBCDValue(int64_t value, int length);

        int64_t readBCDValue(int length);

        const Version &version() const;

        void setVersion(const Version &version);

        Mutex *mutex();

        bool writeBytes(const ByteArray &array);

        ByteArray readBytes(int count, int cacheCount = 1024);

        bool writeText(const String &text);

        String readText(int count, int cacheCount = 1024);

        bool readText(String &text, int maxLength = 64);

        bool readToEnd(ByteArray &array, int cacheCount = 1024);

        bool readToEnd(String &str, int cacheCount = 1024);

        String readLine(int cacheCount = 64);

        void writeLine(const String &str);

    public:
        static bool isLittleEndian();

        static bool isBigEndian();

    protected:
        Stream();

        Stream(const Stream &stream);

    private:
        Version _version;

        Mutex _mutex;
    };
}

#endif // Stream_h
