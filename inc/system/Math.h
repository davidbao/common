#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "system/OsDefine.h"
#if WIN32
#undef min
#undef max
#endif

namespace Common
{
	class Math
	{
	public:
		inline static int32_t min(int32_t a, int32_t b)
		{
			return a < b ? a : b;
		}
		inline static int32_t max(int32_t a, int32_t b)
		{
			return a < b ? b : a;
		}
//        inline static uint min(uint a, uint b)
//        {
//            return a < b ? a : b;
//        }
//        inline static uint max(uint a, uint b)
//        {
//            return a < b ? b : a;
//        }
        inline static int64_t min(int64_t a, int64_t b)
        {
            return a < b ? a : b;
        }
        inline static int64_t max(int64_t a, int64_t b)
        {
            return a < b ? b : a;
        }
//#if !defined(WIN32) && !defined(__ANDROID__) && !defined(__arm__)
        inline static size_t min(size_t a, size_t b)
        {
            return a < b ? a : b;
        }
        inline static size_t max(size_t a, size_t b)
        {
            return a < b ? b : a;
        }
//#endif
        inline static float min(float a, float b)
        {
            return a < b ? a : b;
        }
        inline static float max(float a, float b)
        {
            return a < b ? b : a;
        }
        inline static double min(double a, double b)
        {
            return a < b ? a : b;
        }
        inline static double max(double a, double b)
        {
            return a < b ? b : a;
        }
        
        inline static float round(float value)
        {
            return ::roundf(value);
        }
        inline static double round(double value)
        {
            return ::round(value);
        }
        inline static long double round(long double value)
        {
            return ::roundl(value);
        }
        
        inline static double sin(double value)
        {
            return ::sin(value);
        }
        inline static float sin(float value)
        {
            return ::sinf(value);
        }
        inline static double cos(double value)
        {
            return ::cos(value);
        }
        inline static float cos(float value)
        {
            return ::cosf(value);
        }
        inline static double tan(double value)
        {
            return ::tan(value);
        }
        inline static float tan(float value)
        {
            return ::tanf(value);
        }
        inline static double asin(double value)
        {
            return ::asin(value);
        }
        inline static float asin(float value)
        {
            return ::asinf(value);
        }
        inline static double acos(double value)
        {
            return ::acos(value);
        }
        inline static float acos(float value)
        {
            return ::acosf(value);
        }
        inline static double atan(double value)
        {
            return ::atan(value);
        }
        inline static float atan(float value)
        {
            return ::atanf(value);
        }
        inline static double atan2(double y, double x)
        {
            return ::atan2(y, x);
        }
        inline static float atan2(float y, float x)
        {
            return ::atan2f(y, x);
        }
        
        inline static double pow(double a, double b)
        {
            return ::pow(a, b);
        }
        inline static float pow(float a, float b)
        {
            return ::powf(a, b);
        }
        inline static float sqrt(float value)
        {
            return ::sqrtf(value);
        }
        inline static double sqrt(double value)
        {
            return ::sqrt(value);
        }

        inline static int abs(int value)
        {
            return value < 0 ? -value : value;
        }
        inline static float abs(float value)
        {
            return ::fabsf(value);
        }
        inline static double abs(double value)
        {
            return ::fabs(value);
        }
        inline static long double abs(long double value)
        {
            return ::fabsl(value);
        }
        
        inline static double toRadian(double angle)
        {
            return (double)(angle * RadianFactor);
        }
        inline static double toAngle(double radian)
        {
            return (double)(radian * AngleFactor);
        }
        
        static void calcCoordinate(float angle, float l, float& x, float& y);
        static bool calcPolarCoordinate(float x, float y, float& angle, float& l);
        
        static uint getGreatestCommonDivisor(uint a, uint b);
        static uint getSmallestCommonMultiple(uint a, uint b);
        
    public:
        static const double PI;
        static const double RadianFactor;
        static const double AngleFactor;
        static const double E;
	};
}

#endif // MATH_H
