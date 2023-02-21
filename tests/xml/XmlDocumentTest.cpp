//
//  XmlDocumentTest.cpp
//  common
//
//  Created by baowei on 2022/12/2.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "xml/XmlDocument.h"

using namespace Xml;

bool testConstructor() {
    {
        XmlDocument test;
        if(!test.isValid()) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    return 0;
}