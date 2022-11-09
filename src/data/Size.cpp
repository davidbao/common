//
//  Size.cpp
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#include "data/Size.h"
#include "system/Math.h"
#include "data/Convert.h"

namespace Common {
    const SizeF SizeF::Empty;
    const SizeF SizeF::MinValue = SizeF(Float::MinValue, Float::MinValue);
    const SizeF SizeF::MaxValue = SizeF(Float::MaxValue, Float::MaxValue);

    SizeF::SizeF(float width, float height) {
        this->width = width;
        this->height = height;
    }

    SizeF::SizeF(int width, int height) {
        this->width = (float) width;
        this->height = (float) height;
    }

    SizeF::SizeF(const SizeF &size) {
        this->width = size.width;
        this->height = size.height;
    }

    bool SizeF::isEmpty() const {
        return width == 0.0f && height == 0.0f;
    }

    void SizeF::empty() {
        width = 0.0f;
        height = 0.0f;
    }

    String SizeF::toString() const {
        return String::convert("%.0f,%.0f", width, height);
    }

    void SizeF::operator=(const SizeF &value) {
        this->width = value.width;
        this->height = value.height;
    }

    bool SizeF::operator==(const SizeF &value) const {
        return this->width == value.width && this->height == value.height;
    }

    bool SizeF::operator!=(const SizeF &value) const {
        return !operator==(value);
    }

    bool SizeF::parse(const String &str, SizeF &size) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 2) {
            SizeF temp;
            if (Float::parse(texts[0].trim(), temp.width) &&
                Float::parse(texts[1].trim(), temp.height)) {
                size = temp;
                return true;
            }
        }
        return false;
    }

    Size SizeF::round() const {
        return Size((int) Math::round(width), (int) Math::round(height));
    }

    const Size Size::Empty;
    const Size Size::MinValue = Size(Int32::MinValue, Int32::MinValue);
    const Size Size::MaxValue = Size(Int32::MaxValue, Int32::MaxValue);

    Size::Size(int width, int height) {
        this->width = width;
        this->height = height;
    }

    Size::Size(const Size &size) {
        this->width = size.width;
        this->height = size.height;
    }

    bool Size::isEmpty() const {
        return width == 0 && height == 0;
    }

    void Size::empty() {
        width = 0;
        height = 0;
    }

    String Size::toString() const {
        return String::convert("%d,%d", width, height);
    }

    void Size::operator=(const Size &value) {
        this->width = value.width;
        this->height = value.height;
    }

    bool Size::operator==(const Size &value) const {
        return this->width == value.width && this->height == value.height;
    }

    bool Size::operator!=(const Size &value) const {
        return !operator==(value);
    }

    bool Size::parse(const String &str, Size &size) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 2) {
            Size temp;
            if (Int32::parse(texts[0].trim(), temp.width) &&
                Int32::parse(texts[1].trim(), temp.height)) {
                size = temp;
                return true;
            }
        }
        return false;
    }
}
