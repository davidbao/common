#ifndef POINT_H
#define POINT_H

#include "data/Array.h"
#include "data/ValueType.h"

namespace Common
{
    struct Point;
	struct PointF
	{
	public:
		PointF(float x = 0.0f, float y = 0.0f);
        PointF(int x, int y);
        PointF(long x, long y);
        PointF(const PointF& point);
        
		bool isEmpty() const;
        void empty();
		String toString() const;

		void operator=(const PointF& value);
		bool operator==(const PointF& value) const;
		bool operator!=(const PointF& value) const;
        
        void offset(const PointF& pos);
        void offset(float dx, float dy);
        
        Point round() const;
        
    public:
        static bool parse(const String& str, PointF& point);

	public:
		float x;
		float y;

		static const PointF Empty;
        static const PointF MinValue;
        static const PointF MaxValue;
	};
    
    class PointFs : public Array<PointF>
    {
    public:
        PointFs(uint capacity = Array<PointF>::DefaultCapacity);
        PointFs(const PointFs& array);
        
        String toString(const char split = ';') const;
        
        static bool parse(const String& str, PointFs& points);
    };
    
    struct Point
    {
    public:
        Point(int x = 0, int y = 0);
        Point(const Point& point);
        
        bool isEmpty() const;
        void empty();
        String toString() const;
        
        void operator=(const Point& value);
        bool operator==(const Point& value) const;
        bool operator!=(const Point& value) const;
        
        void offset(const Point& pos);
        void offset(int dx, int dy);
        
    public:
        static bool parse(const String& str, Point& point);
        
    public:
        int x;
        int y;
        
        static const Point Empty;
        static const Point MinValue;
        static const Point MaxValue;
    };
    
    class Points : public Array<Point>
    {
    public:
        Points(uint capacity = Array<Point>::DefaultCapacity);
        
        static bool parse(const String& str, Points& points);
    };
}

#endif	// POINT_H
