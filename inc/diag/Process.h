//
//  Process.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Process_h
#define Process_h

#include "data/String.h"
#include "data/PList.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "system/Delegate.h"

using namespace Data;
using namespace System;

namespace Diag {
    class Process;

    typedef PList<Process> Processes;

    class ProcessOutputEventArgs : public EventArgs {
    public:
        explicit ProcessOutputEventArgs(const String &message);

        ProcessOutputEventArgs(const ProcessOutputEventArgs &other);

        ~ProcessOutputEventArgs() override;

        String message;
    };

    class Process {
    public:
        Process();

        explicit Process(int processId);

        ~Process();

        int id() const;

        const String &name() const;

        const String &stdoutStr() const;

        bool redirectStdout() const;

        void setRedirectStdout(bool redirectStdout = true);

        TimeSpan waitingTimeout() const;

        void setWaitingTimeout(int milliseconds);

        void setWaitingTimeout(const TimeSpan &timeout = TimeSpan::Zero);

        bool kill();

        bool stop(int milliseconds = 10 * 1000);

        bool closeMainWindow() const;

        void *mainWindow() const;

        bool exist() const;

        bool waitForExit(int milliseconds = -1) const;

        Delegates *outputDelegates();

        int exitCode() const;

    public:
        static bool start(const String &fileName, const String &arguments = String::Empty, Process *process = nullptr);

        static bool start(const String &fileName, Process *process);

        static bool startByCmdString(const String &cmdString);

        static bool getProcessById(int processId, Process &process);

        static bool getProcessByName(const String &processName, Processes &processes);

        static bool waitForProcessExit(int processId, int milliseconds = -1);

        static bool getCurrentProcess(Process &process);

        static int getCurrentProcessId();

        static void killAll(const String &processName);

    private:
#ifdef WIN32

        static void enableDebugPriv();

#else

        static int readWithTimeout(int handle, char *data, uint32_t maxlen, uint32_t timeout);

#endif

#ifdef DEBUG
    public:
        bool showChildLog;
#endif

    private:
        int _id;
        String _name;

        bool _redirectStdout;
        String _stdoutStr;

        int _waiting;

        uint64_t _startTick;
        DateTime _startTime;

        Delegates _outputDelegates;

        int _exitCode;
    };
}

#endif // Process_h

