//
//  SqlSelectFilter.cpp
//  auth-center
//
//  Created by baowei on 2020/3/11.
//  Copyright © 2020 com. All rights reserved.
//

#include "database/SqlSelectFilter.h"
#include "json/JsonNode.h"

namespace Database
{
    SqlSelectFilter::SqlSelectFilter(int page, int pageSize) : _page(page), _pageSize(pageSize)
    {
    }

    int SqlSelectFilter::offset() const
    {
        int value = (_page - 1) * _pageSize;
        if(value < 0)
            value = 0;
        return value;
    }
    int SqlSelectFilter::limit() const
    {
        return pageSize();
    }

    int SqlSelectFilter::page() const
    {
        return _page >= 1 ? _page : 1;
    }
    int SqlSelectFilter::pageSize() const
    {
        return _pageSize >= 0 ? _pageSize : 0;
    }

    void SqlSelectFilter::add(const String& key, const String& value)
    {
        _values.add(key, value);
    }
    void SqlSelectFilter::addRange(const String& key, const String& from, const String& to)
    {
        _values.add(String::format("%s.%s", key.c_str(), "from"), from);
        _values.add(String::format("%s.%s", key.c_str(), "to"), to);
    }
    
    String SqlSelectFilter::toLikeStr(const String& key, const String& keyAlias) const
    {
        const char* keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();
        
        String value;
        if(_values.at(key, value) && !value.isNullOrEmpty())
        {
            return String::format("%s like '%%%s%%'", keyStr, value.c_str());
        }
        else
        {
            return "1=1";
        }
    }
    
    String SqlSelectFilter::toEqualTextStr(const String& key, const String& keyAlias) const
    {
        return toEqualStr(key, keyAlias, true);
    }
    String SqlSelectFilter::toEqualNumberStr(const String& key, const String& keyAlias) const
    {
        return toEqualStr(key, keyAlias, false);
    }
    
    String SqlSelectFilter::toRangeTextStr(const String& key, const String& keyAlias) const
    {
        return toRangeStr(key, keyAlias, true);
    }
    String SqlSelectFilter::toRangeNumberStr(const String& key, const String& keyAlias) const
    {
        return toRangeStr(key, keyAlias, false);
    }

    String SqlSelectFilter::toEqualStr(const String& key, const String& keyAlias, bool hasQuotes) const
    {
        const char* keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();
        
        String value;
        if(_values.at(key, value) && !value.isNullOrEmpty())
        {
            return String::format(hasQuotes ? "%s='%s'" : "%s=%s", keyStr, value.c_str());
        }
        else
        {
            return "1=1";
        }
    }
    
    String SqlSelectFilter::toRangeStr(const String& key, const String& keyAlias, bool hasQuotes) const
    {
        const char* keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();
        
        String fromKey = String::format("%s.%s", key.c_str(), "from");
        String toKey = String::format("%s.%s", key.c_str(), "to");
        String fromValue, toValue;
        if(_values.at(fromKey, fromValue) && !fromValue.isNullOrEmpty() &&
           _values.at(toKey, toValue) && !toValue.isNullOrEmpty())
        {
            
            return String::format(hasQuotes ? "(%s>='%s' AND %s<='%s')" : "(%s>=%s AND %s<=%s)",
                                  keyStr, fromValue.c_str(), keyStr, toValue.c_str());
        }
        else
        {
            return "1=1";
        }
    }

    String SqlSelectFilter::getValue(const String& key) const
    {
        String value;
        _values.at(key, value);
        return value;
    }

    bool SqlSelectFilter::parse(const String& str, SqlSelectFilter& filter)
    {
        JsonNode node;
        if(JsonNode::parse(str, node))
        {
            StringArray names;
            if(node.getAttributeNames(names))
            {
                int offset = -1;
                int index;
                String value, key, from, to;
                for (uint i=0; i<names.count(); i++)
                {
                    String name = names[i];
                    if(node.getAttribute(name, value))
                    {
                        if((index = name.find("From")) > 0)
                        {
                            from = value;
                            key = name.substr(0, index);
                        }
                        else if((index = name.find("To")) > 0)
                        {
                            to = value;
                            String temp = name.substr(0, index);
                            if(temp == key)
                                filter.addRange(key, from, to);
                        }
                        else
                        {
                            key = name;
                            from.empty();
                            to.empty();
                            
                            if(name == "page")
                            {
                                Int32::parse(value, filter._page);
                                if(filter._page <= 0)
                                    filter._page = 1;
                                offset = -1;
                            }
                            else if(name == "pageSize")
                                Int32::parse(value, filter._pageSize);
                            else if(name == "offset")
                                Int32::parse(value, offset);
                            else if(name == "limit")
                                Int32::parse(value, filter._pageSize);
                            else
                                filter.add(key, value);
                        }
                    }
                }
                if(offset >= 0)
                    filter._page = offset / filter._pageSize + 1;
                return true;
            }
        }
        return false;
    }
}
