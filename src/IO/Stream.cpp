//
//  Stream.cpp
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#include "IO/Stream.h"
#include "system/BCDUtilities.h"
#include "data/ByteArray.h"
#include "system/Math.h"

namespace Common {
    Stream::Stream() {
    }

    Stream::~Stream() {
    }

    bool Stream::isEnd() const {
        return position() >= length();
    }

    void Stream::writeStr(const String &str, String::StreamLength lengthCount) {
#ifdef DEBUG
        size_t length = str.length();
        if (lengthCount == String::StreamLength2 && length > 0xFFFF) {
            assert(false);
        } else if (lengthCount == String::StreamLength1 && length > 0xFF) {
            assert(false);
        }

#endif //DEBUG
        size_t len = 0;
        if (lengthCount == String::StreamLength2) {
            len = str.length();
            writeUInt16((uint16_t) len);
        } else if (lengthCount == String::StreamLength1) {
            len = str.length();
            writeByte((uint8_t) len);
        } else if (lengthCount == String::StreamLength4) {
            len = str.length();
            writeUInt32((uint32_t) len);
        } else {
            assert(false);
        }

        write((uint8_t *) str.c_str(), 0, len);
    }

    String Stream::readStr(String::StreamLength lengthCount) {
        uint32_t len = 0;
        if (lengthCount == String::StreamLength2) {
            len = readUInt16();
        } else if (lengthCount == String::StreamLength1) {
            len = readByte();
        } else if (lengthCount == String::StreamLength4) {
            len = readUInt32();
        } else {
            assert(false);
        }

        return readFixedLengthStr(len);
    }

    void Stream::writeFixedLengthStr(const String &str, int length) {
        int len = (int) str.length();
        if (len < length) {
            write((uint8_t *) str.c_str(), 0, len);
            int count = length - len;
            uint8_t *buffer = new uint8_t[count];
            memset(buffer, 0, count);
            write(buffer, 0, count);
            delete[] buffer;
        } else {
            write((uint8_t *) str.c_str(), 0, length);
        }
    }

    String Stream::readFixedLengthStr(int length) {
        char *str = new char[length + 1];
        memset(str, 0, length + 1);
        read((uint8_t *) str, 0, length);
        String result = str;
        delete[] str;
        return result;
    }

    void Stream::writeBoolean(bool value) {
        writeByte((uint8_t) value);
    }

    bool Stream::readBoolean() {
        return readByte() > 0;
    }

    void Stream::writeFloat(float value, bool bigEndian) {
        static const int count = sizeof(float);
        uint8_t buffer[count];
        memcpy(buffer, &value, count);
        if ((isBigEndian() && !bigEndian) ||
            (isLittleEndian() && bigEndian)) {
            uint8_t temp[count];
            for (int i = 0; i < count; i++) {
                temp[i] = buffer[count - 1 - i];
            }
            memcpy(buffer, temp, count);
        }
        write(buffer, 0, count);
    }

    float Stream::readFloat(bool bigEndian) {
        static const int count = sizeof(float);
        uint8_t buffer[count];
        read(buffer, 0, count);
        if ((isBigEndian() && !bigEndian) ||
            (isLittleEndian() && bigEndian)) {
            uint8_t temp[count];
            for (int i = 0; i < count; i++) {
                temp[i] = buffer[count - 1 - i];
            }
            memcpy(buffer, temp, count);
        }
        float result;
        memcpy(&result, buffer, count);
        return result;
    }

    void Stream::writeDouble(double value, bool bigEndian) {
        static const int count = sizeof(double);
        uint8_t buffer[count];
        memcpy(buffer, &value, count);
        if ((isBigEndian() && !bigEndian) ||
            (isLittleEndian() && bigEndian)) {
            uint8_t temp[count];
            for (int i = 0; i < count; i++) {
                temp[i] = buffer[count - 1 - i];
            }
            memcpy(buffer, temp, count);
        }
        write(buffer, 0, count);
    }

    double Stream::readDouble(bool bigEndian) {
        static const int count = sizeof(double);
        uint8_t buffer[count];
        read(buffer, 0, count);
        if ((isBigEndian() && !bigEndian) ||
            (isLittleEndian() && bigEndian)) {
            uint8_t temp[count];
            for (int i = 0; i < count; i++) {
                temp[i] = buffer[count - 1 - i];
            }
            memcpy(buffer, temp, count);
        }
        double result;
        memcpy(&result, buffer, count);
        return result;
    }

