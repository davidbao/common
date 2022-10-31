#ifndef MONEY_H
#define MONEY_H

#include "data/ValueType.h"
#include "system/Math.h"
#include "system/Regex.h"

namespace Common
{
	typedef int cent;

	class Money
	{
	public:
		inline static String getYuanString(cent centValue, int pointSize = 2)
		{
			float value = centValue/100.0f;
			return Float(value).toString(pointSize);
		}

		inline static cent getCentInteger(const String& yuanStr)
		{
			static Regex r("^[0-9]+(\\.[0-9]{0,2})?$");
			if(r.match(yuanStr))
			{
				float value = 0.0f;
				int len = 0;
				int result = sscanf(yuanStr.c_str(), "%f%n", &value, &len);
				if(result == 1 && yuanStr.length() == len)
				{
					return (cent)Math::round(value * 100.0f);
				}
			}
			return -1;
		}
	};
}
#endif // MONEY_H
