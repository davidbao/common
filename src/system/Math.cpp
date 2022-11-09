//
//  Math.cpp
//  common
//
//  Created by baowei on 2015/9/6.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Math.h"

namespace Common {
    const double Math::PI = 3.14159265358979323846;
    const double Math::RadianFactor = PI / 180.0;
    const double Math::AngleFactor = 180.0 / PI;
    const double Math::E = 2.7182818284590452354;

    void Math::calcCoordinate(float angle, float l, float &x, float &y) {
        if (angle == 0.0f && l == 0.0f) {
            x = y = 0;
        } else {
            float radian = toRadian(angle);
            x = (float) round(l * cos(radian));
            y = (float) round(l * sin(radian));
        }
    }

    bool Math::calcPolarCoordinate(float x, float y, float &angle, float &l) {
        if (x == 0.0f && y == 0.0f)
            return false;

        bool changed = true;
        float r = 0.0f;
        float length = (float) round(::sqrtf(x * x + y * y));
        if (y == 0) {
            changed = false;
            angle = x > 0 ? 0.0f : 180.0f;            // 0 or 180
        } else if (y > 0) {
            r = asin((float) y / (float) length);        // from 0 to 90
            if (x < 0)
                r = (float) PI - r;                    // from 90 to 180
            else if (x == 0) {
                changed = false;
                angle = 90.0f;                        // 90
            }
        } else // y < 0
        {
            r = asin((float) y / (float) length);        // from -90 to 0
            if (x < 0)
                r = (float) PI - r;                    // from 180 to 270
            else if (x == 0) {
                changed = false;
                angle = 270.0f;                        // 270
            }
        }
        if (changed) {
            angle = toAngle(r);
            l = length;
            return true;
        }
        return false;
    }
}
