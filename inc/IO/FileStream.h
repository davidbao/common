#ifndef FILESTREAM_H
#define FILESTREAM_H

#if WIN32
#include <io.h>
#else
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include "Stream.h"
#include "diag/Trace.h"
#include "data/ByteArray.h"

namespace Common
{
    enum FileMode
    {
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
    enum FileAccess
    {
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
	class FileStream : public Stream
	{
	public:
        FileStream(const char* filename, FileMode mode, FileAccess access = FileReadWrite);
        FileStream(const String& filename, FileMode mode, FileAccess access = FileReadWrite);
        ~FileStream() override;

        ssize_t write(const uint8_t* array, off_t offset, size_t count) override;
        ssize_t read(uint8_t* array, off_t offset, size_t count) override;

        off_t position() const override;
        size_t length() const override;
		bool seek(off_t offset, SeekOrigin origin = SeekOrigin::SeekBegin) override;

        bool isOpen() const;
		void close();

        void setLength(size_t length);
        void flush();
        
        int fd() const;
        const String& fileName() const;
        
    private:
        void open(const char* filename, int openFlag, int mode);
        
        static int openFlag(const char* filename, FileMode mode, FileAccess access);
#ifndef mode_t
#define mode_t int
#endif
        static mode_t openMode(FileAccess access);

	private:
        friend MappingStream;
        
		int _fd;
        String _fileName;
        
    private:
        static const int InvalidHandle = -1;
	};
}

#endif // FILESTREAM_H
