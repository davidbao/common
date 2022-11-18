//
//  FileTraceListener.cpp
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//
#include "diag/FileTraceListener.h"
#include "diag/Trace.h"
#include "IO/Metrics.h"
#include "system/Resources.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "data/DateTime.h"

namespace Common {
    FileTraceListenerContext::FileTraceListenerContext() : FileTraceListenerContext("logs") {
    }

    FileTraceListenerContext::FileTraceListenerContext(const FileTraceListenerContext &context)
            : FileTraceListenerContext() {
        this->operator=(context);
    }

    FileTraceListenerContext::FileTraceListenerContext(const String &path, const char *prefix, const char *suffix) {
        this->path = path;
#ifdef __arm_linux__
        this->reservationDays = 30;        // 30 days
#else
        this->reservationDays = 365;       // 365 days
#endif
        this->diskFullSize = 10;           // 10 M
        this->prefix = prefix;
        this->suffix = suffix;
        this->extName = ".log";
        this->deleteUnusedFilesHour = 1;   // 1 AM
    }

    void FileTraceListenerContext::operator=(const FileTraceListenerContext &context) {
        this->path = context.path;
        this->reservationDays = context.reservationDays;
        this->diskFullSize = context.diskFullSize;
        this->prefix = context.prefix;
        this->suffix = context.suffix;
        this->extName = context.extName;
        this->deleteUnusedFilesHour = context.deleteUnusedFilesHour;
    }

    void FileTraceListenerContext::read(XmlTextReader &reader, const String &localName) {
        if (reader.nodeType() == XmlNodeType::Element &&
            localName == reader.localName()) {
            Boolean::parse(reader.getAttribute("enable"), enable);
            String ph = reader.getAttribute("path");
#ifdef WIN32
            ph = String::UTF8toGBK(ph);
#endif
            if (!ph.isNullOrEmpty()) {
                path = ph;
            }
            int rd;
            if (Int32::parse(reader.getAttribute("reservationDays"), rd) &&
                rd >= 1 && rd <= 3650) {
                reservationDays = rd;
            }
            int dfs;
            if (Int32::parse(reader.getAttribute("diskFullSize"), dfs) &&
                dfs >= 5 && dfs <= 100) {
                diskFullSize = dfs;
            }
            String p = reader.getAttribute("prefix");
            if (!p.isNullOrEmpty()) {
                prefix = p;
            }
            String s = reader.getAttribute("suffix");
            if (!s.isNullOrEmpty()) {
                suffix = s;
            }
            String e = reader.getAttribute("extName");
            if (!e.isNullOrEmpty() && e[0] == '.') {
                extName = e;
            }
            int hour;
            if (Int32::parse(reader.getAttribute("deleteUnusedFilesHour"), hour) &&
                hour >= 0 && hour <= 23) {
                deleteUnusedFilesHour = hour;
            }
        }
    }

    void processProc(void *parameter) {
        FileTraceListener *listener = (FileTraceListener *) parameter;
        assert(listener);
        listener->processProcInner();
    }

    void deleteUnusedFilesAction(void *parameter) {
        FileTraceListener *listener = (FileTraceListener *) parameter;
        assert(listener);
        listener->deleteUnusedFiles();
    }

    FileTraceListener::FileTraceListener(const FileTraceListenerContext &context) {
        _messageCount = 0;
        _diskIsFull = false;
        _fullFileName = String::Empty;
        _currentFileTime = DateTime::now();
        _message = String::Empty;
        _processThread = nullptr;

        _context = context;

        if (!isRealPath(_context.path.c_str())) {
            String logPath = getLogPath();
#ifdef WIN32
            const char *fmt = "%s\\%s";
#else
            const char* fmt = "%s/%s";
#endif
            _context.path = String::convert(fmt, logPath.c_str(), _context.path.c_str());
        }

        if (_context.enable) {
            createFile(_context.path.c_str());

            _processThread = new Thread("FileTraceListenerProc");
            uint32_t interval = 60 * 1000;        // 60 seconds.
            _processThread->startProc(processProc, this, interval, deleteUnusedFilesAction, this);
        }
    }

    FileTraceListener::FileTraceListener(const char *logPath, const char *prefix, const char *suffix)
            : FileTraceListener(FileTraceListenerContext(logPath, prefix, suffix)) {
    }

    FileTraceListener::FileTraceListener(const String &logPath, const char *prefix, const char *suffix)
            : FileTraceListener(logPath.c_str(), prefix, suffix) {
    }

    FileTraceListener::~FileTraceListener() {
        if (_processThread != nullptr) {
            _processThread->stop();
            delete _processThread;
            _processThread = nullptr;
        }

        if (_file != nullptr) {
            flushInner();
            _file->close();
            delete _file;
        }
    }

    const FileTraceListenerContext &FileTraceListener::config() const {
        return _context;
    }

    bool FileTraceListener::parseLogFileName(const String &logFileName, DateTime &date) const {
        String name = logFileName;
        if (!_context.prefix.isNullOrEmpty()) {
            name.replace(_context.prefix, String::Empty);
        }
        if (!_context.suffix.isNullOrEmpty()) {
            name.replace(_context.suffix, String::Empty);
        }
        name.replace(_context.extName, String::Empty);
        return DateTime::parse(name, date);
    }

