//
//  FileTraceListenerTest.cpp
//  common
//
//  Created by baowei on 2022/9/27.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "diag/Trace.h"
#include "diag/FileTraceListener.h"
#include "IO/Directory.h"

using namespace Data;
using namespace Diag;

bool testLog1() {
    static const String logStr = "logs";
    static const String logExtName = ".log";
    FileTraceListener* listener = new FileTraceListener(logStr);
    Trace::addTraceListener(listener);
    Trace::write("test1");
    Trace::removeTraceListener(listener);
    delete listener;

    String logPath = Path::combine(Directory::getAppPath(), logStr);
    DateTime now = DateTime::now();
    String fileName = Path::combine(logPath, String::format("%s%s", now.toString("d").c_str(), logExtName.c_str()));
    if(File::exists(fileName)) {
        String text = File::readAllText(fileName);
        Directory::deleteDirectory(logPath);
        if(text.find("test1") > 0) {
            return true;
        }
    }
    Directory::deleteDirectory(logPath);
    return false;
}

int main() {
    if(!testLog1()) {
        return 1;
    }
    return 0;
}
