#include "IO/SerialPort.h"
#include "diag/Stopwatch.h"

#if WIN32
#include <Windows.h>
#endif

namespace IO {
    SerialPort::SerialPort(const String &name) : IOPort(name) {
        _baudRate = 9600;
        _dataBits = SerialInfo::DATA_8;
        _stopBits = SerialInfo::STOP_1;
        _handshake = SerialInfo::FLOW_OFF;
        _parity = SerialInfo::PAR_NONE;
        _rtsEnable = false;
        _dtrEnable = false;
        _useSignal = false;
    }

    SerialPort::~SerialPort() {
    }

    bool SerialPort::open() {
        if (!IOPort::open())
            return false;

        if (isOpen()) {
            if (!setCommConfig(baudRate(), parity(), dataBits(), stopBits(), handshake()))
                return false;
        }
        return true;
    }

    void SerialPort::setBaudRate(int baudRate) {
        if (baudRate > 0) {
            if (isOpen()) {
                setCommConfig(baudRate, parity(), dataBits(), stopBits(), handshake());
            }
            _baudRate = baudRate;
        }
    }

    int SerialPort::baudRate() const {
        return _baudRate;
    }

    void SerialPort::setDataBits(SerialInfo::DataBitsType dataBits) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity(), dataBits, stopBits(), handshake());
        }
        _dataBits = dataBits;
    }

    SerialInfo::DataBitsType SerialPort::dataBits() const {
        return _dataBits;
    }

    void SerialPort::setParity(SerialInfo::ParityType parity) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity, dataBits(), stopBits(), handshake());
        }
        _parity = parity;
    }

    SerialInfo::ParityType SerialPort::parity() const {
        return _parity;
    }

    void SerialPort::setStopBits(SerialInfo::StopBitsType stopBits) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity(), dataBits(), stopBits, handshake());
        }
        _stopBits = stopBits;
    }

    SerialInfo::StopBitsType SerialPort::stopBits() const {
        return _stopBits;
    }

    void SerialPort::setHandshake(SerialInfo::HandshakeType handshake) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity(), dataBits(), stopBits(), handshake);
        }
        _handshake = handshake;
    }

    SerialInfo::HandshakeType SerialPort::handshake() const {
        return _handshake;
    }

    bool SerialPort::rtsEnable() const {
        return _rtsEnable;
    }

    void SerialPort::setRtsEnable(bool enabled) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity(), dataBits(), stopBits(), handshake());
        }
        _rtsEnable = enabled;
    }

    bool SerialPort::dtrEnable() const {
        return _dtrEnable;
    }

    void SerialPort::setDtrEnable(bool enabled) {
        if (isOpen()) {
            setCommConfig(baudRate(), parity(), dataBits(), stopBits(), handshake());
        }
        _dtrEnable = enabled;
    }

    bool SerialPort::useSignal() const {
        return _useSignal;
    }

    void SerialPort::setUseSignal(bool useSignal) {
        _useSignal = useSignal;
    }

    bool SerialPort::setCommConfig(int baudRate, SerialInfo::ParityType parity, SerialInfo::DataBitsType dataBits,
                                   SerialInfo::StopBitsType stopBits, SerialInfo::HandshakeType handshake) {
        if (isOpen()) {
#if WIN32
            DWORD size = 0;
            COMMCONFIG newcc;
            memset(&newcc, 0, sizeof(COMMCONFIG));
            newcc.dwSize = sizeof(COMMCONFIG);
            GetCommConfig(_handle, &newcc, &size);

            newcc.dcb.fBinary=true;
            newcc.dcb.fInX=false;
            newcc.dcb.fOutX=false;
            newcc.dcb.fAbortOnError=false;
            newcc.dcb.fNull=false;
            newcc.dcb.fDtrControl = false;
            newcc.dcb.fOutxDsrFlow = false;

            /* setup baudrate */
            switch (baudRate)
            {
                /*Some values are not defined on OSX and *CBR_SD */
            case 256000:
                baudRate = CBR_256000;
                break;
            case 115200:
                baudRate = CBR_115200;
                break;
            case 57600:
                baudRate = CBR_57600;
                break;
            case 38400:
                baudRate = CBR_38400;
                break;
            case 19200:
                baudRate = CBR_19200;
                break;
            case 9600:
                baudRate = CBR_9600;
                break;
            case 4800:
                baudRate = CBR_4800;
                break;
            case 2400:
                baudRate = CBR_2400;
                break;
            case 1200:
                baudRate = CBR_1200;
                break;
            case 600:
                baudRate = CBR_600;
                break;
            case 300:
                baudRate = CBR_300;
                break;
            case 110:
                baudRate = CBR_110;
                break;
            case 50:
            case 0:
            default:
                baudRate = CBR_9600;
                break;
            }
            newcc.dcb.BaudRate = baudRate;

            /* char lenght */
            switch(dataBits)
            {
            case SerialInfo::DATA_5:
                newcc.dcb.ByteSize=5;
                break;
            case SerialInfo::DATA_6:
                newcc.dcb.ByteSize=6;
                break;
            case SerialInfo::DATA_7:
                newcc.dcb.ByteSize=7;
                break;
            case SerialInfo::DATA_8:
            default:
                newcc.dcb.ByteSize=8;
                break;
            }

            /* stopbits */
            switch (stopBits)
            {
                /*one stop bit*/
            default:
            case SerialInfo::STOP_1:
                newcc.dcb.StopBits=ONESTOPBIT;
                break;
                /*1.5 stop bits*/
            case SerialInfo::STOP_1_5:
                newcc.dcb.StopBits=ONE5STOPBITS;
                break;
                /*two stop bits*/
            case SerialInfo::STOP_2:
                newcc.dcb.StopBits=TWOSTOPBITS;
                break;
            }

            newcc.dcb.Parity=(unsigned char)parity;
            switch(_parity)
            {
                /*space parity*/
            default:
            case SerialInfo::PAR_SPACE:
                newcc.dcb.fParity=true;
                break;
                /*mark parity - WINDOWS ONLY*/
            case SerialInfo::PAR_MARK:
                newcc.dcb.fParity=true;
                break;
                /*no parity*/
            case SerialInfo::PAR_NONE:
                newcc.dcb.fParity=false;
                break;
                /*even parity*/
            case SerialInfo::PAR_EVEN:
                newcc.dcb.fParity=true;
                break;
                /*odd parity*/
            case SerialInfo::PAR_ODD:
                newcc.dcb.fParity=true;
                break;
            }

            switch(handshake)
            {
                /*no flow control*/
            default:
            case SerialInfo::FLOW_OFF:
                newcc.dcb.fOutxCtsFlow=false;
                newcc.dcb.fRtsControl=RTS_CONTROL_DISABLE;
                newcc.dcb.fInX=false;
                newcc.dcb.fOutX=false;
                break;

                /*software (XON/XOFF) flow control*/
            case SerialInfo::FLOW_XONXOFF:
                newcc.dcb.fOutxCtsFlow=false;
                newcc.dcb.fRtsControl=RTS_CONTROL_DISABLE;
                newcc.dcb.fInX=true;
                newcc.dcb.fOutX=true;
                break;

            case SerialInfo::FLOW_HARDWARE:
                newcc.dcb.fOutxCtsFlow=true;
                newcc.dcb.fRtsControl=RTS_CONTROL_HANDSHAKE;
                newcc.dcb.fInX=false;
                newcc.dcb.fOutX=false;
                break;
            }

            if(useSignal())
            {
                newcc.dcb.fRtsControl = rtsEnable() ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
                newcc.dcb.fDtrControl = dtrEnable() ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
            }

            SetCommConfig(_handle, &newcc, sizeof(COMMCONFIG));
            return true;
#else
            struct termios newtio;

            if (tcgetattr(_handle, &newtio) == -1)
                return false;

            newtio.c_cflag |= (CLOCAL | CREAD);
            newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);
            newtio.c_oflag &= ~(OPOST);
            newtio.c_iflag = IGNBRK;

            /* setup baudrate */
            switch (baudRate) {
                /*Some values are not defined on OSX and *BSD */
#if defined(B921600)
                case 921600:
                    baudRate = B921600;
                    break;
#endif
#if defined(B460800)
                case 460800:
                    baudRate = B460800;
                    break;
#endif
                case 230400:
                    baudRate = B230400;
                    break;
                case 115200:
                    baudRate = B115200;
                    break;
                case 57600:
                    baudRate = B57600;
                    break;
                case 38400:
                    baudRate = B38400;
                    break;
                case 19200:
                    baudRate = B19200;
                    break;
                case 9600:
                    baudRate = B9600;
                    break;
                case 4800:
                    baudRate = B4800;
                    break;
                case 2400:
                    baudRate = B2400;
                    break;
                case 1800:
                    baudRate = B1800;
                    break;
                case 1200:
                    baudRate = B1200;
                    break;
                case 600:
                    baudRate = B600;
                    break;
                case 300:
                    baudRate = B300;
                    break;
                case 200:
                    baudRate = B200;
                    break;
                case 150:
                    baudRate = B150;
                    break;
                case 134:
                    baudRate = B134;
                    break;
                case 110:
                    baudRate = B110;
                    break;
                case 75:
                    baudRate = B75;
                    break;
                case 50:
                case 0:
                default:
                    baudRate = B9600;
                    break;
            }

            /* char lenght */
            newtio.c_cflag &= ~CSIZE;
            switch (dataBits) {
                case SerialInfo::DATA_5:
                    newtio.c_cflag |= CS5;
                    break;
                case SerialInfo::DATA_6:
                    newtio.c_cflag |= CS6;
                    break;
                case SerialInfo::SerialInfo::DATA_7:
                    newtio.c_cflag |= CS7;
                    break;
                case SerialInfo::DATA_8:
                default:
                    newtio.c_cflag |= CS8;
                    break;
            }

            /* stopbits */
            switch (stopBits) {
                case SerialInfo::STOP_1: /* One */
                    /* do nothing, the default is one stop bit */
                    newtio.c_cflag &= ~CSTOPB;
                    break;
                case SerialInfo::STOP_2: /* Two */
                    newtio.c_cflag |= CSTOPB;
                    break;
                case SerialInfo::STOP_1_5: /* OnePointFive */
                    /* XXX unhandled */
                    break;
            }

            /* parity */
            newtio.c_iflag &= ~(INPCK | ISTRIP);

            switch (parity) {
                case SerialInfo::PAR_NONE: /* None */
                    newtio.c_cflag &= ~(PARENB | PARODD);
                    break;

                case SerialInfo::PAR_ODD: /* Odd */
                    newtio.c_cflag |= PARENB | PARODD;
                    break;

                case SerialInfo::PAR_EVEN: /* Even */
                    newtio.c_cflag &= ~(PARODD);
                    newtio.c_cflag |= (PARENB);
                    break;

                case SerialInfo::PAR_MARK: /* Mark */
                    /* XXX unhandled */
                    break;
                case SerialInfo::PAR_SPACE: /* Space */
                    /* XXX unhandled */
                    break;
            }

            /*handshake*/
            newtio.c_iflag &= ~(IXOFF | IXON);
#ifdef CRTSCTS
            newtio.c_cflag &= ~CRTSCTS;
#endif /* def CRTSCTS */

            switch (handshake) {
                case SerialInfo::FLOW_OFF: /* None */
                    /* do nothing */
                    break;
                case SerialInfo::FLOW_HARDWARE:
#ifdef CRTSCTS
                    newtio.c_cflag |= CRTSCTS;
#endif /* def CRTSCTS */
                    break;
                case SerialInfo::FLOW_XONXOFF:
                    newtio.c_iflag |= IXOFF | IXON;
                    break;
            }

            if (useSignal()) {
                int status;
                ioctl(_handle, TIOCMGET, &status);
                if (dtrEnable()) {
                    status |= TIOCM_DTR;
                } else {
                    status &= ~TIOCM_DTR;
                }
                if (rtsEnable()) {
                    status |= TIOCM_RTS;
                } else {
                    status &= ~TIOCM_RTS;
                }
                ioctl(_handle, TIOCMSET, &status);
            }

            if (cfsetospeed(&newtio, baudRate) < 0 || cfsetispeed(&newtio, baudRate) < 0 ||
                tcsetattr(_handle, TCSANOW, &newtio) < 0) {
                return false;
            } else {
                return true;
            }
#endif
        }
        return false;
    }
}