    const String FileTraceListener::getLogPath() const {
        return Directory::getAppPath();
    }

    bool FileTraceListener::isRealPath(const char *path) const {
        return Path::isPathRooted(path);
    }

    void FileTraceListener::write(const char *message, const char *category) {
        if (!_context.enable)
            return;

        Locker locker(&_messageMutex);

        if (message == nullptr || strlen(message) == 0)
            return;

        if (_diskIsFull)
            return;

        _message.append(message);

        updateMessageCount(category);
    }

    void FileTraceListener::updateMessageCount(const char *category) {
        _messageCount++;

        if (category != nullptr &&
            (strcmp(category, Trace::Error) == 0 ||
             strcmp(category, Trace::Fatal) == 0)) {
            flushInner(false);
        } else {
            if (_messageCount >= MaxMessageCount) {
                flushInner(false);
            }
        }
    }

    void FileTraceListener::flushInner(bool locked) {
        if (!isCurrentDate()) {
            if (fileOpened()) {
                _file->close();
                delete _file;
                _file = nullptr;
            }
            createFile(_context.path.c_str());
        }

        if (!fileOpened())
            return;

        if (locked) {
            _messageMutex.lock();
        }

        if (_messageCount > 0) {
            _messageCount = 0;

            size_t length = _message.length();
            _file->write((const uint8_t *) _message.c_str(), 0, length);
            _message.empty();
        }

        if (locked) {
            _messageMutex.unlock();
        }
    }

    bool FileTraceListener::isDiskFull() const {
        const int MaxSize = _context.diskFullSize * 1024 * 1024;    // M
        return DiskStat::isDiskFull(_context.path, MaxSize);
    }

    void FileTraceListener::processProcInner() {
        flushInner();

        DateTime now = DateTime::now();
        // Try to create new log file.
        createFile(now);

        // Try to delete some log files at 1 AM.
        deleteFiles(now);

        _diskIsFull = isDiskFull();
    }

    void FileTraceListener::deleteUnusedFiles() {
        // Remove all log files except today's if disk is full.
        deleteFiles(isDiskFull() ? 1 : _context.reservationDays);
    }

    void FileTraceListener::deleteFiles(const DateTime &time) {
        if (time.hour() == _context.deleteUnusedFilesHour && time.minute() == 0) {
            deleteUnusedFiles();
        }
    }

    void FileTraceListener::deleteFiles(int days) {
        String filter = String::convert("*%s", _context.extName.c_str());
        if (Directory::exists(_context.path)) {
            StringArray fileNames;
            Directory::getFiles(_context.path, filter, SearchOption::TopDirectoryOnly, fileNames);
            for (uint32_t i = 0; i < fileNames.count(); ++i) {
                const String &fileName = fileNames[i];
                String name = Path::getFileName(fileName);
                removeFile(_context.path, name, days);
            }
        }
    }

    void FileTraceListener::removeFile(const String &dir, const String &fileName, int days) {
        String name = fileName;
        String currentFileName = Path::getFileName(_fullFileName);
        if (String::compare(currentFileName, fileName, true) != 0) {
            DateTime now = DateTime::now();
            if (now.year() < 2000)
                return;

            if (!_context.prefix.isNullOrEmpty()) {
                name.replace(_context.prefix, String::Empty);
            }
            if (!_context.suffix.isNullOrEmpty()) {
                name.replace(_context.suffix, String::Empty);
            }
            name.replace(_context.extName, String::Empty);
            DateTime fileDate;
            if (DateTime::parse(name, fileDate)) {
                TimeSpan diff = now - fileDate;
                if (diff > TimeSpan::fromDays(days)) {
                    String fullFileName = Path::combine(dir, fileName);
                    if (File::deleteFile(fullFileName)) {
                        Trace::writeFormatLine(Resources::getString("RemoveLogFilesSuccessfully").c_str(),
                                               fileName.c_str());
                    } else {
                        Trace::writeFormatLine(Resources::getString("FailedToRemoveLogFiles").c_str(),
                                               fileName.c_str());
                    }
                }
            }
        }
    }

    bool FileTraceListener::createFile(const DateTime &time) {
        if (time.hour() == 0 && time.minute() == 0) {
            if (fileOpened()) {
                _file->close();
                delete _file;
                _file = nullptr;
            }
            return createFile(_context.path.c_str());
        }
        return true;
    }

    bool FileTraceListener::createFile(const String &logPath) {
        return createFile(DateTime::now(), logPath);
    }

    bool FileTraceListener::createFile(const DateTime &time, const String &logPath) {
        _currentFileTime = time;
        String fileName = String::format("%s%s%s%s", _context.prefix.c_str(),
                                         time.toString("d").c_str(),
                                         _context.suffix.c_str(),
                                         _context.extName.c_str());

        if (!Directory::exists(logPath)) {
            Directory::createDirectory(logPath);
        }
        _fullFileName = Path::combine(logPath, fileName);
        _file = new FileStream(_fullFileName, FileMode::FileCreate, FileAccess::FileWrite);
        return fileOpened();
    }

    bool FileTraceListener::fileOpened() const {
        return _file != nullptr && _file->isOpen();
    }

    bool FileTraceListener::isCurrentDate() const {
        DateTime now = DateTime::now();
        return _currentFileTime.date() == now.date();
    }
}
