//
//  ProcessTest.cpp
//  common
//
//  Created by baowei on 2023/2/17.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "diag/Process.h"

using namespace Diag;

#ifdef WIN32
String ll_cmd = "cmd";
String ll_param = "/c dir";
String ll_cmd_l = "cmd /c dir";
#else
String ll_cmd = "ls";
String ll_param = "";
String ll_cmd_l = "ls";
#endif

bool testArgs() {
    {
        ProcessOutputEventArgs args("abc123");
        if (args.message != "abc123") {
            return false;
        }
    }
    {
        ProcessOutputEventArgs args("abc123");
        ProcessOutputEventArgs args2(args);
        if (args2.message != "abc123") {
            return false;
        }
    }

    return true;
}

bool testConstructor() {
    {
        Process process;
        if (process.id() != -1) {
            return false;
        }
    }
    {
        Process process(1);
        if (process.id() != 1) {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        if (!Process::start(ll_cmd)) {
            return false;
        }
    }
    {
        Process process;
        process.setRedirectStdout(true);
        if (!Process::start(ll_cmd, ll_param, &process)) {
            return false;
        }
        if (process.stdoutStr().isNullOrEmpty()) {
            return false;
        }
    }
    {
        Process process;
        process.setRedirectStdout(true);
        if (!Process::start(ll_cmd, ll_param, &process)) {
            return false;
        }
        if (process.stdoutStr().isNullOrEmpty()) {
            return false;
        }
    }
    {
        if (!Process::startByCmdString(ll_cmd_l)) {
            return false;
        }
    }

    return true;
}

bool testAttributes() {
    {
        Process process;
        if (process.redirectStdout()) {
            return false;
        }
        process.setRedirectStdout(true);
        if (!process.redirectStdout()) {
            return false;
        }
        process.setRedirectStdout(false);
        if (process.redirectStdout()) {
            return false;
        }
    }
    {
        Process process;
        if (process.waitingTimeout() != TimeSpan::Zero) {
            return false;
        }

        int milliseconds = 3000;
        process.setWaitingTimeout(milliseconds);
        if (process.waitingTimeout() != TimeSpan::fromMilliseconds(milliseconds)) {
            return false;
        }

        process.setWaitingTimeout();
        if (process.waitingTimeout() != TimeSpan::Zero) {
            return false;
        }

        process.setWaitingTimeout(TimeSpan::fromMilliseconds(milliseconds));
        if (process.waitingTimeout() != TimeSpan::fromMilliseconds(milliseconds)) {
            return false;
        }
    }
    {
        Process process;
        if (!Process::start(ll_cmd, ll_param, &process)) {
            return false;
        }
        if (process.id() <= 0) {
            return false;
        }
        if (process.name().isNullOrEmpty()) {
            return false;
        }
    }
    {
        Process process;
        process.setRedirectStdout(true);
        static String message;
        auto function = [](void *owner, void *sender, EventArgs *e) {
            auto args = dynamic_cast<ProcessOutputEventArgs *>(e);
            if (args != nullptr) {
                message.append(args->message);
            }
        };
        process.outputDelegates()->add(Delegate(nullptr, function));
        if (!Process::start(ll_cmd, ll_param, &process)) {
            return false;
        }
        if (message.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testGetCurrentProcess() {
    {
        Process process;
        if (!Process::getCurrentProcess(process)) {
            return false;
        }
        if (process.id() <= 0) {
            return false;
        }
        if (process.name().isNullOrEmpty()) {
            return false;
        }
    }
    {
        if (Process::getCurrentProcessId() <= 0) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testArgs()) {
        return 1;
    }
    if (!testConstructor()) {
        return 2;
    }
    if (!testStart()) {
        return 3;
    }
    if (!testAttributes()) {
        return 4;
    }
    if (!testGetCurrentProcess()) {
        return 5;
    }

    return 0;
}