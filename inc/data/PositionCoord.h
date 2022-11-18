//
//  PositionCoord.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef PositionCoord_h
#define PositionCoord_h

#include "data/ValueType.h"

namespace Common {
    class Stream;
    
    struct PositionCoord {
    public:
        double latitude;
        double longitude;

        explicit PositionCoord(double longitude = Double::NaN, double latitude = Double::NaN);

        PositionCoord(const PositionCoord &coord);

        PositionCoord(const String &str);

        PositionCoord(const char *str);

        bool isEmpty() const;

        void empty();

        String toString() const;

        static bool parse(const String &str, PositionCoord &value);

        PositionCoord &operator=(const PositionCoord &value);

        bool operator==(const PositionCoord &value) const;

        bool operator!=(const PositionCoord &value) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

    public:
        static const PositionCoord Empty;
    };

    typedef Vector<PositionCoord> LocationCoords;
}

#endif /* PositionCoord_h */
