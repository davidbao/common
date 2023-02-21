//
//  MappingStreamTest.cpp
//  common
//
//  Created by baowei on 2022/10/26.
//  Copyright (c) 2022 com. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
//#include <Windows.h>

//class MappingPosition {
//public:
//    MappingPosition(const char* fileName) : _file(nullptr), _mapFile(nullptr), _fileSize(0), _accessor(nullptr) {
//        _file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
//        if(_file != nullptr) {
//            _mapFile = CreateFileMapping(_file, nullptr, PAGE_READONLY, 0, (DWORD) _fileSize, nullptr);
//
//            LARGE_INTEGER li;
//            GetFileSizeEx(_file, &li);
//            _fileSize = li.QuadPart;
//
//            if((_accessor = MapViewOfFile(_mapFile, FILE_MAP_READ, 0, (DWORD)0, (SIZE_T)_fileSize)) == nullptr)
//            {
//                LPVOID lpMsgBuf;
//                DWORD dw = GetLastError();
//
//                FormatMessage(
//                        FORMAT_MESSAGE_ALLOCATE_BUFFER |
//                        FORMAT_MESSAGE_FROM_SYSTEM |
//                        FORMAT_MESSAGE_IGNORE_INSERTS,
//                        nullptr,
//                        dw,
//                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                        (LPTSTR)&lpMsgBuf,
//                        0, nullptr);
//
//                printf("mmap wrong!, reason: %s\n", (LPTSTR)lpMsgBuf);
//                LocalFree(lpMsgBuf);
//            }
//        }
//    }
//
//    ~MappingPosition() {
//        if(_accessor != nullptr) {
//            UnmapViewOfFile(_accessor);
//        }
//        if(_file != nullptr) {
//            CloseHandle(_file);
//        }
//        if(_mapFile != nullptr) {
//            CloseHandle(_mapFile);
//        }
//    }
//
//    bool isValid() {
//        return _accessor != nullptr;
//    }
//
//    size_t read(size_t position, uint8_t *array, size_t offset, size_t count) {
//        if(isValid()) {
//            memcpy(array + offset, (uint8_t *) _accessor + position, count);
//            return count;
//        }
//        return 0;
//    }
//
//    float operator[](size_t pos) {
//        if(isValid()) {
//            size_t offset = HeaderLength + pos * 4;
//            if(offset + sizeof(float) >= size() ) {
//                return 0.0f;
//            }
//
//            union data {
//                uint8_t buffer[sizeof(float)];
//                float value;
//            };
//            data d;
//            size_t length = read(offset, (uint8_t*)&d, 0, sizeof(d));
//            if(length == sizeof(float)) {
//                return d.value;
//            }
//        }
//        return 0.0f;
//    }
//
//    size_t size() {
//        if(isValid()) {
//            return (_fileSize - HeaderLength) / 4;
//        }
//        return 0;
//    }
//
//private:
//    HANDLE _file;
//    HANDLE _mapFile;
//    size_t _fileSize;
//    LPVOID _accessor;
//
//    static const size_t HeaderLength = 4096;
//};

int main() {
//    MappingPosition mp("W:\\1820_ch01_20221002_030728.dat");
//    if(mp.isValid()) {
//        float v1 = mp[0];
//        float v2 = mp[10];
//        float vend = mp[mp.size() - 1];
//        printf("v1: %g, v2: %g, vend: %g\n", v1, v2, vend);
//    }
    return 0;
}