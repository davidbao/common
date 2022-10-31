//
//  Culture.h
//  common
//
//  Created by baowei on 2015/8/29.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Culture_h
#define Culture_h

#include "data/ValueType.h"

namespace Common
{
    class Culture
    {
    public:
        class Data
        {
        public:
            uint lcid;
            String name;
            String region;
            String language;
            uint ansiPageCode;
            uint oemPageCode;
            
            Data(uint lcid = 0x0009, const String& name = "en", const String& region = "English", const String& language = "English", uint ansiPageCode = 1252, uint oemPageCode = 437);
            Data(const Data& data);
            ~Data();

            Data& operator=(const Data& value);
			bool operator==(const Data& value) const;
			bool operator!=(const Data& value) const;
        };
        
        Culture();
        Culture(const String& name);
        Culture(uint lcid);
        Culture(const Culture& culture);
        ~Culture();
        
        const String& name() const;
        uint lcid() const;
        const String& region() const;
        const String& language() const;
        uint ansiPageCode() const;
        uint oemPageCode() const;

        Culture& operator=(const Culture& value);
		bool operator==(const Culture& value) const;
		bool operator!=(const Culture& value) const;

        bool isChinese() const;

    private:
        Data _data;
        
        static const int AllCulturesCount = 273;
        static const Culture::Data* AllCultures[AllCulturesCount];
    };
}

#endif // Culture_h
