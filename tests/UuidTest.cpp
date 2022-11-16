//
//  UuidTest.cpp
//  common
//
//  Created by baowei on 2022/9/26.
//  Copyright © 2012 com. All rights reserved.
//

#include "data/Uuid.h"
#include "IO/MemoryStream.h"

using namespace Common;

bool testGenerate() {
    Uuid id = Uuid::generate();
    return !id.isEmpty();
}

bool testEmpty1() {
    Uuid id = Uuid::Empty;
    return id.isEmpty();
}

bool testEmpty2() {
    Uuid id = Uuid::generate();
    id.empty();
    return id.isEmpty();
}

bool testParse() {
    Uuid id = Uuid::generate();
    String idStr = id.toString();
    Uuid id2;
    if (!Uuid::parse(idStr, id2))
        return false;
    return id == id2;
}

bool testEquals() {
    Uuid id, id2;
    Uuid::parse("08967256-661D-4C25-8449-47B6C632B1F9", id);
    Uuid::parse("08967256-661D-4C25-8449-47B6C632B1F9", id2);
    return !id.isEmpty() && !id2.isEmpty() && id2 == id;
}

bool testGreaterThan() {
    Uuid id, id2;
    Uuid::parse("08967256-661D-4C25-8449-47B6C632B1F9", id);
    Uuid::parse("08967256-661D-4C25-a449-47B6C632B1F9", id2);
    return !id.isEmpty() && !id2.isEmpty() && id2 > id;
}

bool testLessThan() {
    Uuid id, id2;
    Uuid::parse("08967256-661e-4C25-8449-47B6C632B1F9", id);
    Uuid::parse("08967256-661D-4C25-8449-47B6C632B1F9", id2);
    return !id.isEmpty() && !id2.isEmpty() && id2 < id;
}

bool testStream() {
    MemoryStream ms;
    static const String idStr = "08967256-661D-4C25-8449-47B6C632B1F9";
    Uuid id;
    if (Uuid::parse(idStr, id)) {
        id.write(&ms);
        ms.seek(0);
        Uuid id2;
        id2.read(&ms);
        return id == id2;
    }
    return false;
}

bool testUuids() {
    Uuids test{Uuid("08967256-661D-4C25-8449-47B6C632B1F9"), Uuid("08967256-661D-4C25-8449-47B6C632B1FA")};
    if (test.count() != 2) {
        return false;
    }

    return true;
}

int main() {
    if (!testGenerate())
        return 1;
    if (!testEmpty1())
        return 2;
    if (!testEmpty2())
        return 3;
    if (!testParse())
        return 4;
    if (!testEquals())
        return 5;
    if (!testGreaterThan())
        return 6;
    if (!testLessThan())
        return 7;
    if (!testStream())
        return 8;
    if (!testUuids()) {
        return 9;
    }
    return 0;
}