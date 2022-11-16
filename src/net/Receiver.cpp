#include "net/Receiver.h"
#include "data/ByteArray.h"
#include "exception/Exception.h"
#include "thread/TickTimeout.h"
#include "system/Math.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "system/Application.h"

namespace Net
{
    Receiver::Receiver()
    {
    }
    Receiver::~Receiver()
    {
    }

    ssize_t Receiver::receive(size_t count, ByteArray& buffer)
    {
#ifdef DEBUG
        Stopwatch sw("Receiver::receive", 1000);
#endif
        if(count > 0 && count <= 20 * 1024 * 1024)  // 20M
        {
            buffer.clear();
            uint8_t* temp = new uint8_t[count];
            memset(temp, 0, count);
            ssize_t length = 0, totalLength = 0;

            do
            {
                length = receive(temp, 0, count);
                if(length > 0 && length <= count)
                {
                    buffer.addRange(temp, length);
                    totalLength += length;
                }
            }while(length > 0 && totalLength < count);

            delete[] temp;
            return totalLength;
        }
        return -1;
    }

    ssize_t Receiver::receiveBySize(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint timeout, const EscapeOption* escape)
    {
        if (buffer == nullptr)
        {
            return 0;
//            throw ArgumentNullException("buffer");
        }
        if (offset < 0)
        {
            return 0;
//            throw ArgumentOutOfRangeException("offset", "Receiver::receiveBySize, Non-negative number required.");
        }
        if (count < 0)
        {
            return 0;
//            throw ArgumentOutOfRangeException("count", "Receiver::receiveBySize, Non-negative number required.");
        }
        
        if (connected())
        {
            if (escape != nullptr)
            {
                return receiveBySizeWithEscape(buffer, bufferLength, offset, count, timeout, escape);
            }
            return receiveBySizeWithoutEscape(buffer, bufferLength, offset, count, timeout);
        }
        return 0;
    }
    ssize_t Receiver::receiveBySize(uint8_t* buffer, size_t bufferLength, size_t size, uint timeout)
    {
        return receiveBySize(buffer, bufferLength, 0, size, timeout);
    }
    ssize_t Receiver::receiveBySize(uint8_t* buffer, size_t bufferLength, size_t size, const TimeSpan& timeout)
    {
        return receiveBySize(buffer, bufferLength, 0, size, (uint)timeout.totalMilliseconds());
    }
    ssize_t Receiver::receiveBySize(ByteArray* buffer, size_t count, uint timeout, const EscapeOption* escape)
    {
        if (count < 0)
        {
            return 0;
//            throw ArgumentOutOfRangeException("count", "Receiver::receiveBySize, Non-negative number required.");
        }
        
        if (connected())
        {
            if (escape != nullptr)
            {
                throw NotImplementedException("receiveBySizeWithEscape with ByteArray.");
                // return receiveBySizeWithEscape(buffer, count, timeout, escape);
            }
            return receiveBySizeWithoutEscape(buffer, count, timeout);
        }
        return 0;
    }
    ssize_t Receiver::receiveBySize(ByteArray* buffer, size_t count, const TimeSpan& timeout, const EscapeOption* escape)
    {
        return receiveBySize(buffer, count, (uint)timeout.totalMilliseconds(), escape);
    }

