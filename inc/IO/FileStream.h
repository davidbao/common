//
//  FileStream.h
//  common
//
//  Created by baowei on 2017/1/4.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef FileStream_h
#define FileStream_h

#include "IO/Stream.h"

namespace IO {
    enum FileMode {
        // Creates a new file. An exception is raised if the file already exists.
        FileCreateNew = 1,

        // Creates a new file. If the file already exists, it is overwritten.
        FileCreate = 2,

        // Opens an existing file. An exception is raised if the file does not exist.
        FileOpen = 3,

        // Opens the file if it exists. Otherwise, creates a new file.
        FileOpenOrCreate = 4,

        // Opens an existing file. Once opened, the file is truncated so that its
        // size is zero bytes. The calling process must open the file with at least
        // WRITE access. An exception is raised if the file does not exist.
        FileTruncate = 5,

        // Opens the file if it exists and seeks to the end.  Otherwise,
        // creates a new file.
        FileAppend = 6,

        // Opens an existing file without exception.
        FileOpenWithoutException = 7,
    };
    enum FileAccess {
        // Specifies read access to the file. Data can be read from the file and
        // the file pointer can be moved. Combine with WRITE for read-write access.
        FileRead = 1,

        // Specifies write access to the file. Data can be written to the file and
        // the file pointer can be moved. Combine with READ for read-write access.
        FileWrite = 2,

        // Specifies read and write access to the file. Data can be written to the
        // file and the file pointer can be moved. Data can also be read from the
        // file.
        FileReadWrite = 3,
    };

    class MappingStream;

    class FileStream : public Stream {
    public:
        using Stream::seek;

        FileStream(const String &filename, FileMode mode, FileAccess access = FileReadWrite);

        ~FileStream() override;

        ssize_t write(const uint8_t *array, off_t offset, size_t count) override;

        ssize_t read(uint8_t *array, off_t offset, size_t count) override;

        off_t position() const override;

        size_t length() const override;

        off_t seek(off_t offset, SeekOrigin origin) override;

        bool canWrite() const override;

        bool canRead() const override;

        void flush() override;

        void close() override;

        bool isOpen() const;

        void setLength(size_t length);

        int fd() const;

        const String &fileName() const;

    private:
        void open(const String &fileName, int openFlag, int mode);

    private:
        static int openFlag(const String &fileName, FileMode mode, FileAccess access);

        static int openMode(FileAccess access);

    private:
        friend MappingStream;

        int _fd;
        String _fileName;
        FileAccess _access;

    private:
        static const int InvalidHandle = -1;
    };
}

#endif // FileStream_h
