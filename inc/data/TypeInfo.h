//
//  TypeInfo.h
//  common
//
//  Created by baowei on 2022/1/1.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef TypeInfo_h
#define TypeInfo_h

#include <type_traits>
#include "system/OsDefine.h"

namespace Common {
    template<typename T>
    static constexpr bool _isRelocatable() {
#if defined(CC_CLANG) || !defined(CC_GNU) || CC_GNU >= 501
        return std::is_trivially_copyable<T>::value && std::is_trivially_destructible<T>::value;
#else
        return std::is_enum<T>::value || std::is_integral<T>::value;
#endif
    }

    template<typename T>
    static constexpr bool _isTrivial() {
#if defined(CC_CLANG) || !defined(CC_GNU) || CC_GNU >= 501
        return std::is_trivial<T>::value;
#else
        return std::is_enum<T>::value || std::is_integral<T>::value;
#endif
    }

/*
The catch-all template.
*/
    template<typename T>
    class TypeInfo {
    public:
        enum {
            isSpecialized = std::is_enum<T>::value, // don't require every enum to be marked manually
            isPointer = false,
            isIntegral = std::is_integral<T>::value,
            isComplex = !_isTrivial<T>(),
            isStatic = true,
            isRelocatable = _isRelocatable<T>(),
            isLarge = (sizeof(T) > sizeof(void *)),
            sizeOf = sizeof(T)
        };
    };

    template<>
    class TypeInfo<void> {
    public:
        enum {
            isSpecialized = true,
            isPointer = false,
            isIntegral = false,
            isComplex = false,
            isStatic = false,
            isRelocatable = false,
            isLarge = false,
            sizeOf = 0
        };
    };

    template<typename T>
    class TypeInfo<T *> {
    public:
        enum {
            isSpecialized = true,
            isPointer = true,
            isIntegral = false,
            isComplex = false,
            isStatic = false,
            isRelocatable = true,
            isLarge = false,
            sizeOf = sizeof(T *)
        };
    };
}

#endif // TypeInfo_h
