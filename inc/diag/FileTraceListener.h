//
//  FileTraceListener.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef FileTraceListener_h
#define FileTraceListener_h

#include "thread/Mutex.h"
#include "system/Singleton.h"
#include "thread/Thread.h"
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

        FileTraceListenerContext(const String &path, const char *prefix = nullptr, const char *suffix = nullptr);

        void operator=(const FileTraceListenerContext &context);

        void read(XmlTextReader &reader, const String &localName = "log");
    };

    class FileTraceListener : public TraceListener {
    public:
        FileTraceListener(const FileTraceListenerContext &config);

        FileTraceListener(const char *logPath = "logs", const char *prefix = nullptr, const char *suffix = nullptr);

        FileTraceListener(const String &logPath, const char *prefix = nullptr, const char *suffix = nullptr);

        ~FileTraceListener() override;

        const FileTraceListenerContext &config() const;

        bool parseLogFileName(const String &logFileName, DateTime &date) const;

    protected:
        void write(const char *message, const char *category) override;

    private:
        bool createFile(const String &logPath);

        bool createFile(const DateTime &time);

        bool createFile(const DateTime &time, const String &logPath);

        void deleteUnusedFiles();

        void deleteFiles(int days = 30);

        void deleteFiles(const DateTime &time);

        void updateMessageCount(const char *category = nullptr);

        void flushInner(bool locked = true);

        bool isDiskFull() const;

        void removeFile(const String &dir, const String &fileName, int days);

        const String getLogPath() const;

        bool isRealPath(const char *path) const;

        bool fileOpened() const;

        bool isCurrentDate() const;

    protected:
        friend void processProc(void *parameter);

        void processProcInner();

        friend void deleteUnusedFilesAction(void *parameter);

    private:
        String _fullFileName;
        DateTime _currentFileTime;

        FileStream *_file;

        int _messageCount;
        static const int MaxMessageCount = 10;

        String _message;
        Mutex _messageMutex;

        bool _diskIsFull;

        Thread *_processThread;

        FileTraceListenerContext _context;
    };
}

#endif // FileTraceListener_h