    void Stream::writeInt64(int64_t value, bool bigEndian) {
        uint8_t buffer[8];
        if (bigEndian) {
            buffer[0] = (uint8_t) ((value & 0xFF00000000000000) >> 56);
            buffer[1] = (uint8_t) ((value & 0x00FF000000000000) >> 48);
            buffer[2] = (uint8_t) ((value & 0x0000FF0000000000) >> 40);
            buffer[3] = (uint8_t) ((value & 0x000000FF00000000) >> 32);
            buffer[4] = (uint8_t) ((value & 0x00000000FF000000) >> 24);
            buffer[5] = (uint8_t) ((value & 0x0000000000FF0000) >> 16);
            buffer[6] = (uint8_t) ((value & 0x000000000000FF00) >> 8);
            buffer[7] = (uint8_t) ((value & 0x00000000000000FF) >> 0);
        } else {
            buffer[7] = (uint8_t) ((value & 0xFF00000000000000) >> 56);
            buffer[6] = (uint8_t) ((value & 0x00FF000000000000) >> 48);
            buffer[5] = (uint8_t) ((value & 0x0000FF0000000000) >> 40);
            buffer[4] = (uint8_t) ((value & 0x000000FF00000000) >> 32);
            buffer[3] = (uint8_t) ((value & 0x00000000FF000000) >> 24);
            buffer[2] = (uint8_t) ((value & 0x0000000000FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x000000000000FF00) >> 8);
            buffer[0] = (uint8_t) ((value & 0x00000000000000FF) >> 0);
        }
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeInt48(int64_t value, bool bigEndian) {
        uint8_t buffer[6];
        if (bigEndian) {
            buffer[0] = (uint8_t) ((value & 0x0000FF0000000000) >> 40);
            buffer[1] = (uint8_t) ((value & 0x000000FF00000000) >> 32);
            buffer[2] = (uint8_t) ((value & 0x00000000FF000000) >> 24);
            buffer[3] = (uint8_t) ((value & 0x0000000000FF0000) >> 16);
            buffer[4] = (uint8_t) ((value & 0x000000000000FF00) >> 8);
            buffer[5] = (uint8_t) ((value & 0x00000000000000FF) >> 0);
        } else {
            buffer[5] = (uint8_t) ((value & 0x0000FF0000000000) >> 40);
            buffer[4] = (uint8_t) ((value & 0x000000FF00000000) >> 32);
            buffer[3] = (uint8_t) ((value & 0x00000000FF000000) >> 24);
            buffer[2] = (uint8_t) ((value & 0x0000000000FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x000000000000FF00) >> 8);
            buffer[0] = (uint8_t) ((value & 0x00000000000000FF) >> 0);
        }
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeUInt64(uint64_t value, bool bigEndian) {
        writeInt64((int64_t) value, bigEndian);
    }

    void Stream::writeUInt48(uint64_t value, bool bigEndian) {
        writeInt48((int64_t) value, bigEndian);
    }

    void Stream::writeInt32(int value, bool bigEndian) {
        uint8_t buffer[4];
        if (bigEndian) {
            buffer[0] = (uint8_t) ((value & 0xFF000000) >> 24);
            buffer[1] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[2] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[3] = (uint8_t) ((value & 0x000000FF) >> 0);
        } else {
            buffer[3] = (uint8_t) ((value & 0xFF000000) >> 24);
            buffer[2] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[0] = (uint8_t) ((value & 0x000000FF) >> 0);
        }
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeInt24(int value, bool bigEndian) {
        uint8_t buffer[3];
        if (bigEndian) {
            buffer[0] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[2] = (uint8_t) ((value & 0x000000FF) >> 0);
        } else {
            buffer[2] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[0] = (uint8_t) ((value & 0x000000FF) >> 0);
        }
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeInt16(int16_t value, bool bigEndian) {
        uint8_t buffer[2];
        if (bigEndian) {
            buffer[0] = ((value >> 8) & 0xFF);
            buffer[1] = ((value) & 0xFF);
        } else {
            buffer[1] = ((value >> 8) & 0xFF);
            buffer[0] = ((value) & 0xFF);
        }
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeUInt16(uint16_t value, bool bigEndian) {
        writeInt16(value, bigEndian);
    }

    void Stream::writeByte(uint8_t value) {
        writeUInt8(value);
    }

    void Stream::writeUInt8(uint8_t value) {
        uint8_t buffer[1];
        buffer[0] = value;
        write(buffer, 0, sizeof(buffer));
    }

    void Stream::writeInt8(int8_t value) {
        uint8_t buffer[1];
        buffer[0] = value;
        write(buffer, 0, sizeof(buffer));
    }

    int64_t Stream::readInt64(bool bigEndian) {
        int64_t value = 0;
        uint8_t buffer[8];
        memset(buffer, 0, sizeof(buffer));
        read(buffer, 0, sizeof(buffer));
        if (bigEndian) {
            for (size_t i = 0; i < sizeof(buffer); i++) {
                value <<= 8;
                value |= buffer[i];
            }

        } else {
            for (int i = sizeof(buffer) - 1; i >= 0; i--) {
                value <<= 8;
                value |= buffer[i];
            }
        }
        return value;
    }

    int64_t Stream::readInt48(bool bigEndian) {
        int64_t value = 0;
        uint8_t buffer[6];
        memset(buffer, 0, sizeof(buffer));
        read(buffer, 0, sizeof(buffer));
        if (bigEndian) {
            for (size_t i = 0; i < sizeof(buffer); i++) {
                value <<= 8;
                value |= buffer[i];
            }

        } else {
            for (int i = sizeof(buffer) - 1; i >= 0; i--) {
                value <<= 8;
                value |= buffer[i];
            }
        }
        return value;
    }

    uint64_t Stream::readUInt64(bool bigEndian) {
        return (uint64_t) readInt64(bigEndian);
    }

    uint64_t Stream::readUInt48(bool bigEndian) {
        return (uint64_t) readInt48(bigEndian);
    }

    int Stream::readInt32(bool bigEndian) {
        int value = 0;
        uint8_t buffer[4];
        memset(buffer, 0, sizeof(buffer));
        read(buffer, 0, sizeof(buffer));
        if (bigEndian) {
            for (size_t i = 0; i < sizeof(buffer); i++) {
                value <<= 8;
                value |= buffer[i];
            }

        } else {
            for (int i = sizeof(buffer) - 1; i >= 0; i--) {
                value <<= 8;
                value |= buffer[i];
            }
        }
        return value;
    }

    int Stream::readInt24(bool bigEndian) {
        int value = 0;
        uint8_t buffer[3];
        memset(buffer, 0, sizeof(buffer));
        read(buffer, 0, sizeof(buffer));
        if (bigEndian) {
            for (size_t i = 0; i < sizeof(buffer); i++) {
                value <<= 8;
                value |= buffer[i];
            }

        } else {
            for (int i = sizeof(buffer) - 1; i >= 0; i--) {
                value <<= 8;
                value |= buffer[i];
            }
        }
        return value;
    }

    int16_t Stream::readInt16(bool bigEndian) {
        uint8_t buffer[2];
        read(buffer, 0, sizeof(buffer));
        int16_t value = 0;
        value = bigEndian ? ((buffer[0] << 8) & 0xFF00) + buffer[1] : ((buffer[1] << 8) & 0xFF00) + buffer[0];
        return value;
    }

    uint16_t Stream::readUInt16(bool bigEndian) {
        return (uint16_t) readInt16(bigEndian);
    }

    uint8_t Stream::readByte() {
        return readUInt8();
    }

    uint8_t Stream::readUInt8() {
        uint8_t buffer[1];
        read(buffer, 0, sizeof(buffer));
        return buffer[0];
    }

    int8_t Stream::readInt8() {
        uint8_t buffer[1];
        read(buffer, 0, sizeof(buffer));
        return buffer[0];
    }

    void Stream::writeBCDInt32(int value) {
        writeBCDValue(value, 4);
    }

    int32_t Stream::readBCDInt32() {
        return (int32_t) readBCDValue(4);
    }

    uint32_t Stream::readUInt32(bool bigEndian) {
        return (uint32_t) readInt32(bigEndian);
    }

    void Stream::writeUInt32(uint32_t value, bool bigEndian) {
        writeInt32((int32_t) value, bigEndian);
    }

    uint32_t Stream::readUInt24(bool bigEndian) {
        return (uint32_t) readInt24(bigEndian);
    }

    void Stream::writeUInt24(uint32_t value, bool bigEndian) {
        writeInt24((int32_t) value, bigEndian);
    }

    void Stream::writeBCDUInt32(uint32_t value) {
        writeBCDValue(value, 4);
    }

    uint32_t Stream::readBCDUInt32() {
        return (uint32_t) readBCDValue(4);
    }

    void Stream::writeBCDByte(uint8_t value) {
        writeBCDValue(value, 1);
    }

    uint8_t Stream::readBCDByte() {
        return (uint8_t) readBCDValue(1);
    }

    void Stream::writeBCDUInt16(uint16_t value) {
        writeBCDValue(value, 2);
    }

    uint16_t Stream::readBCDUInt16() {
        return (uint16_t) readBCDValue(2);
    }

    void Stream::writeBCDInt16(int16_t value) {
        writeBCDUInt16(value);
    }

    int16_t Stream::readBCDInt16() {
        return (int16_t) readBCDUInt16();
    }

    void Stream::writeBCDValue(int64_t value, int length) {
        uint8_t buffer[8];
        memset(buffer, 0, sizeof(buffer));
        BCDUtilities::Int64ToBCD(value, buffer, length);
        write(buffer, 0, length);
    }

    uint32_t Stream::readBCDValue(int length) {
        uint8_t buffer[8];
        memset(buffer, 0, sizeof(buffer));
        read(buffer, 0, length);
        return (uint32_t) BCDUtilities::BCDToInt64(buffer, 0, length);
    }

    bool Stream::canWrite() const {
        return true;
    }

    bool Stream::canRead() const {
        return true;
    }

    bool Stream::canSeek() const {
        return true;
    }

    const Version &Stream::version() const {
        return _version;
    }

    void Stream::setVersion(const Version &version) {
        _version = version;
    }

    Mutex *Stream::mutex() {
        return &_mutex;
    }

    bool Stream::writeText(const String &text) {
        const size_t length = text.length();
        return write((const uint8_t *) text.c_str(), 0, length) == length;
    }

    bool Stream::readText(String &text, int maxLength) {
        int cacheCount = Math::min(maxLength + 4, 1024);
        uint8_t *buffer = new uint8_t[cacheCount];
        int64_t count = read(buffer, 0, cacheCount);
        if (count > 0) {
            off_t offset = 0;
            if (count > 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) // UTF8 BOM
                offset = 3;
            String temp = String((const char *) buffer + offset, (uint32_t) (count - offset));
            text = temp.substr(0, Math::min(maxLength, (int) temp.length()));
        }
        delete[] buffer;

        return true;
    }

    bool Stream::readToEnd(ByteArray &array, uint32_t cacheCount) {
        uint8_t *buffer = new uint8_t[cacheCount];
        int64_t count = 0;
        do {
            count = read(buffer, 0, cacheCount);
            if (count > 0) {
                array.addRange(buffer, (uint32_t) count);
            }
        } while (count > 0);
        delete[] buffer;

        return true;
    }

    bool Stream::readToEnd(String &str, uint32_t cacheCount) {
        str.empty();
        uint8_t *buffer = new uint8_t[cacheCount];
        int64_t count = 0;
        do {
            count = read(buffer, 0, cacheCount);
            if (count > 0) {
                str.append((const char *) buffer, (uint32_t) count);
            }
        } while (count > 0);
        delete[] buffer;

        return true;
    }

    String Stream::readLine(uint32_t cacheCount) {
        String str;
        uint8_t *buffer = new uint8_t[cacheCount];
        int64_t count = 0;
        do {
            int64_t pos = position();
            count = read(buffer, 0, cacheCount);
            if (count > 0) {
                off_t offset = 0;
                if (count > 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) // UTF8 BOM
                    offset = 3;
                String temp = String((const char *) buffer + offset, (uint32_t) (count - offset));
                int index = temp.find('\n');
                if (index >= 0) {
                    str.append(temp.substr(0, index));
                    str = str.trim('\r', '\n');
                    seek(pos + offset + index + 1);
                    break;
                } else {
                    str.append(temp);
                }
            }
        } while (count > 0);
        delete[] buffer;
        return str;
    }

    void Stream::writeLine(const String &str) {
        write((uint8_t *) str.c_str(), 0, str.length());
        writeByte('\n');
    }
}
