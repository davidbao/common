//
//  ResourcesTest.cpp
//  common
//
//  Created by baowei on 2022/9/26.
//  Copyright © 2022 com. All rights reserved.
//

#include "system/Resources.h"
#include "system/Application.h"
#include <locale.h>

using namespace System;

bool testGetString1() {
    Application app;
    app.setCulture(Culture("en"));
    String str = Resources::getString("Error");
    return str == "Error";
};

bool testGetString2() {
    Application app;
    app.setCulture(Culture("zh-CN"));
    String str = Resources::getString("Error");
    return str == "错误";
};

bool testGetString3() {
    Application app;
    app.setCulture(Culture("ca"));
    String str = Resources::getString("Error");
    return str == "Error";
};

bool testGetString4() {
    Application app;
    String str = Resources::getString("Error");
    return str == "Error";
};

int main() {
    if(!testGetString1())
        return 1;
    if(!testGetString2())
        return 2;
    if(!testGetString3())
        return 3;
    if(!testGetString4())
        return 4;

    return 0;
}