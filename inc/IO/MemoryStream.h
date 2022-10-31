//
//  MemoryStream.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef MemoryStream_h
#define MemoryStream_h

#include "Stream.h"

namespace Common
{
    class ByteArray;
	class MemoryStream : public Stream
	{
	public:
        MemoryStream(size_t capacity = DefaultCapacity);
        MemoryStream(const ByteArray* buffer, bool copyBuffer = true);
        MemoryStream(const uint8_t* buffer, size_t count, size_t capacity = DefaultCapacity);
        MemoryStream(const MemoryStream& stream);
        ~MemoryStream() override;

		ssize_t write(const uint8_t* array, off_t offset, size_t count) override;
		ssize_t read(uint8_t* array, off_t offset, size_t count) override;

        off_t position() const override;
        size_t length() const override;
		bool seek(off_t offset, SeekOrigin origin = SeekOrigin::SeekBegin) override;

        void clear();
        
        const ByteArray* buffer() const;
        
        void copyTo(uint8_t* buffer) const;
        void copyTo(ByteArray& buffer) const;
        
    private:
        size_t minInteger(size_t a, size_t b);

	private:
		ByteArray* _buffer;
        bool _copyBuffer;
        off_t _position;
        
        static const int DefaultCapacity = 1024;
	};
}

#endif // MemoryStream_h
