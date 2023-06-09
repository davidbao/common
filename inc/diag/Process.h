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
#include "data/TimeSpan.h"
#include "system/Delegate.h"

using namespace System;

namespace Diag {
    class Process;

    typedef PList<Process> Processes;

    class ProcessOutputEventArgs : public EventArgs {
    public:
        explicit ProcessOutputEventArgs(const String &message) {
            this->message = message;
        }

        ~ProcessOutputEventArgs() override = default;

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

        bool waitingTimeout() const;

        void setWaitingTimeout(int milliseconds = -1);

        void setWaitingTimeout(TimeSpan timeout = TimeSpan::Zero);

        bool kill();

        bool stop(int milliseconds = 10 * 1000);

        bool closeMainWindow() const;

        void *mainWindow() const;

        bool exist() const;

        bool waitForExit(int milliseconds = -1) const;

        Delegates *outputDelegates();

    public:
        static bool start(const String &fileName, const String &arguments = String::Empty, Process *process = nullptr);

        static bool startByCmdString(const String &cmdString);

        static bool getProcessById(int processId, Process &process);

        static bool getProcessByName(const char *processName, Processes &processes);

        static bool getProcessByName(const String &processName, Processes &processes);

        static bool waitForProcessExit(int processId, int milliseconds = -1);

        static bool getCurrentProcess(Process &process);

        static int getCurrentProcessId();

    private:
        static bool isProcessEnded(void *parameter);

#ifdef WIN32

        void enableDebugPriv() const;

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

        Delegates _outputDelegates;
    };
}

#endif // Process_h

