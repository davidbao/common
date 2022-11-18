//
//  MemoryStream.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#include "IO/MemoryStream.h"
#include "data/ByteArray.h"

namespace Common
{
    MemoryStream::MemoryStream(size_t capacity) : _buffer(nullptr), _copyBuffer(true), _position(0)
    {
        _buffer = new ByteArray(capacity);
    }
    MemoryStream::MemoryStream(const ByteArray* buffer, bool copyBuffer) : _buffer(nullptr), _copyBuffer(copyBuffer), _position(0)
    {
        if(_copyBuffer)
            _buffer = new ByteArray(*buffer);
        else
            _buffer = (ByteArray*)buffer;
    }
    MemoryStream::MemoryStream(const uint8_t* buffer, size_t count, size_t capacity) : _buffer(nullptr), _copyBuffer(true), _position(0)
    {
        _buffer = new ByteArray(buffer, count, capacity);
    }
    MemoryStream::MemoryStream(const MemoryStream& stream)
    {
        _copyBuffer = stream._copyBuffer;
        _position = stream._position;
        if(_copyBuffer)
            _buffer = new ByteArray(*stream._buffer);
        else
            _buffer = stream._buffer;
    }
    MemoryStream::~MemoryStream()
    {
        if(_copyBuffer)
            delete _buffer;
        _buffer = nullptr;
    }

    off_t MemoryStream::position() const
    {
        return _position;
    }
    size_t MemoryStream::length() const
    {
        return _buffer->count();
    }
    bool MemoryStream::seek(off_t offset, SeekOrigin origin)
    {
        if (origin == SEEK_SET)
        {
            if (offset >= 0 && offset <= _buffer->count())
            {
                _position = offset;
                return true;
            }
        }
        else if (origin == SEEK_CUR)
        {
            if (offset >= 0 && offset <= _buffer->count() - _position)
            {
                _position += offset;
                return true;
            }
        }
        else if (origin == SEEK_END)
        {
            if (offset >= 0 && offset <= _buffer->count() - _position)
            {
                _position = _buffer->count() - offset;
                return true;
            }
        }
        return false;
    }
    void MemoryStream::clear()
    {
        seek(0);
        _buffer->clear();
    }
    const ByteArray* MemoryStream::buffer() const
    {
        return _buffer;
    }
    ssize_t MemoryStream::write(const uint8_t* array, off_t offset, size_t count)
    {
        if (_position == _buffer->count())
        {
            _buffer->addRange((array + offset), count);
        }
        else if (_position < _buffer->count())
        {
            _buffer->setRange((uint32_t)_position, (array + offset), count);
        }
        else	// _position > _buffer->count()
        {
            size_t c = _position - _buffer->count();
            uint8_t* zero = new uint8_t[c];
            memset(zero, 0, c);
            _buffer->addRange(zero, c);
            _buffer->addRange((array + offset), count);
            delete[] zero;
        }
        _position += count;
        return count;
    }
    size_t MemoryStream::minInteger(size_t a, size_t b)
    {
        return a < b ? a : b;
    }
    ssize_t MemoryStream::read(uint8_t* array, off_t offset, size_t count)
    {
        if (_position >= 0 && _position < (int)length() &&
            count > 0)
        {
            uint8_t* temp = _buffer->data();
            size_t bufferCount = _buffer->count();
            size_t readCount = minInteger(count, bufferCount - _position);
            memcpy(array + offset, temp + (int)_position, readCount);
            _position += readCount;
            return readCount;
        }
        return 0;
    }
    
    void MemoryStream::copyTo(uint8_t* buffer) const
    {
        uint8_t* temp = _buffer->data();
        memcpy(buffer, temp, (size_t)length());
    }
    void MemoryStream::copyTo(ByteArray& buffer) const
    {
        buffer = *_buffer;
    }
}
