//
//  IOPort.h
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "data/ByteArray.h"
#include "IO/IOPort.h"
#include "diag/Stopwatch.h"

#ifdef WIN32

#include <Windows.h>

#elif MSYS

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

#else

#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#endif

using namespace Diag;

namespace IO {
    IOPort::IOPort() : _handle(invalidHandle) {
    }

    IOPort::~IOPort() {
        close();
    }

    bool IOPort::connected() {
        return isOpen();
    }

    ssize_t IOPort::send(const uint8_t *buffer, off_t offset, size_t count) {
        ssize_t len = 0;
        if (isOpen()) {
#ifdef WIN32
            LPDWORD length = nullptr;
            COMSTAT ComStat;
            DWORD dwErrorFlags;
            ClearCommError(_handle, &dwErrorFlags, &ComStat);
            if (!WriteFile(_handle, (void *) (buffer + offset), (DWORD) count, (LPDWORD) &length, nullptr)) {
                length = (LPDWORD) -1;
            }
            len = (ssize_t) length;
#else
            len = ::write(_handle, buffer + offset, count);
#endif
        }
        return len;
    }

    ssize_t IOPort::receive(uint8_t *buffer, off_t offset, size_t count) {
        ssize_t len = 0;
        if (isOpen()) {
#ifdef WIN32
            LPDWORD length = nullptr;
            if (!ReadFile(_handle, (void *) (buffer + offset), (DWORD) count, (LPDWORD) &length, nullptr)) {
                length = (LPDWORD) -1;
            }
            len = (ssize_t) length;
#else
            len = ::read(_handle, buffer + offset, count);
#endif
        }
        return len;
    }

    bool IOPort::open() {
#ifdef DEBUG
        Stopwatch sw(String::convert("IOPort::Open, name: %s", name().c_str()), 200);
#endif
#ifdef WIN32
        String portName = name();
        String portExt = R"(\\.\)";
        if (portName.length() > 4) {
            portName = portExt + portName;
        }
        if (isOpen()) {
            close();
        }
        _handle = CreateFileA(portName,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr,
                              OPEN_EXISTING,
                              0,
                              nullptr);
        if (_handle == INVALID_HANDLE_VALUE) {
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
        _handle = ::open(name(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (_handle < 0) {
            Debug::writeFormatLine("Can not find the comport, name: %s", name().c_str());
            return false;
        }
#endif
        return true;
    }

    void IOPort::close() {
#ifdef DEBUG
        Stopwatch sw(String::convert("IOPort::Close, name: %s", name().c_str()), 200);
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

    bool IOPort::useReceiveTimeout() const {
        return false;
    }
}
