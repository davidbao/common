//
//  Size.cpp
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#include <cfloat>
#include "data/Size.h"
#include "system/Math.h"
#include "data/Convert.h"

namespace Drawing {
    const SizeF SizeF::Empty;
    const SizeF SizeF::MinValue = SizeF(-FLT_MAX, -FLT_MAX);
    const SizeF SizeF::MaxValue = SizeF(FLT_MAX, FLT_MAX);

    SizeF::SizeF(float width, float height) {
        this->width = width;
        this->height = height;
    }

    SizeF::SizeF(int width, int height) {
        this->width = (float) width;
        this->height = (float) height;
    }

    SizeF::SizeF(long width, long height) {
        this->width = (float) width;
        this->height = (float) height;
    }

    SizeF::SizeF(const SizeF &size) {
        this->width = size.width;
        this->height = size.height;
    }

    SizeF::~SizeF() = default;

    bool SizeF::equals(const SizeF &other) const {
        return this->width == other.width && this->height == other.height;
    }

    void SizeF::evaluates(const SizeF &other) {
        this->width = other.width;
        this->height = other.height;
    }

    int SizeF::compareTo(const SizeF &other) const {
        if (width != other.width) {
            if (width > other.width) {
                return 1;
            }
            return -1;
        }
        if (height != other.height) {
            if (height > other.height) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool SizeF::isEmpty() const {
        return width == 0.0f && height == 0.0f;
    }

    void SizeF::empty() {
        width = 0.0f;
        height = 0.0f;
    }

    String SizeF::toString() const {
        return String::convert("%g,%g", width, height);
    }

    SizeF &SizeF::operator=(const SizeF &value) {
        this->width = value.width;
        this->height = value.height;
        return *this;
    }

    SizeF SizeF::operator+=(const SizeF &other) {
        *this = SizeF::add(*this, other);
        return *this;
    }

    SizeF SizeF::operator+(const SizeF &other) const {
        return SizeF::add(*this, other);
    }

    SizeF SizeF::operator-=(const SizeF &other) {
        *this = subtract(*this, other);
        return *this;
    }

    SizeF SizeF::operator-(const SizeF &other) const {
        return SizeF::subtract(*this, other);
    }

    SizeF SizeF::operator*=(float value) {
        *this = SizeF::multiply(*this, value);
        return *this;
    }

    SizeF SizeF::operator*(float value) const {
        return SizeF::multiply(*this, value);
    }

    SizeF SizeF::operator*=(int value) {
        *this = SizeF::multiply(*this, value);
        return *this;
    }

    SizeF SizeF::operator*(int value) const {
        return SizeF::multiply(*this, value);
    }

    SizeF SizeF::operator/=(float value) {
        *this = SizeF::division(*this, value);
        return *this;
    }

    SizeF SizeF::operator/(float value) const {
        return SizeF::division(*this, value);
    }

    SizeF SizeF::operator/=(int value) {
        *this = SizeF::division(*this, value);
        return *this;
    }

    SizeF SizeF::operator/(int value) const {
        return SizeF::division(*this, value);
    }

    void SizeF::add(const SizeF &size) {
        *this = SizeF::add(*this, size);
    }

    void SizeF::subtract(const SizeF &size) {
        *this = SizeF::subtract(*this, size);
    }

    void SizeF::multiply(float value) {
        *this = SizeF::multiply(*this, value);
    }

    void SizeF::multiply(int value) {
        *this = SizeF::multiply(*this, value);
    }

    void SizeF::division(float value) {
        *this = SizeF::division(*this, value);
    }

    void SizeF::division(int value) {
        *this = SizeF::division(*this, value);
    }

    Size SizeF::ceiling() const {
        return Size::ceiling(*this);
    }

    Size SizeF::round() const {
        return Size::round(*this);
    }

    Size SizeF::truncate() const{
        return Size::truncate(*this);
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

    SizeF SizeF::add(const SizeF &sz1, const SizeF &sz2) {
        return SizeF(sz1.width + sz2.width, sz1.height + sz2.height);
    }

    SizeF SizeF::subtract(const SizeF &sz1, const SizeF &sz2) {
        return SizeF(sz1.width - sz2.width, sz1.height - sz2.height);
    }

    SizeF SizeF::multiply(const SizeF& size, float value) {
        return SizeF(size.width * value, size.height * value);
    }

    SizeF SizeF::multiply(const SizeF& size, int value) {
        return multiply(size, (float)(value));
    }

    SizeF SizeF::division(const SizeF& size, float value) {
        return SizeF(size.width / value, size.height / value);
    }

    SizeF SizeF::division(const SizeF& size, int value) {
        return division(size, (float)(value));
    }

    SizeFs::SizeFs(size_t capacity) : Vector<SizeF>(capacity) {
    }

    SizeFs::SizeFs(const SizeFs &array) = default;

    SizeFs::SizeFs(std::initializer_list<SizeF> list) : Vector<SizeF>(list) {
    }

    // size:{0, 0};size:{300, 300}
    // 0, 0;300, 300
    bool SizeFs::parse(const String &str, SizeFs &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                SizeF point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "size", true) &&
                            SizeF::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (SizeF::parse(texts[i], point)) {
                        points.add(point);
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    String SizeFs::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const SizeF &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }

    const Size Size::Empty;
    const Size Size::MinValue = Size(INT32_MIN, INT32_MIN);
    const Size Size::MaxValue = Size(INT32_MAX, INT32_MAX);

    Size::Size(int width, int height) {
        this->width = width;
        this->height = height;
    }

    Size::Size(const Size &size) {
        this->width = size.width;
        this->height = size.height;
    }

    Size::~Size() = default;

    bool Size::equals(const Size &other) const {
        return this->width == other.width && this->height == other.height;
    }

    void Size::evaluates(const Size &other) {
        this->width = other.width;
        this->height = other.height;
    }

    int Size::compareTo(const Size &other) const {
        if (width != other.width) {
            if (width > other.width) {
                return 1;
            }
            return -1;
        }
        if (height != other.height) {
            if (height > other.height) {
                return 1;
            }
            return -1;
        }
        return 0;
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

    Size &Size::operator=(const Size &value) {
        this->width = value.width;
        this->height = value.height;
        return *this;
    }

    Size Size::operator+=(const Size &other) {
        *this = Size::add(*this, other);
        return *this;
    }

    Size Size::operator+(const Size &other) const {
        return Size::add(*this, other);
    }

    Size Size::operator-=(const Size &other) {
        *this = subtract(*this, other);
        return *this;
    }

    Size Size::operator-(const Size &other) const {
        return Size::subtract(*this, other);
    }

    Size Size::operator*=(float value) {
        *this = Size::multiply(*this, value);
        return *this;
    }

    Size Size::operator*(float value) const {
        return Size::multiply(*this, value);
    }

    Size Size::operator*=(int value) {
        *this = Size::multiply(*this, value);
        return *this;
    }

    Size Size::operator*(int value) const {
        return Size::multiply(*this, value);
    }

    Size Size::operator/=(float value) {
        *this = Size::division(*this, value);
        return *this;
    }

    Size Size::operator/(float value) const {
        return Size::division(*this, value);
    }

    Size Size::operator/=(int value) {
        *this = Size::division(*this, value);
        return *this;
    }

    Size Size::operator/(int value) const {
        return Size::division(*this, value);
    }

    Size::operator SizeF() const {
        return {width, height};
    }

    void Size::add(const Size &size) {
        *this = Size::add(*this, size);
    }

    void Size::subtract(const Size &size) {
        *this = Size::subtract(*this, size);
    }

    void Size::multiply(float value) {
        *this = Size::multiply(*this, value);
    }

    void Size::multiply(int value) {
        *this = Size::multiply(*this, value);
    }

    void Size::division(float value) {
        *this = Size::division(*this, value);
    }

    void Size::division(int value) {
        *this = Size::division(*this, value);
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

    Size Size::add(const Size &sz1, const Size &sz2) {
        return Size(sz1.width + sz2.width, sz1.height + sz2.height);
    }

    Size Size::subtract(const Size &sz1, const Size &sz2) {
        return Size(sz1.width - sz2.width, sz1.height - sz2.height);
    }

    Size Size::multiply(const Size& size, float value) {
        return Size((int)Math::round((float)size.width * value), (int)Math::round((float)size.height * value));
    }

    Size Size::multiply(const Size& size, int value) {
        return multiply(size, (float)(value));
    }

    Size Size::division(const Size& size, float value) {
        return Size((int)Math::round((float)size.width / value), (int)Math::round((float)size.height / value));
    }

    Size Size::division(const Size& size, int value) {
        return division(size, (float)(value));
    }

    Size Size::ceiling(const SizeF &size) {
        return Size((int) Math::ceiling(size.width), (int) Math::ceiling(size.height));
    }

    Size Size::round(const SizeF &size) {
        return Size((int) Math::round(size.width), (int) Math::round(size.height));
    }

    Size Size::truncate(const SizeF &size) {
        return Size((int) size.width, (int) size.height);
    }

    Sizes::Sizes(size_t capacity) : Vector<Size>(capacity) {
    }

    Sizes::Sizes(const Sizes &array) = default;

    Sizes::Sizes(std::initializer_list<Size> list) : Vector<Size>(list) {
    }

    // size:{0, 0};size:{300, 300}
    // 0, 0;300, 300
    bool Sizes::parse(const String &str, Sizes &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                Size point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "size", true) &&
                            Size::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (Size::parse(texts[i], point)) {
                        points.add(point);
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    String Sizes::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const Size &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }
}
