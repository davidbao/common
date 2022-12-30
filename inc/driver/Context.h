#ifndef CONTEXT_H
#define CONTEXT_H

#include "data/ValueType.h"
#include "data/PList.h"

using namespace Data;

namespace Drivers {
    enum ValueType {
        ValueEmpty = 0,
        ValueInt = 1,
        ValueShort = 2,
        ValueChar = 3,
        ValueStr = 4,

    };

    struct ValueSet {
        ValueType type;
        union {
            int iValue;
            char strValue[256];
            unsigned short wValue;
            char cValue;
        } Value;

    public:
        ValueSet();

        ValueSet(const String &value);

        ValueSet(int value);

        bool isEmpty() const;
    };

    struct Data {
    public:
        Data();

        Data(const String &name, ValueSet value);

        const String &name() const;

        ValueSet getValue() const;

    private:
        String _name;
        ValueSet _value;
    };

    typedef PList <Data> Datas;

    class Context {
    public:
        Context();

        virtual ~Context();

//        void setValue(const String& name, ValueSet value);
//        ValueSet getValue(const String& name);
//
//    private:
//        Data* find(const String& name);

//    private:
//        Datas* _data;
    };
}
#endif // CONTEXT_H
