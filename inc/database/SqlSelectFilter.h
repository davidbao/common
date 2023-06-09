//
//  SqlSelectFilter.h
//  common
//
//  Created by baowei on 2020/3/11.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef SqlSelectFilter_h
#define SqlSelectFilter_h

#include "data/String.h"
#include "data/StringMap.h"

using namespace Data;

namespace Database {
    class OrderByItem : public IEvaluation<OrderByItem>, public IEquatable<OrderByItem> {
    public:
        String name;
        bool asc;

        explicit OrderByItem(const String &name = String::Empty, bool asc = true);

        OrderByItem(const OrderByItem &item);

        ~OrderByItem() override;

        void evaluates(const OrderByItem &other) override;

        bool equals(const OrderByItem &other) const override;

        OrderByItem &operator=(const OrderByItem &other);

        bool isEmpty() const;
    };

    typedef Vector<OrderByItem> OrderByItems;

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

        String toArrayLikeStr(const String &key, const String &keyAlias, const char &splitSymbol = ';') const;

        String toEqualTextStr(const String &key, const String &keyAlias = String::Empty) const;

        String toEqualNumberStr(const String &key, const String &keyAlias = String::Empty) const;

        String toRangeTextStr(const String &key, const String &keyAlias = String::Empty) const;

        String toRangeNumberStr(const String &key, const String &keyAlias = String::Empty) const;

        String getValue(const String &key) const;

        bool getRangeValue(const String &key, String &fromValue, String &toValue) const;

        String getFromValue(const String &key) const;

        String getToValue(const String &key) const;

        String orderBy() const;

        void setOrderBy(const String &orderBy);

        String toSelectSql(const String &tableName, const String &columnStr) const;

        String toSelectSql(const String &tableName) const;

        String toCountSql(const String &tableName) const;

        const StringMap &values() const;

    public:
        static bool parse(const String &str, SqlSelectFilter &filter);

        static bool parseOrderBy(const String &str, OrderByItems &items);

    private:
        String toEqualStr(const String &key, const String &keyAlias, bool hasQuotes) const;

        String toRangeStr(const String &key, const String &keyAlias, bool hasQuotes) const;

    private:
        StringMap _values;
        String _orderBy;

        int _page;
        int _pageSize;

    private:
        static constexpr const char *FromStr = "from";
        static constexpr const char *ToStr = "to";

        static constexpr const char *AscStr = "asc";
        static constexpr const char *DescStr = "desc";
    };
}

#endif // SqlSelectFilter_h
