//
//  PositionCoord.cpp
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#include "data/PositionCoord.h"
#include "data/ByteArray.h"
#include "data/Convert.h"
#include "IO/Stream.h"

namespace Common {
    const PositionCoord PositionCoord::Empty = PositionCoord();

    PositionCoord::PositionCoord(double
                                 longitude, double
                                 latitude) {
        this->longitude = longitude;
        this->latitude = latitude;
    }

    PositionCoord::PositionCoord(
            const PositionCoord &coord) {
        this->longitude = coord.longitude;
        this->latitude = coord.latitude;
    }

    PositionCoord::PositionCoord(
            const String &str) {
        PositionCoord coord;
        if (PositionCoord::parse(str, coord)) {
            this->operator=(coord);
        }
    }

    PositionCoord::PositionCoord(
            const char *str) : PositionCoord(String(str)) {
    }

    bool PositionCoord::isEmpty() const {
        return Double::isNaN(longitude) && Double::isNaN(latitude);
    }

    void PositionCoord::empty() {
        longitude = Double::NaN;
        latitude = Double::NaN;
    }

    const String PositionCoord::toString() const {
        if (isEmpty())
            return String::Empty;
        else
            return String::convert("%s,%s",
                                   Double(longitude).toString().c_str(),
                                   Double(latitude).toString().c_str());
    }

    bool PositionCoord::parse(const String &str, PositionCoord &value) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() != 2)
            Convert::splitStr(str, ';', texts);
        if (texts.count() != 2)
            Convert::splitStr(str, '|', texts);
        if (texts.count() == 2) {
            String longitudeStr = texts[0].trim();
            String latitudeStr = texts[1].trim();

            return Double::parse(longitudeStr, value.longitude) &&
                   Double::parse(latitudeStr, value.latitude);
        }
        return false;
    }

    void PositionCoord::operator=(const PositionCoord &value) {
        longitude = value.longitude;
        latitude = value.latitude;
    }

    bool PositionCoord::operator==(const PositionCoord &value) const {
        return longitude == value.longitude &&
               latitude == value.latitude;
    }

    bool PositionCoord::operator!=(const PositionCoord &value) const {
        return !operator==(value);
    }

    void PositionCoord::write(Stream *stream, bool bigEndian) const {
        Double(longitude).write(stream, bigEndian);
        Double(latitude).write(stream, bigEndian);
    }

    void PositionCoord::read(Stream *stream, bool bigEndian) {
        Double longitude, latitude;
        longitude.read(stream, bigEndian);
        latitude.read(stream, bigEndian);

        this->latitude = latitude;
        this->longitude = longitude;
    }
}
