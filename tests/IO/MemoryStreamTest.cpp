//  MemoryStreamTest.cpp
//  common
//
//  Created by baowei on 2023/1/20.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/MemoryStream.h"

using namespace IO;

bool testConstructor() {
    {
        MemoryStream ms;
        if (ms.length() != 0) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms(&buffer, true);
        if (buffer != *ms.buffer()) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms(&buffer, false);
        if (buffer.data() != ms.buffer()->data()) {
            return false;
        }
    }

    {
        uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms(buffer, sizeof(buffer));
        if (memcmp(buffer, ms.buffer()->data(), sizeof(buffer)) != 0) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms(&buffer, true);
        MemoryStream ms2(ms);
        if (*ms.buffer() != *ms2.buffer()) {
            return false;
        }
    }

    return true;
}

bool testReadWrite() {
    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms(&buffer, true);
        ByteArray actual = ms.readBytes(8);
        if (buffer != actual) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        ms.seek(0);
        ByteArray actual = ms.readBytes(8);
        if (buffer != actual) {
            return false;
        }
    }

    return true;
}

bool testProperties() {
    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);

        if (ms.length() != 8) {
            return false;
        }

        if (ms.position() != 8) {
            return false;
        }
        ms.seek(1);
        if (ms.position() != 1) {
            return false;
        }
        ms.seek(10);
        if (ms.position() != 1) {
            return false;
        }
    }

    return true;
}

bool testSeek() {
    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(-2);
        if (position >= 0) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(1);
        if (position != 1) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(10);
        if (position >= 0) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(0, SeekOrigin::SeekEnd);
        if (position != 8) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(-2, SeekOrigin::SeekEnd);
        if (position != 6) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(2, SeekOrigin::SeekEnd);
        if (position != -1) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(-2, SeekOrigin::SeekCurrent);
        if (position != 6) {
            return false;
        }
    }

    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        off_t position = ms.seek(2, SeekOrigin::SeekCurrent);
        if (position != -1) {
            return false;
        }
    }

    return true;
}

bool testCopyTo() {
    {
        ByteArray buffer = {1, 2, 3, 4, 5, 6, 7, 8};
        MemoryStream ms;
        ms.writeBytes(buffer);
        ByteArray actual;
        ms.copyTo(actual);
        if (actual != buffer) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    if (!testReadWrite()) {
        return 2;
    }

    if (!testProperties()) {
        return 3;
    }

    if (!testSeek()) {
        return 4;
    }

    if (!testCopyTo()) {
        return 5;
    }

    return 0;
}