#if WIN32
#include <io.h>
#include <Windows.h>
#else
#include <errno.h>
#endif
#include "IO/NamedPipeStream.h"
#include "diag/Trace.h"
#include "IO/File.h"
#include "exception/Exception.h"

namespace Common
{
    NamedPipeStream::NamedPipeStream(const String& pipeName, PipeMode mode) : _pipeName(pipeName), _fd(InvalidHandle)
    {
#ifdef WIN32
		const String name = String::convert("\\\\.\\Pipe\\%s", pipeName.c_str());
		_pipeName = name;
		_pipeMode = mode;
		_fd = CreateNamedPipe(name, fifoMode(mode), PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000, nullptr);
		if (_fd == INVALID_HANDLE_VALUE)
		{
			// Retrieve the system error message for the last-error code

			LPVOID lpMsgBuf;
			DWORD dw = GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, nullptr);

			Debug::writeFormatLine("can't open named pipe: name: %s, error: %s", pipeName.c_str(), (LPTSTR)lpMsgBuf);
			LocalFree(lpMsgBuf);
		}
#else
		const String name = String::convert("/tmp/%s", pipeName.c_str());
		_pipeName = name;
        if(exists(name))
        {
			_fd = ::open(name.c_str(), openMode(mode));
            if (_fd == InvalidHandle)
            {
                Debug::writeFormatLine("can't open named pipe: name: %s, error: %s", pipeName.c_str(), strerror(errno));
            }
        }
        else
        {
			if(mkfifo(name.c_str(), fifoMode(mode)) == 0)
            {
				_fd = ::open(name.c_str(), openMode(mode));
                if (_fd == InvalidHandle)
                {
                    Debug::writeFormatLine("can't open named pipe: name: %s, error: %s", pipeName.c_str(), strerror(errno));
                }
            }
            else
            {
                int errnum = errno;
                char * mesg = strerror(errnum);
                Debug::writeFormatLine("mkfifo wrong!, code: %d, reason: %s", errnum, mesg);
            }
        }
#endif
	}
    NamedPipeStream::~NamedPipeStream()
    {
        close();
    }

    ssize_t NamedPipeStream::write(const uint8_t* array, off_t offset, size_t count)
    {
        if (isOpen())
        {
#ifdef WIN32
			DWORD len;
			if(WriteFile(_fd, array + offset, (DWORD)count, &len, nullptr))
				return len;
#else
            return ::write(_fd, array + offset, count);
#endif
        }
        return 0;
    }
    ssize_t NamedPipeStream::read(uint8_t* array, off_t offset, size_t count)
    {
        if (isOpen())
        {
#ifdef WIN32
			DWORD len;
			if (ReadFile(_fd, array + offset, (DWORD)count, &len, nullptr))
				return len;
#else
            return ::read(_fd, array + offset, count);
#endif
        }
        return 0;
    }

    off_t NamedPipeStream::position() const
    {
        throw NotSupportedException("The named pipe can not support seek.");
    }
    size_t NamedPipeStream::length() const
    {
        throw NotSupportedException("The named pipe can not support seek.");
    }
    bool NamedPipeStream::seek(off_t offset, SeekOrigin origin)
    {
        throw NotSupportedException("The named pipe can not support seek.");
    }
    
    void NamedPipeStream::close()
    {
        if (_fd != InvalidHandle)
        {
#ifdef WIN32
			FlushFileBuffers(_fd);
			DisconnectNamedPipe(_fd); 
			CloseHandle(_fd);
#else
            ::close(_fd);
#endif
        }
        _fd = InvalidHandle;
    }
    bool NamedPipeStream::isOpen() const
    {
        return _fd != InvalidHandle;
    }
    
    bool NamedPipeStream::exists(const String& pipeName)
    {
        return File::exists(pipeName);
//        return mkfifo(pipeName.c_str(), O_CREAT | O_EXCL) < 0 && errno == EEXIST;
    }
    bool NamedPipeStream::remove(const String& pipeName)
    {
        if(unlink(pipeName.c_str()) < 0)
        {
            int errnum = errno;
            char * mesg = strerror(errnum);
            Debug::writeFormatLine("unlink wrong!, reason: %s", mesg);
            return false;
        }
        return true;
    }
    
    mode_t NamedPipeStream::fifoMode(PipeMode mode)
    {
#ifdef WIN32
		switch (mode)
		{
		case PipeRead:
			return PIPE_ACCESS_INBOUND;
		case PipeWrite:
			return PIPE_ACCESS_OUTBOUND;
		case PipeReadWrite:
			return PIPE_ACCESS_DUPLEX;
		default:
			return PIPE_ACCESS_INBOUND;
		}
#else
        switch (mode)
        {
            case PipeRead:
                return S_IFIFO | O_CREAT | O_RDONLY | S_IRUSR;
            case PipeWrite:
                return S_IFIFO | O_CREAT | O_WRONLY | S_IWUSR;
            case PipeReadWrite:
                return S_IFIFO | O_CREAT | O_RDWR | S_IRWXU;
            default:
                return S_IFIFO | O_CREAT | O_RDONLY;
        }
#endif
    }
    mode_t NamedPipeStream::openMode(PipeMode mode)
    {
#ifdef WIN32
		switch (mode)
		{
		case PipeRead:
			return GENERIC_READ;
		case PipeWrite:
			return GENERIC_WRITE;
		case PipeReadWrite:
			return GENERIC_READ | GENERIC_WRITE;
		default:
			return GENERIC_READ;
	}
#else
        switch (mode)
        {
            case PipeRead:
                return O_RDONLY;
            case PipeWrite:
                return O_WRONLY;
            case PipeReadWrite:
                return O_RDWR;
            default:
                return O_RDONLY;
        }
#endif
    }

#ifdef WIN32
	bool NamedPipeStream::connect() const
	{
		if (isOpen())
			return ConnectNamedPipe(_fd, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
		else
			return false;
	}
	bool NamedPipeStream::waitForConnection(uint32_t timeout)
	{
		while (1)
		{
			_fd = CreateFile(
				_pipeName,   // pipe name 
				openMode(_pipeMode),
				0,              // no sharing 
				nullptr,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe 
				0,              // default attributes 
				nullptr);          // no template file 

			// Break if the pipe handle is valid. 

			if (_fd != INVALID_HANDLE_VALUE)
				return true;

			// Exit if an error other than ERROR_PIPE_BUSY occurs. 

			if (GetLastError() != ERROR_PIPE_BUSY)
			{
				Debug::writeFormatLine("Could not open pipe. GLE=%d", GetLastError());
				return false;
			}

			// All pipe instances are busy, so wait for 20 seconds. 

			if (!WaitNamedPipe(_pipeName, timeout))
			{
				Debug::writeFormatLine("Could not open pipe: %d ms wait timed out.", timeout);
				return false;
			}
		}
	}
#endif
}
