#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "data/ValueType.h"
#include "data/ByteArray.h"

namespace Common
{
    class Clipboard
    {
    public:
        Clipboard(bool inner = true);
        ~Clipboard();
        
        bool clear();
        
        bool isText() const;
        bool setText(const String& data);
        bool getText(String& data);

        bool isImage() const;
        bool setImage(const ByteArray& data);
        bool getImage(ByteArray& data);
        
        bool isValue(const String& typeName) const;
        template <class T>
        bool setValue(const String& typeName, const T& value)
        {
            if(_inner)
            {
                if(typeName.isNullOrEmpty())
                    return false;
                
                clear();
                
                _type = typeName;
                _value = value.clone();
				return true;
            }
            else
            {
                // todo: maybe can do it.
                return false;
            }
        }
        template <class T>
        bool getValue(const String& typeName, T& value)
        {
            if(_inner)
            {
                if(typeName.isNullOrEmpty())
                    return false;
                
                if(_type == typeName && _value != nullptr)
                {
                    T* temp = (T*)_value;
                    value.copyFrom(temp);
                    return true;
                }
                return false;
            }
            else
            {
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
        void* _clipboard;
        
        static void* _value;
        static String _type;
        
    private:
        static const String TextType;
        static const String ImageType;
    };
}

#endif // CLIPBOARD_H
