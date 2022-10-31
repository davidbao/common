#ifndef SIZE_H
#define SIZE_H

#include "data/Array.h"
#include "data/ValueType.h"

namespace Common
{
    struct Size;
	struct SizeF
	{
	public:
		SizeF(float width = 0.0f, float height = 0.0f);
		SizeF(int width, int height);
        SizeF(const SizeF& size);

		bool isEmpty() const;
        void empty();
		String toString() const;

		void operator=(const SizeF& value);
		bool operator==(const SizeF& value) const;
		bool operator!=(const SizeF& value) const;
        
        Size round() const;
        
    public:
        static bool parse(const String& str, SizeF& size);

	public:
		float width;
		float height;

		static const SizeF Empty;
        static const SizeF MinValue;
        static const SizeF MaxValue;
	};
    typedef Array<SizeF> SizeFs;
    
    struct Size
    {
    public:
        Size(int width = 0, int height = 0);
        Size(const Size& size);
        
        bool isEmpty() const;
        void empty();
        String toString() const;
        
        void operator=(const Size& value);
        bool operator==(const Size& value) const;
        bool operator!=(const Size& value) const;
        
    public:
        static bool parse(const String& str, Size& size);
        
    public:
        int width;
        int height;
        
        static const Size Empty;
        static const Size MinValue;
        static const Size MaxValue;
    };
    typedef Array<Size> Sizes;
}

#endif	// SIZE_H
