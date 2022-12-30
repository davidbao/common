//
//  EnvironmentTest.cpp
//  common
//
//  Created by baowei on 2022/10/24.
//  Copyright © 2022 com. All rights reserved.
//

#include "system/Environment.h"
#include "diag/Trace.h"

using namespace Diag;
using namespace System;

#ifdef WIN32
static const char* TestName = "PATHEXT";
static const char* PathContent = "EXE";
#elif __EMSCRIPTEN__
static const char* TestName = "PATH";
static const char* PathContent = "/";
#else
static const char* TestName = "PATH";
static const char* PathContent = "/usr/bin";
#endif

bool testGetVariables() {
    StringMap variables;
    Environment::getVariables(variables);
    StringArray keys;
    variables.keys(keys);
    for (uint32_t i = 0; i < keys.count(); i++) {
        const String &key = keys[i];
        String value;
        if (variables.at(key, value)) {
            Debug::writeFormatLine("%s=%s", key.c_str(), value.c_str());
        }
    }
    if(!variables.contains(TestName)) {
        return false;
    }
    return true;
}

bool testGetVariable() {
    String variable;
    if(!Environment::getVariable(TestName, variable)) {
        return false;
    }
    Debug::writeLine(variable);
    if(variable.find(PathContent) < 0) {
        return false;
    }

    return true;
}

bool testSetVariable() {
    static const char* Name = "Common_Test_Variable";

    String value = "abc123";
    if(!Environment::setVariable(Name, value)) {
        return false;
    }
    String variable;
    if(!Environment::getVariable(Name, variable)) {
        return false;
    }
    if(value != variable) {
        return false;
    }

    String value2 = "abc123.def456";
    if(!Environment::setVariable(Name, value2, true)) {
        return false;
    }
    String variable2;
    if(!Environment::getVariable(Name, variable2)) {
        return false;
    }
    if(value2 != variable2) {
        return false;
    }

    String value3 = "abc123.def456.ghi789";
    if(Environment::setVariable(Name, value3, false)) {
        return false;
    }
    String variable3;
    if(!Environment::getVariable(Name, variable3)) {
        return false;
    }
    if(value3 == variable3) {
        return false;
    }
    if(value2 != variable3) {
        return false;
    }

    return true;
}

bool testRemoveVariable() {
    static const char* Name = "Common_Test_Variable";

    String value = "abc123";
    if(!Environment::setVariable(Name, value)) {
        return false;
    }
#ifdef WIN32
    // Do not support remove a variable in windows.
#else
    if(!Environment::removeVariable(Name)) {
        return false;
    }
    String variable;
    if(Environment::getVariable(Name, variable)) {
        return false;
    }
#endif

    return true;
}

int main() {
    if(!testGetVariables()) {
        return 1;
    }
    if(!testGetVariable()) {
        return 2;
    }
    if(!testSetVariable()) {
        return 3;
    }
    if(!testRemoveVariable()) {
        return 4;
    }

    return 0;
}