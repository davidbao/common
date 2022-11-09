//
//  TypeInfoTest.cpp
//  common
//
//  Created by baowei on 2022/11/8.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/TypeInfo.h"
#include "data/ValueType.h"

using namespace Common;

bool testIsSpecialized() {
    {
        if (TypeInfo<int>::isSpecialized) {
            return false;
        }
    }
    {
        if (TypeInfo<Int32>::isSpecialized) {
            return false;
        }
    }
    {
        if (TypeInfo<String>::isSpecialized) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (!TypeInfo<Values>::isSpecialized) {
            return false;
        }
    }
    {
        if (!TypeInfo<String *>::isSpecialized) {
            return false;
        }
    }

    return true;
}

bool testIsPointer() {
    {
        if (TypeInfo<int>::isPointer) {
            return false;
        }
    }
    {
        if (TypeInfo<Int32>::isPointer) {
            return false;
        }
    }
    {
        if (TypeInfo<String>::isPointer) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (TypeInfo<Values>::isPointer) {
            return false;
        }
    }
    {
        if (!TypeInfo<String *>::isPointer) {
            return false;
        }
    }

    return true;
}

bool testIsIntegral() {
    {
        if (!TypeInfo<int>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<long>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<char>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<wchar_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<int8_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<uint8_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<int16_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<uint16_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<int32_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<uint32_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<int64_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<uint64_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<unsigned char>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<size_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<ssize_t>::isIntegral) {
            return false;
        }
    }
    {
        if (!TypeInfo<off_t>::isIntegral) {
            return false;
        }
    }
    {
        if (TypeInfo<float>::isIntegral) {
            return false;
        }
    }
    {
        if (TypeInfo<double>::isIntegral) {
            return false;
        }
    }
    {
        if (TypeInfo<Int32>::isIntegral) {
            return false;
        }
    }
    {
        if (TypeInfo<String>::isIntegral) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (TypeInfo<Values>::isIntegral) {
            return false;
        }
    }
    {
        if (TypeInfo<String *>::isIntegral) {
            return false;
        }
    }

    return true;
}

bool testIsComplex() {
    {
        if (TypeInfo<int>::isComplex) {
            return false;
        }
    }
    {
        if (!TypeInfo<Int32>::isComplex) {
            return false;
        }
    }
    {
        if (!TypeInfo<String>::isComplex) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (TypeInfo<Values>::isComplex) {
            return false;
        }
    }
    {
        if (TypeInfo<String *>::isComplex) {
            return false;
        }
    }

    return true;
}

bool testIsStatic() {
    {
        if (!TypeInfo<int>::isStatic) {
            return false;
        }
    }
    {
        if (!TypeInfo<Int32>::isStatic) {
            return false;
        }
    }
    {
        if (!TypeInfo<String>::isStatic) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (!TypeInfo<Values>::isStatic) {
            return false;
        }
    }
    {
        if (TypeInfo<String *>::isStatic) {
            return false;
        }
    }

    return true;
}

bool testIsRelocatable() {
    {
        if (!TypeInfo<int>::isRelocatable) {
            return false;
        }
    }
    {
        if (TypeInfo<Int32>::isRelocatable) {
            return false;
        }
    }
    {
        if (TypeInfo<String>::isRelocatable) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (!TypeInfo<Values>::isRelocatable) {
            return false;
        }
    }
    {
        if (!TypeInfo<String *>::isRelocatable) {
            return false;
        }
    }

    return true;
}

bool testIsLarge() {
    {
        if (TypeInfo<int>::isLarge) {
            return false;
        }
    }
    {
        struct test {
            int64_t v1;
            int64_t v2;
        };
        if (!TypeInfo<test>::isLarge) {
            return false;
        }
    }
    {
        if (!TypeInfo<Int32>::isLarge) {
            return false;
        }
    }
    {
        if (!TypeInfo<String>::isLarge) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (TypeInfo<Values>::isLarge) {
            return false;
        }
    }
    {
        if (TypeInfo<String *>::isLarge) {
            return false;
        }
    }

    return true;
}

bool testSizeOf() {
    {
        if (TypeInfo<int>::sizeOf != sizeof(int)) {
            return false;
        }
    }
    {
        if (TypeInfo<Int32>::sizeOf != sizeof(Int32)) {
            return false;
        }
    }
    {
        if (TypeInfo<String>::sizeOf != sizeof(String)) {
            return false;
        }
    }
    {
        enum Values {
            V1 = 1,
            V2 = 2,
            V3 = 3
        };
        if (TypeInfo<Values>::sizeOf != sizeof(Values)) {
            return false;
        }
    }
    {
        if (TypeInfo<String *>::sizeOf != sizeof(void *)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testIsSpecialized()) {
        return 1;
    }
    if (!testIsPointer()) {
        return 2;
    }
    if (!testIsIntegral()) {
        return 3;
    }
    if (!testIsComplex()) {
        return 4;
    }
    if (!testIsStatic()) {
        return 5;
    }
    if (!testIsRelocatable()) {
        return 6;
    }
    if (!testIsLarge()) {
        return 7;
    }
    if (!testSizeOf()) {
        return 8;
    }

    return 0;
}