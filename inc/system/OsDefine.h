//
//  OsDefine.h
//  common
//
//  Created by baowei on 2022/09/26.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef OsDefine_h
#define OsDefine_h

#include <stdio.h>
#include <string.h>

#if !defined(COMMON_ATTRIBUTE)
#  if defined(__clang__) || defined(__GNUC__)
#    define COMMON_ATTRIBUTE(attr) __attribute__((attr))
#  else
#    define COMMON_ATTRIBUTE(attr)
#  endif
#endif

#if !defined(COMMON_UNUSED)
#  define COMMON_UNUSED COMMON_ATTRIBUTE(unused)
#endif

#if !defined(COMMON_ATTR_DEPRECATED)
// FIXME: we ignore msg for now...
#  define COMMON_ATTR_DEPRECATED(msg) COMMON_ATTRIBUTE(deprecated)
#endif

#ifdef __APPLE__
# include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE || __ANDROID__
#define PHONE_OS 1
#endif

#if __linux__ && __arm__ && !__ANDROID__
#define __arm_linux__ 1
#endif

#if PHONE_OS || __arm_linux__ || __EMSCRIPTEN__
#define LITE_OS 1
#endif

#ifndef LITE_OS
#define PC_OS 1
#endif

#if __APPLE__ && TARGET_OS_OSX
#define MAC_OS 1
#endif
#ifdef WIN32
#define WIN_OS 1
#endif
#if __linux__ && !__arm__ && !__ANDROID__
#define LINUX_OS 1
#endif
#if __EMSCRIPTEN__
#define BROWSER_OS 1
#endif
#if __ANDROID__
#define ANDROID_OS 1
#endif
#if TARGET_OS_IOS
#define IPHONE_OS 1
#endif

#if _WIN64 || __x86_64__ || __arm64__
#define OS_X64 1
#else
#define OS_X32 1
#endif

#ifndef UINT64_MAX
#define UINT64_MAX        18446744073709551615ULL
#endif
#ifndef UINT32_MAX
#define UINT32_MAX        4294967295U
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#if WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
#ifndef HAS_SSIZE_T
#define HAS_SSIZE_T
typedef SSIZE_T ssize_t;
#endif  // HAS_SSIZE_T
#endif  // _MSC_VER

#if defined(__clang__)
/* Clang also masquerades as GCC */
#    if defined(__apple_build_version__)
#      /* http://en.wikipedia.org/wiki/Xcode#Toolchain_Versions */
#      if __apple_build_version__ >= 8020041
#        define CC_CLANG 309
#      elif __apple_build_version__ >= 8000038
#        define CC_CLANG 308
#      elif __apple_build_version__ >= 7000053
#        define CC_CLANG 306
#      elif __apple_build_version__ >= 6000051
#        define CC_CLANG 305
#      elif __apple_build_version__ >= 5030038
#        define CC_CLANG 304
#      elif __apple_build_version__ >= 5000275
#        define CC_CLANG 303
#      elif __apple_build_version__ >= 4250024
#        define CC_CLANG 302
#      elif __apple_build_version__ >= 3180045
#        define CC_CLANG 301
#      elif __apple_build_version__ >= 2111001
#        define CC_CLANG 300
#      else
#        error "Unknown Apple Clang version"
#      endif
#    else
#      define CC_CLANG ((__clang_major__ * 100) + __clang_minor__)
#    endif
#endif
#if defined(__GNUC__)
#define CC_GNU          (__GNUC__ * 100 + __GNUC_MINOR__)
#endif  // __GNUC__

#endif // OsDefine_h
