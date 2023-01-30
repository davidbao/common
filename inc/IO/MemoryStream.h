//
//  MemoryStream.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef MemoryStream_h
#define MemoryStream_h

#include "IO/Stream.h"

namespace Data {
    class ByteArray;
}

namespace IO {
    class MemoryStream : public Stream {
    public:
        using Stream::seek;

        explicit MemoryStream(size_t capacity = DefaultCapacity);

        explicit MemoryStream(const ByteArray *buffer, bool copyBuffer = true);

        MemoryStream(const uint8_t *buffer, size_t count, size_t capacity = DefaultCapacity);

        MemoryStream(const MemoryStream &stream);

        ~MemoryStream() override;

        ssize_t write(const uint8_t *array, off_t offset, size_t count) override;

        ssize_t read(uint8_t *array, off_t offset, size_t count) override;

        off_t position() const override;

        size_t length() const override;

        off_t seek(off_t offset, SeekOrigin origin) override;

        void flush() override;

        void close() override;

        void clear();

        const ByteArray *buffer() const;

        void copyTo(ByteArray &buffer) const;

    private:
        ByteArray *_buffer;
        bool _copyBuffer;
        off_t _position;

    private:
        static const int DefaultCapacity = 1024;
    };
}

#endif // MemoryStream_h
