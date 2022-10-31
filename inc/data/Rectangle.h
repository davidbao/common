#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "data/ValueType.h"
#include "Point.h"
#include "Size.h"

namespace Common
{
    struct Rectangle;
	struct RectangleF
	{
	public:
		RectangleF(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f);
		RectangleF(PointF location, SizeF size);
        RectangleF(const RectangleF& size);
        
		bool isEmpty() const;
        void empty();
		String toString() const;

		PointF location() const;
        void setLocation(const PointF& location);
		SizeF size() const;
        void setSize(const SizeF& size);

		float left() const;
		float top() const;
		float right() const;
		float bottom() const;
        
        PointF leftTop() const;
        PointF rightTop() const;
        PointF leftBottom() const;
        PointF rightBottom() const;

		void operator=(const RectangleF& value);
		bool operator==(const RectangleF& value) const;
		bool operator!=(const RectangleF& value) const;

		bool contains(float x, float y) const;
		bool contains(const PointF& point) const;
		bool contains(const RectangleF& rect) const;

		void inflate(float width, float height);
		void inflate(const SizeF& size);

		void intersect(const RectangleF& rect);
		bool intersectsWith(const RectangleF& rect) const;

		void offset(const PointF& pos);
		void offset(float dx, float dy);
        
        void unions(const RectangleF& rect);
        
        PointF center() const;
        
        Rectangle round() const;

	public:
		static RectangleF intersect(const RectangleF& a, const RectangleF& b);

		// Creates a rectangle that represents the union between a and b.
		static RectangleF unions(const RectangleF& a, const RectangleF& b);

        static bool parse(const String& str, RectangleF& rect);
        
        static RectangleF makeLTRB(float left, float top, float right, float bottom);
        static RectangleF makeLTRB(const PointF& leftTop, const PointF& rightBottom);

	public:
		float x;
		float y;
		float width;
		float height;

		static const RectangleF Empty;
        static const RectangleF MaxValue;
	};
    typedef Array<RectangleF> RectangleFs;
    
    struct Rectangle
    {
    public:
        Rectangle(int x = 0, int y = 0, int width = 0, int height = 0);
        Rectangle(Point location, Size size);
        Rectangle(const Rectangle& rect);
        
        bool isEmpty() const;
        void empty();
        String toString() const;
        
        Point location() const;
        void setLocation(const Point& location);
        Size size() const;
        void setSize(const Size& size);
        
        int left() const;
        int top() const;
        int right() const;
        int bottom() const;
        
        void operator=(const Rectangle& value);
        bool operator==(const Rectangle& value) const;
        bool operator!=(const Rectangle& value) const;
        
        bool contains(int x, int y) const;
        bool contains(const Point& point) const;
        bool contains(const Rectangle& rect) const;
        
        void inflate(int width, int height);
        void inflate(const Size& size);
        
        void intersect(const Rectangle& rect);
        bool intersectsWith(const Rectangle& rect) const;
        
        void offset(const Point& pos);
        void offset(int dx, int dy);
        
        void unions(const Rectangle& rect);
        
        Point center() const;
        
    public:
        static Rectangle intersect(const Rectangle& a, const Rectangle& b);
        
        // Creates a rectangle that represents the union between a and b.
        static Rectangle unions(const Rectangle& a, const Rectangle& b);
        
        static bool parse(const String& str, Rectangle& rect);
        
        static Rectangle makeLTRB(int left, int top, int right, int bottom);
        
    public:
        int x;
        int y;
        int width;
        int height;
        
        static const Rectangle Empty;
        static const Rectangle MaxValue;
    };
    typedef Array<Rectangle> Rectangles;
}

#endif	// RECTANGLE_H
