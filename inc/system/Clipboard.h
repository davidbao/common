//
//  Clipboard.h
//  common
//
//  Created by baowei on 2016/12/27.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Clipboard_h
#define Clipboard_h

#include "data/String.h"
#include "data/ByteArray.h"

using namespace Data;

namespace System {
    class Clipboard {
    public:
        explicit Clipboard(bool inner = true);

        ~Clipboard();

        bool clear();

        bool isText() const;

        bool setText(const String &data);

        bool getText(String &data);

        bool isImage() const;

        bool setImage(const ByteArray &data);

        bool getImage(ByteArray &data);

        bool isValue(const String &typeName) const;

        template<class T>
        bool setValue(const String &typeName, const T &value) {
            if (_inner) {
                if (typeName.isNullOrEmpty())
                    return false;

                clear();

                _type = typeName;
                _value = value.clone();
                return true;
            } else {
                // todo: maybe can do it.
                return false;
            }
        }

        template<class T>
        bool getValue(const String &typeName, T &value) {
            if (_inner) {
                if (typeName.isNullOrEmpty())
                    return false;

                if (_type == typeName && _value != nullptr) {
                    T *temp = (T *) _value;
                    value.copyFrom(temp);
                    return true;
                }
                return false;
            } else {
                // todo: maybe can do it.
                return false;
            }
        }

//#ifdef WIN_OS
//        bool setBitmap(void* hBitmap);
//        bool getBitmap(void*& hBitmap);
//#endif

    private:
        bool _inner;
        void *_clipboard;

        static void *_value;
        static String _type;

    private:
        static const String TextType;
        static const String ImageType;
    };
}

#endif // Clipboard_h
