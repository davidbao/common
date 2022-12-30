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

namespace IO {
    class Stream;
}
using namespace IO;

namespace Data {
    struct PositionCoord
            : public IEquatable<PositionCoord>, public IEvaluation<PositionCoord>, public IComparable<PositionCoord> {
    public:
        double latitude;
        double longitude;

        explicit PositionCoord(double longitude = Double::NaN, double latitude = Double::NaN);

        PositionCoord(const PositionCoord &coord);

        explicit PositionCoord(const String &str);

        explicit PositionCoord(const char *str);

        ~PositionCoord() override;

        bool equals(const PositionCoord &other) const override;

        void evaluates(const PositionCoord &other) override;

        int compareTo(const PositionCoord &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        static bool parse(const String &str, PositionCoord &value);

        PositionCoord &operator=(const PositionCoord &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

    public:
        static const PositionCoord Empty;
    };

    typedef Vector<PositionCoord> LocationCoords;
}

#endif /* PositionCoord_h */
