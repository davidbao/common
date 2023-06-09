//
//  VariantTest.cpp
//  common
//
//  Created by baowei on 2022/11/28.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/Variant.h"
#include "IO/MemoryStream.h"

using namespace Data;

bool testConstructor() {
    {
        Variant test;
        if (!test.isNullType()) {
            return false;
        }
    }

    {
        Variant test(true);
        if (!test) {
            return false;
        }
    }
    {
        Variant test(false);
        if (test) {
            return false;
        }
    }

    {
        Variant test((int8_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((uint8_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((uint16_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((uint32_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((int64_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test((uint64_t) 1);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test(1.0f);
        if (test != 1) {
            return false;
        }
    }
    {
        Variant test(1.0);
        if (test != 1) {
            return false;
        }
    }

    {
        Variant test("1");
        if (test != "1") {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        if (test != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromHours(1));
        if (test != TimeSpan::fromHours(1)) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        if (test != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    {
        Variant test(ByteArray{1, 2, 3, 4});
        Variant test2(test);
        if (test2 != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        Variant test(true);
        Variant test2;
        test2.evaluates(test);
        if (!test2) {
            return false;
        }
    }

    {
        Variant test((int8_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint8_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint16_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint32_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int64_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint64_t) 1);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test(1.0f);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test(1.0);
        Variant test2;
        test2.evaluates(test);
        if (test2 != 1) {
            return false;
        }
    }

    {
        Variant test("1");
        Variant test2;
        test2.evaluates(test);
        if (test2 != "1") {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        Variant test2;
        test2.evaluates(test);
        if (test2 != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromHours(1));
        Variant test2;
        test2.evaluates(test);
        if (test2 != TimeSpan::fromHours(1)) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        Variant test2;
        test2.evaluates(test);
        if (test2 != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        Variant test(false);
        if (!test.equals(false)) {
            return false;
        }
    }

    {
        Variant test((int8_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((uint8_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((uint16_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((uint32_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((int64_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test((uint64_t) 1);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test(1.0f);
        if (!test.equals(1)) {
            return false;
        }
    }
    {
        Variant test(1.0);
        if (!test.equals(1)) {
            return false;
        }
    }

    {
        Variant test("1");
        if (!test.equals("1")) {
            return false;
        }
    }
    {
        Variant test("1");
        if (test != "1") {
            return false;
        }
    }
    {
        Variant test("1");
        if (!(test == "1")) {
            return false;
        }
    }

    {
        Variant test(DateTime(2010, 1, 2));
        if (!test.equals(DateTime(2010, 1, 2))) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromHours(1));
        if (!test.equals(TimeSpan::fromHours(1))) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        if (!test.equals(ByteArray{1, 2, 3, 4})) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        Variant test(Variant::Null);
        if (!test.isNullType()) {
            return false;
        }
    }
    {
        Variant test(Variant::Integer16);
        if (test.isNullType()) {
            return false;
        }
    }

    {
        Variant test(Variant::Integer16);
        if (!test.isNullValue()) {
            return false;
        }
    }

    {
        Variant test(Variant::Integer16);
        test.setValue(2);
        if (test.isNullValue()) {
            return false;
        }
    }
    {
        Variant test(Variant::Integer16);
        test.setValue(2);
        test.setNullValue();
        if (!test.isNullValue()) {
            return false;
        }
    }

    {
        Variant test((int16_t) 1);
        if (test.type() != Variant::Integer16) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        const Variant::Value &value = test.value();
        if (value.sValue != 1) {
            return false;
        }
    }

    {
        Variant test((int16_t) 1);
        if (test.valueStr() != "1") {
            return false;
        }
    }

    {
        Variant test((int16_t) 1);
        if (test.typeStr() != "Integer16") {
            return false;
        }
    }

    {
        Variant test((int16_t) 1);
        if (test.toString() != "1") {
            return false;
        }
    }

    {
        Variant test((int16_t) 1);
        if (!test.isAnalogValue()) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        if (!test.isIntegerValue()) {
            return false;
        }
    }
    {
        Variant test((float) 1);
        if (!test.isFloatValue()) {
            return false;
        }
    }
    {
        Variant test((double) 1);
        if (!test.isFloatValue()) {
            return false;
        }
    }
    {
        Variant test("1");
        if (!test.isStringValue()) {
            return false;
        }
    }
    {
        Variant test(true);
        if (!test.isDigitalValue()) {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        if (!test.isDateTimeValue()) {
            return false;
        }
    }

    {
        Variant test(true);
        if (test.valueSize() != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        if (test.valueSize() != sizeof(int32_t)) {
            return false;
        }
    }

    return true;
}

bool testValue() {
    {
        Variant test(true);
        bool value = false;
        if (!(test.getValue(value) && value)) {
            return false;
        }
    }
    {
        Variant test(2);
        bool value = false;
        if (!(test.getValue(value) && value)) {
            return false;
        }
    }
    {
        Variant test("2");
        bool value = false;
        if (!(test.getValue(value) && value)) {
            return false;
        }
    }
    {
        Variant test(2.0f);
        bool value = false;
        if (!(test.getValue(value) && value)) {
            return false;
        }
    }
    {
        Variant test(2.0);
        bool value = false;
        if (!(test.getValue(value) && value)) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        bool value = false;
        if (test.getValue(value)) {
            return false;
        }
    }
    {
        Variant test(Variant::Decimal);
        if (!test.setValue(1)) {
            return false;
        }
        if (test.toString() != "1") {
            return false;
        }
    }
    {
        Variant test(Variant::Decimal);
        if (!test.setValue((uint64_t)113715892637925384)) {
            return false;
        }
        if (test.toString() != "113715892637925384") {
            return false;
        }
    }
    {
        Variant test(Variant::Decimal);
        if (!test.setValue("113715892637925384")) {
            return false;
        }
        if (test.toString() != "113715892637925384") {
            return false;
        }
    }

    return true;
}

bool testOperator() {
    {
        Variant test(true);
        Variant test2;
        test2 = test;
        if (!test2) {
            return false;
        }
    }

    {
        Variant test((int8_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint8_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint16_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint32_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((int64_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test((uint64_t) 1);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test(1.0f);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }
    {
        Variant test(1.0);
        Variant test2;
        test2 = test;
        if (test2 != 1) {
            return false;
        }
    }

    {
        Variant test("1");
        Variant test2;
        test2 = test;
        if (test2 != "1") {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        Variant test2;
        test2 = test;
        if (test2 != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromHours(1));
        Variant test2;
        test2 = test;
        if (test2 != TimeSpan::fromHours(1)) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        Variant test2;
        test2 = test;
        if (test2 != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    {
        Variant test(true);
        Variant test2;
        test2 = test;
        bool value = test2;
        if (!value) {
            return false;
        }
    }

    {
        Variant test((int8_t) 1);
        Variant test2;
        test2 = test;
        int8_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((uint8_t) 1);
        Variant test2;
        test2 = test;
        uint8_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((int16_t) 1);
        Variant test2;
        test2 = test;
        int16_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((uint16_t) 1);
        Variant test2;
        test2 = test;
        uint16_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        Variant test2;
        test2 = test;
        int32_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((uint32_t) 1);
        Variant test2;
        test2 = test;
        uint32_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((int64_t) 1);
        Variant test2;
        test2 = test;
        int64_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test((uint64_t) 1);
        Variant test2;
        test2 = test;
        uint64_t value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test(1.0f);
        Variant test2;
        test2 = test;
        float value = test2;
        if (value != 1) {
            return false;
        }
    }
    {
        Variant test(1.0);
        Variant test2;
        test2 = test;
        double value = test2;
        if (value != 1) {
            return false;
        }
    }

    {
        Variant test("1");
        Variant test2;
        test2 = test;
        String value = test2;
        if (value != "1") {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        Variant test2;
        test2 = test;
        DateTime value = test2;
        if (value != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromHours(1));
        Variant test2;
        test2 = test;
        TimeSpan value = test2;
        if (value != TimeSpan::fromHours(1)) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        Variant test2;
        test2 = test;
        ByteArray value = test2;
        if (value != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    return true;
}

bool testStaticMethod() {
    {
        Variant::Type t1 = Variant::Timestamp;
        Variant::Value v1(TimeSpan::fromHours(1).ticks());
        Variant::Type t2 = Variant::Timestamp;
        Variant::Value v2;
        if (!Variant::changeValue(t1, v1, t2, v2)) {
            return false;
        }
        if (v1.timeValue != v2.timeValue) {
            return false;
        }
    }
    {
        Variant::Type t1 = Variant::Timestamp;
        Variant::Value v1(TimeSpan::fromHours(1).ticks());
        Variant::Type t2 = Variant::Integer64;
        Variant::Value v2;
        if (!Variant::changeValue(t1, v1, t2, v2)) {
            return false;
        }
        if (v1.timeValue != v2.lValue) {
            return false;
        }
    }

    {
        Variant::Type t1 = Variant::Text;
        Variant::Value v1((char *) "123");
        Variant::Type t2 = Variant::Integer64;
        Variant::Value v2;
        if (!Variant::changeValue(t1, v1, t2, v2)) {
            return false;
        }
        if (v2.lValue != 123) {
            return false;
        }
    }

    {
        Variant::Type t1 = Variant::Timestamp;
        Variant::Value v1(TimeSpan::fromHours(1).ticks());
        Variant::Type t2 = Variant::Timestamp;
        Variant::Value v2(TimeSpan::fromHours(1).ticks());
        if (!Variant::equals(t1, v1, t2, v2)) {
            return false;
        }
    }
    {
        Variant::Type t1 = Variant::Timestamp;
        Variant::Value v1(TimeSpan::fromHours(1).ticks());
        Variant::Type t2 = Variant::Integer64;
        Variant::Value v2(TimeSpan::fromHours(1).ticks());
        if (!Variant::equals(t1, v1, t2, v2)) {
            return false;
        }
    }

    {
        Variant::Type t1 = Variant::Text;
        Variant::Value v1((char *) "123");
        Variant::Type t2 = Variant::Integer64;
        Variant::Value v2((int64_t) 123);
        if (!Variant::equals(t1, v1, t2, v2)) {
            return false;
        }
    }

    {
        if (!Variant::isAnalogValue(Variant::Float32)) {
            return false;
        }
        if (!Variant::isDigitalValue(Variant::Digital)) {
            return false;
        }
        if (!Variant::isDigitalValue(Variant::Bool)) {
            return false;
        }
        if (!Variant::isIntegerValue(Variant::Integer16)) {
            return false;
        }
        if (!Variant::isFloatValue(Variant::Float64)) {
            return false;
        }
        if (!Variant::isStringValue(Variant::Text)) {
            return false;
        }
        if (!Variant::isDateTimeValue(Variant::Date)) {
            return false;
        }
        if (!Variant::isDateTimeValue(Variant::Time)) {
            return false;
        }
        if (!Variant::isDateTimeValue(Variant::Timestamp)) {
            return false;
        }
        if (!Variant::isNullType(Variant())) {
            return false;
        }
        if (!Variant::isNullValue(Variant())) {
            return false;
        }

        if (Variant::toTypeScriptStr(Variant::Integer32) != "int32_t") {
            return false;
        }

        if (Variant::toTypeStr(Variant::Integer32) != "Integer32") {
            return false;
        }
        if (Variant::fromTypeStr("Integer32") != Variant::Integer32) {
            return false;
        }
    }

    {
        StringArray array;
        Variant::getAllTypeStr(array);
        if (array.count() != 19) {
            return false;
        }
    }

    {
        Variant::Value value(3.5);
        if (Variant::toAnalogValue(Variant::Float64, value) != 3.5) {
            return false;
        }
    }
    {
        Variant::Value value(3.5f);
        if (Variant::toAnalogValue(Variant::Float32, value) != 3.5) {
            return false;
        }
    }
    {
        Variant::Value value((int64_t) 4);
        if (Variant::toAnalogValue(Variant::Integer64, value) != 4) {
            return false;
        }
    }

    {
        Variant::Value value = Variant::fromAnalogValue(Variant::Integer64, 4);
        if (value.lValue != 4) {
            return false;
        }
    }
    {
        Variant::Value value = Variant::fromAnalogValue(Variant::Float64, 3.5);
        if (value.dValue != 3.5) {
            return false;
        }
    }

    {
        Variant test(true);
        if (Variant::valueSize(test.type(), test.value()) != 1) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        if (Variant::valueSize(test.type(), test.value()) != sizeof(int32_t)) {
            return false;
        }
    }

    {
        String str = Variant::toValueString(Variant::Integer64, Variant::Value((int64_t) 4));
        if (str != "4") {
            return false;
        }
    }
    {
        String str = Variant::toValueString(Variant::Float64, Variant::Value(4.0));
        if (str != "4") {
            return false;
        }
    }

    {
        Variant::Value value;
        if (!Variant::parseValueString("4", Variant::Integer64, value)) {
            return false;
        }
        if (value.lValue != 4) {
            return false;
        }
    }
    {
        Variant::Value value;
        if (!Variant::parseValueString("3.5", Variant::Float64, value)) {
            return false;
        }
        if (value.dValue != 3.5) {
            return false;
        }
    }

    return true;
}

bool testStream() {
    {
        Variant test(Variant::Null);
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        Variant test((int32_t) 1);
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        Variant test("abc123");
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        Variant test(ByteArray{1, 2, 3, 4});
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        Variant test(DateTime(2010, 1, 2));
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        Variant test(TimeSpan::fromSeconds(40));
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Variant test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool testValueCompareTo() {
    {
        Variant v1((T) 1), v2((T) 2);
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1((T) -2), v2((T) -1);
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1((T) 2), v2((T) 1);
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        Variant v1((T) -1), v2((T) -2);
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        Variant v1((T) 2), v2((T) 2);
        if (!(v1 == v2)) {
            return false;
        }
    }

    return true;
}

bool testCompareTo() {
    // Bool
    {
        Variant v1(false), v2(true);
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1(true), v2(false);
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        Variant v1(false), v2(false);
        if (!(v1 == v2)) {
            return false;
        }
    }
    {
        Variant v1(true), v2(true);
        if (!(v1 == v2)) {
            return false;
        }
    }

    // Integer
    if (!testValueCompareTo<int8_t>()) {
        return false;
    }
    if (!testValueCompareTo<uint8_t>()) {
        return false;
    }
    if (!testValueCompareTo<int16_t>()) {
        return false;
    }
    if (!testValueCompareTo<uint16_t>()) {
        return false;
    }
    if (!testValueCompareTo<int32_t>()) {
        return false;
    }
    if (!testValueCompareTo<uint32_t>()) {
        return false;
    }
    if (!testValueCompareTo<int64_t>()) {
        return false;
    }
    if (!testValueCompareTo<uint64_t>()) {
        return false;
    }

    // Float & Double
    if (!testValueCompareTo<float>()) {
        return false;
    }
    if (!testValueCompareTo<double>()) {
        return false;
    }

    // Text
    {
        Variant v1("1"), v2("2");
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1("-2"), v2("-1");
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        Variant v1("2"), v2("1");
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        Variant v1("-1"), v2("-2");
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1("2"), v2("2");
        if (!(v1 == v2)) {
            return false;
        }
    }
    {
        Variant v1("123"), v2("abc");
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1("0"), v2("abc");
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1("1"), v2(2);
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1(1), v2("2");
        if (!(v1 < v2)) {
            return false;
        }
    }

    // Date
    {
        DateTime now = DateTime::now();
        Variant v1(now.addDays(1)), v2(now.addDays(2));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        DateTime now = DateTime::now();
        Variant v1(now.addDays(-2)), v2(now.addDays(-1));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        DateTime now = DateTime::now();
        Variant v1(now.addDays(2)), v2(now.addDays(1));
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        DateTime now = DateTime::now();
        Variant v1(now.addDays(-1)), v2(now.addDays(-2));
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        DateTime now = DateTime::now();
        Variant v1(now.addDays(2)), v2(now.addDays(2));
        if (!(v1 == v2)) {
            return false;
        }
    }

    // Timestamp
    {
        TimeSpan now = TimeSpan::fromDays(5.346);
        Variant v1(now.add(1)), v2(now.add(2));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        TimeSpan now = TimeSpan::fromDays(5.346);
        Variant v1(now.add(-2)), v2(now.add(-1));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        TimeSpan now = TimeSpan::fromDays(5.346);
        Variant v1(now.add(2)), v2(now.add(1));
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        DateTime now = DateTime::now();
        Variant v1(now.add(-1)), v2(now.add(-2));
        if (!(v1 > v2)) {
            return false;
        }
    }
    {
        TimeSpan now = TimeSpan::fromDays(5.346);
        Variant v1(now.add(2)), v2(now.add(2));
        if (!(v1 == v2)) {
            return false;
        }
    }

    // Blob
    {
        Variant v1(ByteArray({1, 2, 3, 4})), v2(ByteArray({1, 2, 3, 4, 5}));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1(ByteArray({1, 2, 3, 4})), v2(ByteArray({1, 2, 3, 5}));
        if (!(v1 < v2)) {
            return false;
        }
    }
    {
        Variant v1(ByteArray({1, 2, 3, 4})), v2(ByteArray({1, 2, 3, 4}));
        if (!(v1 == v2)) {
            return false;
        }
    }

    return true;
}

bool testCollection() {
    {
        Variants test {
                Variant(1.0), Variant(1), Variant("abc")
        };
        if (test.count() != 3) {
            return false;
        }
    }
    {
        VariantMap test {
                {"1", Variant(1.0)},
                {"2", Variant(1)},
                {"3", Variant("abc")}
        };
        if (test.count() != 3) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testEvaluates()) {
        return 2;
    }
    if (!testEquals()) {
        return 3;
    }
    if (!testProperty()) {
        return 4;
    }
    if (!testValue()) {
        return 5;
    }
    if (!testOperator()) {
        return 6;
    }
    if (!testStaticMethod()) {
        return 7;
    }
    if (!testStaticMethod()) {
        return 8;
    }
    if (!testStream()) {
        return 9;
    }
    if (!testCompareTo()) {
        return 10;
    }
    if (!testCollection()) {
        return 11;
    }

    return 0;
}