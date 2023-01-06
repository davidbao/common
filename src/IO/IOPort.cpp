#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "data/ByteArray.h"
#include "thread/TickTimeout.h"
#include "IO/IOPort.h"

#if WIN32
#include <Windows.h>
#endif
#ifdef MSYS
#define IOCPARM_MASK 0x7f
#define IOC_VOID 0x20000000
#define IOC_OUT 0x40000000
#define IOC_IN 0x80000000
#define IOC_INOUT (IOC_IN|IOC_OUT)

#define _IO(x,y) (IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t) (IOC_OUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t) (IOC_IN|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define FIONREAD _IOR('f',127,u_long)
#define FIONBIO _IOW('f',126,u_long)
#define FIOASYNC _IOW('f',125,u_long)
#endif

using namespace Diag;

namespace IO {
    IOPort::IOPort(const String &name) {
        _portName = name;
        _handle = invalidHandle;
    }

    IOPort::~IOPort() {
        close();
    }

    bool IOPort::open() {
#ifdef DEBUG
        Stopwatch sw(String::convert("IOPort::Open, name: %s", _portName.c_str()), 200);
#endif
#ifdef WIN32
        String portExt = "\\\\.\\";
        if(_portName.length() > 4)
        {
            _portName = portExt + _portName;
        }
        if (isOpen())
        {
            close();
        }
        _handle = CreateFileA(_portName.c_str(),
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
        if (_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        COMMTIMEOUTS ct;
        ct.ReadIntervalTimeout = 10;
        ct.ReadTotalTimeoutConstant = 10;
        ct.ReadTotalTimeoutMultiplier = 0;
        ct.WriteTotalTimeoutMultiplier = 10;
        ct.WriteTotalTimeoutConstant = 3000;
        SetCommTimeouts(_handle, &ct);
#else
        if (isOpen()) {
            close();
        }
        /*open the port*/
        _handle = ::open(_portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (_handle < 0) {
            Debug::writeFormatLine("Can not find the comport, name: %s", _portName.c_str());
            return false;
        }

        //struct termios ts;
        //tcgetattr(_handle, &ts);
        //ts.c_cc[VTIME] = 150;
        //ts.c_cc[VMIN] = 1;
        //tcsetattr(_handle, TCSANOW, &ts);
#endif
        return true;
    }

    void IOPort::close() {
#ifdef DEBUG
        Stopwatch sw(String::convert("IOPort::Close, name: %s", _portName.c_str()), 200);
#endif
        if (isOpen()) {
#if WIN32
            CloseHandle(_handle);
#else
            ::close(_handle);
#endif
            _handle = invalidHandle;
        }
    }

    bool IOPort::isOpen() const {
        return _handle != invalidHandle;
    }

    const String &IOPort::portName() const {
        return _portName;
    }

    size_t IOPort::available() {
        size_t nb = 0;
        if (isOpen()) {
#ifdef WIN32
            DWORD dwErrorFlags = 0;
            COMSTAT ComStat;
            memset(&ComStat, 0, sizeof(ComStat));
            ClearCommError(_handle, &dwErrorFlags, &ComStat);
            nb = ComStat.cbInQue;
#else
            //Debug::writeLine("IOPort::available()");
            ioctl(_handle, FIONREAD, &nb);
#endif
        }
        return nb;
    }

    ssize_t IOPort::write(const char *data, size_t count) {
        ssize_t len = 0;
        if (isOpen()) {
#ifdef WIN32
            LPDWORD length = 0;
            COMSTAT ComStat;
            DWORD dwErrorFlags;
            ClearCommError(_handle,&dwErrorFlags,&ComStat);
            if (!WriteFile(_handle, (void*)data, (DWORD)count,(LPDWORD) &length, NULL))
            {
                length = (LPDWORD)-1;
            }
            len = (ssize_t)length;
#else
            len = ::write(_handle, data, count);
#endif
        }
        return len;
    }

    ssize_t IOPort::read(char *data, size_t count) {
        ssize_t len = 0;
        if (isOpen()) {
#ifdef WIN32
            LPDWORD length=0;
            if (!ReadFile(_handle, (void*)data, (DWORD)count, (LPDWORD)&length, NULL))
            {
                length = (LPDWORD)-1;
            }
            len = (ssize_t)length;
#else
            len = ::read(_handle, data, count);
#endif
        }
        return len;
    }

    ssize_t IOPort::read(char *data, size_t count, uint32_t timeout) {
        ssize_t len = 0;
        if (isOpen()) {
            uint32_t startTime = TickTimeout::getCurrentTickCount();
            uint32_t deadTime = TickTimeout::getDeadTickCount(startTime, timeout);
            do {
                size_t available = this->available();
                if (available >= count) {
                    len = count;
                    break;
                }

                if (TickTimeout::isTimeout(startTime, deadTime)) {
                    len = available;
                    break;
                }

                Thread::msleep(1);
            } while (true);

            return read(data, len);
        }
        return len;
    }

    ssize_t IOPort::readEndBytes(char *buffer, size_t bufferLength, const char *endBuffer, size_t ebLength, int suffix,
                                 uint32_t timeout) {
        ssize_t received = 0;
        if (isOpen()) {
            if (buffer == nullptr || endBuffer == nullptr)
                return received;

            bool bReceiveEndBytes = false;
            int nSuffix = 0, nStartByte = 0;
            uint32_t startTime = TickTimeout::getCurrentTickCount();
            uint32_t deadTime = TickTimeout::getDeadTickCount(startTime, timeout);
            do {
                if (bReceiveEndBytes && nSuffix >= suffix)
                    break;
                int nBytesToRead = (int) available();
                if (nBytesToRead <= 0) {
                    Thread::msleep(1);
                    if (TickTimeout::isTimeout(startTime, deadTime))
                        break;
                    continue;
                }
                if (received + 1 <= (ssize_t) bufferLength) {
                    received += read(buffer + received, 1);
                } else {
                    return received;
                }

                if (!bReceiveEndBytes) {
                    if (buffer[received - 1] == endBuffer[nStartByte]) {
                        nStartByte++;
                        if (nStartByte == ebLength) {
                            bReceiveEndBytes = true;
                        }
                    } else {
                        if (nStartByte == 0) {
                            continue;
                        }

                        nStartByte = 0;
                        if (buffer[received - 1] == endBuffer[nStartByte]) {
                            nStartByte++;
                        }
                    }
                } else {
                    nSuffix++;
                }

                deadTime = TickTimeout::getDeadTickCount(timeout);
            } while (true);
            return received;
        }
        return received;
    }

    ssize_t IOPort::readLine(char *data, size_t count, uint32_t timeout, const char *newLine) {
        return readEndBytes(data, count, newLine, strlen(newLine), 0, timeout);
    }
}
