//
//  ConvertTest.cpp
//  common
//
//  Created by baowei on 2023/12/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "data/Convert.h"

using namespace Data;

bool testSplitStr() {
    {
        String str = "abc";
        StringArray parameters;
        Convert::splitStr(str, ' ', parameters, '"', '\'');
        if (parameters.count() != 1) {
            return false;
        }
        if (parameters[0] != "abc") {
            return false;
        }
    }

    {
        String str = "-c -n 127.0.0.1";
        StringArray parameters;
        Convert::splitStr(str, ' ', parameters, '"', '\'');
        if (parameters.count() != 3) {
            return false;
        }
        if (parameters[0] != "-c") {
            return false;
        }
        if (parameters[1] != "-n") {
            return false;
        }
        if (parameters[2] != "127.0.0.1") {
            return false;
        }
    }

    {
        String str = "-c -n 'abc edf'";
        StringArray parameters;
        Convert::splitStr(str, ' ', parameters, '"', '\'');
        if (parameters.count() != 3) {
            return false;
        }
        if (parameters[0] != "-c") {
            return false;
        }
        if (parameters[1] != "-n") {
            return false;
        }
        if (parameters[2] != "abc edf") {
            return false;
        }
    }

    {
        String str = "-c -n \"abc edf\"";
        StringArray parameters;
        Convert::splitStr(str, ' ', parameters, '"', '\'');
        if (parameters.count() != 3) {
            return false;
        }
        if (parameters[0] != "-c") {
            return false;
        }
        if (parameters[1] != "-n") {
            return false;
        }
        if (parameters[2] != "abc edf") {
            return false;
        }
    }

    {
        String str = R"(test_button.py button1 '{"operation_time":"2023-10-20 15:24:33","operation_user":"web","param1":"1","param2":"test","param3":"test2"}')";
        StringArray parameters;
        Convert::splitStr(str, ' ', parameters, '"', '\'');
        if (parameters.count() != 3) {
            return false;
        }
        if (parameters[0] != "test_button.py") {
            return false;
        }
        if (parameters[1] != "button1") {
            return false;
        }
        if (parameters[2] != R"({"operation_time":"2023-10-20 15:24:33","operation_user":"web","param1":"1","param2":"test","param3":"test2"})") {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testSplitStr()) {
        return 1;
    }

    return 0;
}