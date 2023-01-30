//
//  MemoryStream.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#include "IO/MemoryStream.h"
#include "system/Math.h"

using namespace System;

namespace IO {
    MemoryStream::MemoryStream(size_t capacity) : _copyBuffer(true), _position(0) {
        _buffer = new ByteArray(capacity);
    }

    MemoryStream::MemoryStream(const ByteArray *buffer, bool copyBuffer) : _copyBuffer(copyBuffer), _position(0) {
        if (_copyBuffer)
            _buffer = new ByteArray(*buffer);
        else
            _buffer = (ByteArray *) buffer;
    }

    MemoryStream::MemoryStream(const uint8_t *buffer, size_t count, size_t capacity) : _copyBuffer(true), _position(0) {
        _buffer = new ByteArray(buffer, count, capacity);
    }

    MemoryStream::MemoryStream(const MemoryStream &stream) : Stream(stream) {
        _copyBuffer = stream._copyBuffer;
        _position = stream._position;
        if (_copyBuffer)
            _buffer = new ByteArray(*stream._buffer);
        else
            _buffer = stream._buffer;
    }

    MemoryStream::~MemoryStream() {
        if (_copyBuffer)
            delete _buffer;
        _buffer = nullptr;
    }

    off_t MemoryStream::position() const {
        return _position;
    }

    size_t MemoryStream::length() const {
        return _buffer->count();
    }

    off_t MemoryStream::seek(off_t offset, SeekOrigin origin) {
        if (origin == SeekOrigin::SeekBegin) {
            if (offset >= 0 && offset <= (off_t) _buffer->count()) {
                _position = offset;
                return _position;
            }
        } else if (origin == SeekOrigin::SeekCurrent) {
            if (offset >= 0 && offset <= (off_t) _buffer->count() - _position) {
                _position += offset;
                return _position;
            }
        } else if (origin == SeekOrigin::SeekEnd) {
            if (offset >= 0 && offset <= (off_t) _buffer->count() - _position) {
                _position = (off_t) _buffer->count() - offset;
                return _position;
            }
        }
        return -1;
    }

    void MemoryStream::flush() {
    }

    void MemoryStream::close() {
        clear();
    }

    void MemoryStream::clear() {
        seek(0);
        _buffer->clear();
    }

    const ByteArray *MemoryStream::buffer() const {
        return _buffer;
    }

    ssize_t MemoryStream::write(const uint8_t *array, off_t offset, size_t count) {
        if (_position == (off_t) _buffer->count()) {
            _buffer->addRange((array + offset), count);
        } else if (_position < (off_t) _buffer->count()) {
            _buffer->setRange((uint32_t) _position, (array + offset), count);
        } else {
            size_t c = _position - _buffer->count();
            auto *zero = new uint8_t[c];
            memset(zero, 0, c);
            _buffer->addRange(zero, c);
            _buffer->addRange((array + offset), count);
            delete[] zero;
        }
        _position += (off_t) count;
        return (ssize_t) count;
    }

    ssize_t MemoryStream::read(uint8_t *array, off_t offset, size_t count) {
        if (_position >= 0 && _position < (off_t) length() &&
            count > 0) {
            uint8_t *temp = _buffer->data();
            size_t bufferCount = _buffer->count();
            size_t readCount = Math::min(count, bufferCount - (size_t) _position);
            memcpy(array + offset, temp + _position, readCount);
            _position += (off_t) readCount;
            return (ssize_t) readCount;
        }
        return 0;
    }

    void MemoryStream::copyTo(ByteArray &buffer) const {
        buffer = *_buffer;
    }
}
