//
//  Uuid.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef Uuid_h
#define Uuid_h

#include <stdio.h>

#if WIN32
#ifndef GUID_DEFINED
#include <guiddef.h>
#endif /* GUID_DEFINED */

#ifndef UUID_DEFINED
#define UUID_DEFINED
typedef GUID UUID;
#ifndef uuid_t
#define uuid_t UUID
#endif
#endif
#endif

#include "data/Vector.h"
#include "data/ValueType.h"

namespace Common {
#ifndef uuid_t
    typedef unsigned char uuid_t[16];
#endif

    struct Uuid {
    public:
        Uuid();

        Uuid(const Uuid &value);

        Uuid(const String &value);

        ~Uuid();

        void operator=(const Uuid &value);

        bool operator==(const Uuid &value) const;

        bool operator!=(const Uuid &value) const;

        void operator=(const String &value);

        bool operator==(const String &value) const;

        bool operator!=(const String &value) const;

        bool operator>(const Uuid &value) const;

        bool operator>=(const Uuid &value) const;

        bool operator<(const Uuid &value) const;

        bool operator<=(const Uuid &value) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString(bool upper = true) const;

        void empty();

        bool isEmpty() const;

        void clear();

    public:
        static bool parse(const String &str, Uuid &value);

        static Uuid generate();

    public:
        static const Uuid Empty;

    private:
        uuid_t _value;

        static const int Size = sizeof(uuid_t);
    };

    typedef Vector<Uuid> Uuids;
}

#endif // Uuid_h
