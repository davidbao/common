//
//  SummerApplicationTest.cpp
//  common
//
//  Created by baowei on 2022/10/25.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "microservice/SummerApplication.h"
#include "IO/Directory.h"

using namespace IO;
using namespace Microservice;

void cleanUp() {
    String rootPath;
    {
        SummerApplication app;
        rootPath = SummerApplication::instance()->rootPath();
    }
    if(Directory::exists(rootPath)) {
        Directory::deleteDirectory(rootPath);
    }
}

bool testConstructor() {
    SummerApplication app;

    return true;
}

int main() {
    if(!testConstructor()) {
        return 1;
    }

    cleanUp();

    return 0;
}