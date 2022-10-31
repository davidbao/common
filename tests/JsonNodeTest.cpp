//
//  JsonNodeTest.cpp
//  common
//
//  Created by baowei on 2022/10/21.
//  Copyright © 2022 com. All rights reserved.
//

#include "json/JsonNode.h"
#include "diag/Trace.h"

using namespace Common;

bool testToString() {
    JsonNode node;
    node.add(JsonNode("test1", 1));
    node.add(JsonNode("test2", "abc"));
    node.add(JsonNode("test3", true));
    String str = node.toString();
    Trace::writeLine(str);
    if(str != "{\"test1\":1,\"test2\":\"abc\",\"test3\":true}") {
        return false;
    }
    return true;
}

int main() {
    if(!testToString()) {
        return 1;
    }
    return 0;
}