    ssize_t Receiver::receiveByEndBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* endBuffer, size_t ebLength, int suffix, uint timeout)
    {
        if (buffer == nullptr)
        {
            throw ArgumentNullException("buffer");
        }
        if (endBuffer == nullptr)
        {
            throw ArgumentNullException("endBuffer");
        }
        
        if(!useReceiveTimeout())
        {
            uint received = 0;
            if (connected())
            {
                if (buffer == nullptr || endBuffer == nullptr)
                    return received;
                
                bool bReceiveEndBytes = false;
                int nSuffix = 0, nStartByte = 0;
                uint startTime = TickTimeout::getCurrentTickCount();
                uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);
                do
                {
                    if (bReceiveEndBytes && nSuffix >= suffix)
                        break;
                    size_t nBytesToRead = this->available();
                    if (nBytesToRead <= 0)
                    {
                        Thread::msleep(1);
                        if (TickTimeout::isTimeout(startTime, deadTime))
                            break;
                        continue;
                    }
                    if (received + 1 <= (uint)bufferLength)
                    {
                        received += this->receive(buffer, received, 1);
                    }
                    else
                    {
                        return received;
                    }
                    
                    if (!bReceiveEndBytes)
                    {
//#ifdef DEBUG
//                        if (received - 1 > (uint)bufferLength || nStartByte > ebLength)
//                        {
//                            Debug::writeFormatLine("nStartByte: %d, received: %d", nStartByte, received);
//                            ByteArray sBuffer(buffer, received);
//                            Debug::writeLine(sBuffer.toString());
//                        }
//#endif
                        if (buffer[received - 1] == endBuffer[nStartByte])
                        {
                            nStartByte++;
                            if (nStartByte == ebLength)
                            {
                                bReceiveEndBytes = true;
                            }
                        }
                        else
                        {
                            if (nStartByte == 0)
                            {
                                continue;
                            }
                            
                            nStartByte = 0;
                            if (buffer[received - 1] == endBuffer[nStartByte])
                            {
                                nStartByte++;
                            }
                        }
                    }
                    else
                    {
                        nSuffix++;
                    }
                    
                    deadTime = TickTimeout::getDeadTickCount(timeout);
                } while (true);
                return received;
            }
            return received;
        }
        else
        {
            uint received = 0;
            if (connected())
            {
                if (buffer == nullptr || endBuffer == nullptr)
                    return received;
                
                bool bReceiveEndBytes = false;
                int nSuffix = 0, nStartByte = 0;
                do
                {
                    if (bReceiveEndBytes && nSuffix >= suffix)
                        break;

                    int readlen = this->receive(buffer, received, 1, timeout);
                    if(readlen <= 0)
                    {
                        return 0;
                    }
                    if (received + 1 <= (uint)bufferLength)
                    {
                        received += readlen;
                    }
                    else
                    {
                        return received;
                    }
                    
                    if (!bReceiveEndBytes)
                    {
                        if (buffer[received - 1] == endBuffer[nStartByte])
                        {
                            nStartByte++;
                            if (nStartByte == ebLength)
                            {
                                bReceiveEndBytes = true;
                            }
                        }
                        else
                        {
                            if (nStartByte == 0)
                            {
                                continue;
                            }
                            
                            nStartByte = 0;
                            if (buffer[received - 1] == endBuffer[nStartByte])
                            {
                                nStartByte++;
                            }
                        }
                    }
                    else
                    {
                        nSuffix++;
                    }
                } while (true);
                return received;
            }
            return received;
        }
    }

    ssize_t Receiver::receiveByEndBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* startBuffer, size_t sbLength, const uint8_t* endBuffer, size_t ebLength, int suffix, uint timeout)
    {
        if (buffer == nullptr)
        {
            throw ArgumentNullException("buffer");
        }
        if (startBuffer == nullptr)
        {
            throw ArgumentNullException("startBuffer");
        }
        if (endBuffer == nullptr)
        {
            throw ArgumentNullException("endBuffer");
        }
        
        if (receiveStartBytes(startBuffer, sbLength, timeout))
        {
            size_t startCount = sbLength;
            for (size_t i = 0; i < startCount && i < bufferLength; i++)
            {
                buffer[i] = startBuffer[i];
            }
            if (bufferLength > startCount)
            {
                uint8_t* buffer2 = new uint8_t[bufferLength - startCount];
                memset(buffer2, 0, bufferLength - startCount);
                size_t count = receiveByEndBytes(buffer2, bufferLength - startCount, endBuffer, ebLength, suffix, timeout);
                memcpy(buffer + startCount, buffer2, count);
                delete[] buffer2;
                
                return count + startCount;
            }
            return bufferLength;
        }
        return 0;
    }

    ssize_t Receiver::readLine(uint8_t* buffer, size_t bufferLength, uint timeout, const char* newLine)
    {
        if(newLine == nullptr)
            return 0;
        return receiveByEndBytes(buffer, bufferLength, (const uint8_t*)newLine, (int)strlen(newLine), 0, timeout);
    }

    ssize_t Receiver::getLengthByEndBytes(const ByteArray& endBuffer, uint timeout)
    {
        ssize_t received = 0;
        if (connected())
        {
            if (endBuffer.count() == 0)
                return 0;
            
            uint startTime = TickTimeout::getCurrentTickCount();
            uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);
            do
            {
                if (TickTimeout::isTimeout(startTime, deadTime))
                    break;

                size_t nBytesToRead = this->available();
                if (nBytesToRead > 0)
                {
                    ByteArray tempBuffer;
                    receiveBySizeWithoutEscape(&tempBuffer, nBytesToRead, timeout);
                    //                Debug::writeFormatLine("tempBuffer, count: %d, buffer: %s", tempBuffer.count(), tempBuffer.toString().c_str());
                    received = tempBuffer.find(endBuffer);
                    if(received > 0)
                    {
                        received += endBuffer.count();
                        break;
                    }
                }
                
                Thread::msleep(10);
            } while (true);
        }
        return received;
    }
    ssize_t Receiver::getLengthByLine(uint timeout, const char* newLine)
    {
        if(newLine == nullptr)
            return 0;
        
        ByteArray newLineArray((const uint8_t*)newLine, (uint)strlen(newLine));
        return getLengthByEndBytes(newLineArray, timeout);
    }
    
    bool Receiver::receiveByte(uint8_t& data, uint timeout)
    {
        uint8_t buffer[8];
        memset(buffer, 0, sizeof(buffer));
        bool result = (receiveBySize(buffer, sizeof(buffer), 1, timeout) == 1);
        data = buffer[0];
        return result;
    }
    
    bool Receiver::receiveStartBytes(const uint8_t* startBuffer, size_t sbLength, uint timeout)
    {
        if (startBuffer == nullptr)
        {
            throw ArgumentNullException("startBuffer");
        }
        
        uint8_t buffer[1024];
        return receiveStartBytes(buffer, sizeof(buffer), startBuffer, sbLength, timeout);
    }
    
    bool Receiver::receiveStartBytes(uint8_t* buffer, size_t bufferLength, const uint8_t* startBuffer, size_t sbLength, uint timeout)
    {
        if (buffer == nullptr)
        {
            throw ArgumentNullException("buffer");
        }
        if (startBuffer == nullptr)
        {
            throw ArgumentNullException("startBuffer");
        }
        
        uint nReadStartCount = (uint)receiveByEndBytes(buffer, bufferLength, startBuffer, sbLength, 0, timeout);
        return nReadStartCount >= (uint)sbLength;
    }

    ssize_t Receiver::receiveBySizeWithEscape(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint timeout, const EscapeOption* escape)
    {
        if(!useReceiveTimeout())
        {
            uint startTime = TickTimeout::getCurrentTickCount();
            uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);

            size_t targetLength = escape->escapeBuffer[0] == 0 ? 0 : escape->escapeLength;
            size_t adjustCount = targetLength - (escape->toEscapeBuffer[0] == 0 ? 0 : escape->toEscapeLength);
            ssize_t received = 0;
            uint startByte = 0;
            do
            {
                size_t available = this->available();
                if (available <= 0)
                {
                    Thread::msleep(1);
                    if (TickTimeout::isTimeout(startTime, deadTime))
                        break;
                    continue;
                }
                if (received + 1 <= bufferLength)
                {
                    received += this->receive(buffer, received + offset, 1);
                }
                else
                {
                    return received;
                }

                if (buffer[received + offset - 1] == escape->toEscapeBuffer[startByte])
                {
                    startByte++;
                    if (startByte >= (uint)escape->toEscapeLength)
                    {
                        if (escape->escapeBuffer[0] != 0)
                        {
                            for (size_t i = 0; i < targetLength; i++)
                            {
                                buffer[received - escape->toEscapeLength + i + offset] = escape->escapeBuffer[i];
                            }
                        }
                        received += adjustCount;
                        
                        startByte = 0;
                    }
                }
                else
                {
                    startByte = 0;
                }
                if (received >= count && startByte <= 0)
                {
                    return received;
                }
                deadTime = TickTimeout::getDeadTickCount(timeout);
            } while (true);
            return received;
        }
        else
        {
            size_t targetLength = escape->escapeBuffer[0] == 0 ? 0 : escape->escapeLength;
            size_t adjustCount = targetLength - (escape->toEscapeBuffer[0] == 0 ? 0 : escape->toEscapeLength);
            ssize_t received = 0;
            uint startByte = 0;
            do
            {
                ssize_t readlen = this->receive(buffer, received + offset, 1, timeout);
                if(readlen <= 0)
                {
                    return 0;
                }
                if (received + 1 <= bufferLength)
                {
                    received += readlen;
                }
                else
                {
                    return received;
                }
                
                if (buffer[received + offset - 1] == escape->toEscapeBuffer[startByte])
                {
                    startByte++;
                    if (startByte >= (uint)escape->toEscapeLength)
                    {
                        if (escape->escapeBuffer[0] != 0)
                        {
                            for (size_t i = 0; i < targetLength; i++)
                            {
                                buffer[received - escape->toEscapeLength + i + offset] = escape->escapeBuffer[i];
                            }
                        }
                        received += adjustCount;
                        
                        startByte = 0;
                    }
                }
                else
                {
                    startByte = 0;
                }
                if (received >= count && startByte <= 0)
                {
                    return received;
                }
            } while (true);
            return received;
        }
    }

    ssize_t Receiver::receiveBySizeWithoutEscape(uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint timeout)
    {
        if(!useReceiveTimeout())
        {
            uint startTime = TickTimeout::getCurrentTickCount();
            uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);

            size_t available = this->available();
            bool dataReady = false;
            do
            {
                if (this->available() >= count)
                {
                    dataReady = true;
                    break;
                }
                uint now = TickTimeout::getCurrentTickCount();
                if (this->available() == available)
                {
                    Thread::msleep(1);
                    if (TickTimeout::isTimeout(startTime, deadTime, now))
                        break;
                }
                else
                {
                    available = this->available();
                    deadTime = TickTimeout::getDeadTickCount(now, timeout);
                }
            } while (true);
            
            if (this->available() > 0)
            {
                return dataReady
                ? this->receive(buffer, offset, count)
                : this->receive(buffer, offset, Math::min(this->available(), bufferLength));
            }
            return 0;
        }
        else
        {
            return this->receive(buffer, offset, count, timeout);
        }
    }

    ssize_t Receiver::receiveBySizeWithoutEscape(ByteArray* buffer, size_t count, uint timeout)
    {
        if(buffer == nullptr)
            return 0;
        
        if(!useReceiveTimeout())
        {
            uint startTime = TickTimeout::getCurrentTickCount();
            uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);
            
            uint8_t temp[BufferLength];
            memset(temp, 0, sizeof(temp));
            size_t readCount = 0;
            size_t totalCount = 0;
            size_t singleReceiveCount = 0;
            do
            {
                size_t available = this->available();
                if(available > 0)
                {
                    singleReceiveCount = Math::min(count-totalCount, BufferLength);
                    singleReceiveCount = Math::min(singleReceiveCount, available);
                    readCount = this->receive(temp, 0, singleReceiveCount);

                    if(readCount > 0)
                    {
                        buffer->addRange(temp, readCount);
                        totalCount += readCount;
                    }
                }
                
                if(TickTimeout::isTimeout(startTime, deadTime))
                    break;
                
                Thread::msleep(1);
            }while(readCount >= 0 && totalCount < count);
            return totalCount == count ? totalCount : 0;
        }
        else
        {
            return this->receive(buffer, count, timeout);
        }
        return 0;
    }
    ssize_t Receiver::receiveDirectly(ByteArray* buffer, size_t count)
    {
        if(buffer == nullptr)
            return 0;
        
        uint8_t temp[BufferLength];
        memset(temp, 0, sizeof(temp));
        ssize_t readCount = 0;
        ssize_t totalCount = 0;
        do
        {
            readCount = this->receive(temp, 0, Math::min(count-(size_t)totalCount, BufferLength));
            if(readCount > 0)
            {
                buffer->addRange(temp, readCount);
                totalCount += readCount;
            }
        }while(readCount > 0 && totalCount < count);
        return totalCount;
    }
    
    void Receiver::clearReceiveBuffer()
    {
        if(!useReceiveTimeout())
        {
            size_t available = this->available();
            if (available > 0)
            {
#ifdef DEBUG
                Debug::writeFormatLine("clearReceiveBuffer, available: %d", available);
#endif
                uint8_t* buffer = new uint8_t[available];
                receiveBySize(buffer, available, available, 1000);
                delete[] buffer;
            }
        }
        else
        {
            uint8_t buffer[128];
            receiveBySizeWithoutEscape(buffer, sizeof(buffer), 0, sizeof(buffer), 2);    // ms
        }
    }
}
