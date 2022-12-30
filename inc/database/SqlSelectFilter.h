//
//  SqlSelectFilter.h
//  common
//
//  Created by baowei on 2020/3/11.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef SqlSelectFilter_h
#define SqlSelectFilter_h

#include "data/ValueType.h"
#include "data/StringMap.h"

using namespace Data;

namespace Database {
    class SqlSelectFilter {
    public:
        explicit SqlSelectFilter(int page = 0, int pageSize = 0);

        SqlSelectFilter(int page, int pageSize, std::initializer_list<StringMap::ValueType> list);

        SqlSelectFilter(const SqlSelectFilter &filter);

        int offset() const;

        int limit() const;

        bool hasLimit() const;

        int page() const;

        int pageSize() const;

        void add(const String &key, const String &value);

        void addRange(const String &key, const String &from, const String &to);

        String toLikeStr(const String &key, const String &keyAlias = String::Empty) const;

        String toEqualTextStr(const String &key, const String &keyAlias = String::Empty) const;

        String toEqualNumberStr(const String &key, const String &keyAlias = String::Empty) const;

        String toRangeTextStr(const String &key, const String &keyAlias = String::Empty) const;

        String toRangeNumberStr(const String &key, const String &keyAlias = String::Empty) const;

        String getValue(const String &key) const;

        String orderBy() const;

        void setOrderBy(const String &orderBy);

        String toQuerySql(const String &tableName) const;

        String toCountSql(const String &tableName) const;

    public:
        static bool parse(const String &str, SqlSelectFilter &filter);

    private:
        String toEqualStr(const String &key, const String &keyAlias, bool hasQuotes) const;

        String toRangeStr(const String &key, const String &keyAlias, bool hasQuotes) const;

        String toSql(const String &tableName, const String &columnStr) const;

    private:
        StringMap _values;
        String _orderBy;

        int _page;
        int _pageSize;
    };
}

#endif // SqlSelectFilter_h
