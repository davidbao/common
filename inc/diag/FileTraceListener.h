//
//  FileTraceListener.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef FileTraceListener_h
#define FileTraceListener_h

#include "thread/Mutex.h"
#include "system/Singleton.h"
#include "thread/Timer.h"
#include "xml/XmlTextReader.h"
#include "data/String.h"
#include "data/DateTime.h"
#include "diag/TraceListener.h"
#include "IO/FileStream.h"

using namespace Xml;

namespace Diag {
    class FileTraceListenerContext : public TraceListenerContext {
    public:
        String path;
        int reservationDays;        // unit: days
        int diskFullSize;           // unit: M
        String prefix;
        String suffix;
        String extName;             // with .
        int deleteUnusedFilesHour;  // 0-23

        FileTraceListenerContext();

        FileTraceListenerContext(const FileTraceListenerContext &context);

        explicit FileTraceListenerContext(const String &path,
                                          const String &prefix = String::Empty,
                                          const String &suffix = String::Empty);

        bool equals(const TraceListenerContext &other) const override;

        void evaluates(const TraceListenerContext &other) override;

        FileTraceListenerContext &operator=(const FileTraceListenerContext &other);

        void read(XmlTextReader &reader, const String &localName = "log");
    };

    class FileTraceListener : public TraceListener {
    public:
        explicit FileTraceListener(const FileTraceListenerContext &config);

        explicit FileTraceListener(const String &path = "logs",
                                   const String &prefix = String::Empty,
                                   const String &suffix = String::Empty);

        ~FileTraceListener() override;

        const FileTraceListenerContext &context() const;

        bool parseLogFileName(const String &logFileName, DateTime &date) const;

    protected:
        void write(const String &message, const String &category) override;

    protected:
        void processProc();

    private:
        bool createFile(const String &logPath);

        bool createFile(const DateTime &time);

        bool createFile(const DateTime &time, const String &logPath);

        void deleteUnusedFiles();

        void deleteFiles(int days = 30);

        void deleteFiles(const DateTime &time);

        void updateMessageCount(const String &category = String::Empty);

        void flushInner(bool locked = true);

        bool isDiskFull() const;

        void removeFile(const String &dir, const String &fileName, int days);

        bool fileOpened() const;

        bool isCurrentDate() const;

    private:
        static String getLogPath();

        static bool isRealPath(const String &path);

    private:
        String _fullFileName;
        DateTime _currentFileTime;

        FileStream *_file;

        int _messageCount;

        String _message;
        Mutex _messageMutex;

        bool _diskIsFull;

        Timer *_processTimer;

        FileTraceListenerContext _context;

    private:
        static const int MaxMessageCount = 10;
    };
}

#endif // FileTraceListener_h
