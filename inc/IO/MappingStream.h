//
//  MappingStream.h
//  common
//
//  Created by baowei on 2015/9/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef MappingStream_h
#define MappingStream_h

#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "data/PList.h"

namespace IO {
    class MappingStream : public Stream {
    public:
        using Stream::seek;

#if WIN32
        typedef void* ViewMapping;
#else
        typedef int ViewMapping;
#endif
        typedef void *ViewAccessor;

        class View {
        public:
            off_t offset;
            size_t size;

            View(ViewMapping mapping, FileAccess access, off_t offset, size_t size);

            ~View();

            ssize_t write(off_t position, const uint8_t *array, off_t offset, size_t count);

            ssize_t read(off_t position, uint8_t *array, off_t offset, size_t count);

            void close();

            bool flush();

        private:
            bool isValid() const;

            void updateViewAccessor();

            bool canWrite() const;

            bool canRead() const;

        private:
            ViewAccessor _accessor;
            ViewMapping _mapping;
            FileAccess _access;
        };

        typedef PList<View> Views;

        MappingStream(const String &fileName, size_t fileSize);

        explicit MappingStream(const String &fileName);

        ~MappingStream() override;

        ssize_t write(const uint8_t *array, off_t offset, size_t count) override;

        ssize_t read(uint8_t *array, off_t offset, size_t count) override;

        off_t position() const override;

        size_t length() const override;

        off_t seek(off_t offset, SeekOrigin origin) override;

        void flush() override;

        void close() override;

        bool canWrite() const override;

        bool canRead() const override;

        bool canSeek() const override;

        bool isOpen() const;

    private:
        size_t viewSize() const;

    private:
        // https://connect.microsoft.com/VisualStudio/feedback/details/552859/memorymappedviewaccessor-flush-throws-ioexception?wa=wsignin1.0
        const int64_t WriteViewSize = 20L * 1024 * 1024;
        const int64_t ReadViewSize = 256L * 1024 * 1024;

        size_t _fileSize;
        Views _views;
        FileAccess _access;
        off_t _position;

#if WIN32
        void* _mapFile;
        void* _file;
#else
        FileStream *_mapFile;
#endif
    };
}

#endif // MappingStream_h
