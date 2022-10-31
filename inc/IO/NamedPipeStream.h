#ifndef NAMEDPIPESTREAM_H
#define NAMEDPIPESTREAM_H

#ifdef WIN32
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include "data/ValueType.h"
#include "data/DateTime.h"
#include "Stream.h"

namespace Common
{
    enum PipeMode
    {
        PipeRead = 1,
        PipeWrite = 2,
        PipeReadWrite = 3
    };
    class NamedPipeStream : public Stream
	{
	public:
        NamedPipeStream(const String& pipeName, PipeMode mode);
        ~NamedPipeStream() override;

        ssize_t write(const uint8_t* array, off_t offset, size_t count) override;
        ssize_t read(uint8_t* array, off_t offset, size_t count) override;

        off_t position() const override;
        size_t length() const override;
        bool seek(off_t offset, SeekOrigin origin = SeekOrigin::SeekBegin) override;
        
        bool isOpen() const;
        void close();

#ifdef WIN32
		bool connect() const;
		bool waitForConnection(uint timeout = 10 * 1000);
#endif
        
    public:
        static bool exists(const String& pipeName);
        static bool remove(const String& pipeName);
        
    private:
#ifndef mode_t
    #define mode_t int
#endif
    static mode_t fifoMode(PipeMode mode);
    static mode_t openMode(PipeMode mode);

    private:
        String _pipeName;
#ifdef WIN32
		PipeMode _pipeMode;
		HANDLE _fd;
#define InvalidHandle INVALID_HANDLE_VALUE
#else
        int _fd;
        static const int InvalidHandle = -1;
#endif
	};
}

#endif // NAMEDPIPESTREAM_H
