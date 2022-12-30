#ifndef IOPORT_H
#define IOPORT_H

#if !WIN32

#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/poll.h>

#endif

#include "data/ValueType.h"

using namespace Data;

namespace IO {
#ifdef WIN32
    typedef void* handle;
    const handle invalidHandle = ((handle)-1);
#else
    typedef int handle;
    const handle invalidHandle = -1;
#endif

    class IOPort {
    public:
        IOPort(const String &name);

        virtual ~IOPort();

        virtual bool open();

        void close();

        bool isOpen() const;

        const String &portName() const;

        size_t available();

        ssize_t write(const char *data, size_t count);

        ssize_t read(char *data, size_t count);

        ssize_t read(char *data, size_t count, uint32_t timeout);

        ssize_t readLine(char *data, size_t count, uint32_t timeout, const char *newLine = "\n");

        ssize_t readEndBytes(char *buffer, size_t bufferLength, const char *endBuffer, size_t ebLength, int suffix,
                             uint32_t timeout);

    protected:
        handle _handle;
        String _portName;
    };
}
#endif  //IOPORT_H
