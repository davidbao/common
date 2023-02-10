//
//  SqlSelectFilter.cpp
//  common
//
//  Created by baowei on 2020/3/11.
//  Copyright © 2020 com. All rights reserved.
//

#include "database/SqlSelectFilter.h"
#include "data/Convert.h"
#include "json/JsonNode.h"

using namespace Json;

namespace Database {
    SqlSelectFilter::SqlSelectFilter(int page, int pageSize) : _page(page), _pageSize(pageSize) {
    }

    SqlSelectFilter::SqlSelectFilter(int page, int pageSize, std::initializer_list<StringMap::ValueType> list)
            : _page(page), _pageSize(pageSize), _values(list) {
    }

    SqlSelectFilter::SqlSelectFilter(const SqlSelectFilter &filter) : _page(filter._page), _pageSize(filter._pageSize),
                                                                      _values(filter._values) {
    }

    int SqlSelectFilter::offset() const {
        int value = (_page - 1) * _pageSize;
        if (value < 0)
            value = 0;
        return value;
    }

    int SqlSelectFilter::limit() const {
        return pageSize();
    }

    bool SqlSelectFilter::hasLimit() const {
        return !(_page == 0 && _pageSize == 0);
    }

    int SqlSelectFilter::page() const {
        return _page >= 1 ? _page : 1;
    }

    int SqlSelectFilter::pageSize() const {
        return _pageSize >= 1 ? _pageSize : 1;
    }

    void SqlSelectFilter::add(const String &key, const String &value) {
        _values.add(key, value);
    }

    void SqlSelectFilter::addRange(const String &key, const String &from, const String &to) {
        _values.add(String::format("%s.%s", key.c_str(), "from"), from);
        _values.add(String::format("%s.%s", key.c_str(), "to"), to);
    }

    String SqlSelectFilter::toLikeStr(const String &key, const String &keyAlias) const {
        const char *keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();

        String value;
        if (_values.at(key, value) && !value.isNullOrEmpty()) {
            return String::format("%s like '%%%s%%'", keyStr, value.c_str());
        } else {
            return "1=1";
        }
    }

    String SqlSelectFilter::toEqualTextStr(const String &key, const String &keyAlias) const {
        return toEqualStr(key, keyAlias, true);
    }

    String SqlSelectFilter::toEqualNumberStr(const String &key, const String &keyAlias) const {
        return toEqualStr(key, keyAlias, false);
    }

    String SqlSelectFilter::toRangeTextStr(const String &key, const String &keyAlias) const {
        return toRangeStr(key, keyAlias, true);
    }

    String SqlSelectFilter::toRangeNumberStr(const String &key, const String &keyAlias) const {
        return toRangeStr(key, keyAlias, false);
    }

    String SqlSelectFilter::toEqualStr(const String &key, const String &keyAlias, bool hasQuotes) const {
        const char *keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();

        String value;
        if (_values.at(key, value) && !value.isNullOrEmpty()) {
            return String::format(hasQuotes ? "%s='%s'" : "%s=%s", keyStr, value.c_str());
        } else {
            return "1=1";
        }
    }

    String SqlSelectFilter::toRangeStr(const String &key, const String &keyAlias, bool hasQuotes) const {
        const char *keyStr = !keyAlias.isNullOrEmpty() ? keyAlias.c_str() : key.c_str();

        String fromKey = String::format("%s.%s", key.c_str(), "from");
        String toKey = String::format("%s.%s", key.c_str(), "to");
        String fromValue, toValue;
        if (_values.at(fromKey, fromValue) && !fromValue.isNullOrEmpty() &&
            _values.at(toKey, toValue) && !toValue.isNullOrEmpty()) {

            return String::format(hasQuotes ? "(%s>='%s' AND %s<='%s')" : "(%s>=%s AND %s<=%s)",
                                  keyStr, fromValue.c_str(), keyStr, toValue.c_str());
        } else {
            return "1=1";
        }
    }

    String SqlSelectFilter::getValue(const String &key) const {
        String value;
        _values.at(key, value);
        return value;
    }

    String SqlSelectFilter::orderBy() const {
        return _orderBy;
    }

    void SqlSelectFilter::setOrderBy(const String &orderBy) {
        _orderBy = orderBy;
    }

    String SqlSelectFilter::toSelectSql(const String &tableName) const {
        return toSelectSql(tableName, "*");
    }

    String SqlSelectFilter::toCountSql(const String &tableName) const {
        return toSelectSql(tableName, "COUNT(1)");
    }

    String SqlSelectFilter::toSelectSql(const String &tableName, const String &columnStr) const {
        String result = String::format("SELECT %s FROM %s",
                                       columnStr.isNullOrEmpty() ? "*" : columnStr.c_str(),
                                       tableName.c_str());

        // where
        if (_values.count() > 0) {
            String whereStr;
            StringMap excluded;
            for (auto it = _values.begin(); it != _values.end(); ++it) {
                const String &k = it.key();
                const String &v = it.value();

                if (!excluded.contains(k)) {
                    String keyStr, fromKey, fromValue, toKey, toValue;

                    StringArray texts;
                    Convert::splitStr(k, texts, '.');
                    if (texts.count() == 2) {
                        keyStr = texts[0];
                        if (texts[1] == "from") {
                            fromKey = k;
                            fromValue = _values[fromKey];
                            toKey = String::format("%s.%s", texts[0].c_str(), "to");
                            toValue = _values[toKey];
                            excluded.add(toKey, toKey);
                        } else if (texts[1] == "to") {
                            toKey = k;
                            toValue = _values[toKey];
                            fromKey = String::format("%s.%s", texts[0].c_str(), "from");
                            fromValue = _values[fromKey];
                            excluded.add(fromKey, fromKey);
                        }
                    }

                    if (!fromValue.isNullOrEmpty() && !toValue.isNullOrEmpty()) {
                        // range.
                        if (!whereStr.isNullOrEmpty()) {
                            whereStr.append(" AND ");
                        }
                        double value;
                        bool hasQuotes = !Double::parse(fromValue, value);
                        whereStr.appendFormat(hasQuotes ? "(%s>='%s' AND %s<='%s')" : "(%s>=%s AND %s<=%s)",
                                              keyStr.c_str(), fromValue.c_str(), keyStr.c_str(), toValue.c_str());
                    } else {
                        // equals.
                        if (!whereStr.isNullOrEmpty()) {
                            whereStr.append(" AND ");
                        }
                        if (v.find("like") >= 0) {
                            String temp = String::replace(v, "like", "%");
                            whereStr.append(String::format("%s like '%s'", k.c_str(), temp.c_str()));
                        } else if (v.find("%") >= 0) {
                            whereStr.append(String::format("%s like '%s'", k.c_str(), v.c_str()));
                        } else if (v.find("quote") >= 0) {
                            String temp = String::replace(v, "quote", "'");
                            whereStr.append(String::format("%s = %s", k.c_str(), temp.c_str()));
                        } else if (v.find('\"') >= 0) {
                            String temp = String::replace(v, "\"", "'");
                            whereStr.append(String::format("%s = %s", k.c_str(), temp.c_str()));
                        } else {
                            whereStr.append(String::format("%s = %s", k.c_str(), v.c_str()));
                        }
                    }
                }
            }

            result.append(" WHERE ");
            result.append(whereStr);
        }

        if (columnStr.find("COUNT") < 0) {
            // order by
            String orderBy = this->orderBy();
            if (!orderBy.isNullOrEmpty()) {
                result.append(" ORDER BY ");
                result.append(orderBy);
            }

            // limit
            if (hasLimit()) {
                result.append(String::format(" LIMIT %d,%d", offset(), limit()));
            }
        }

        return result;
    }

    bool SqlSelectFilter::parse(const String &str, SqlSelectFilter &filter) {
        JsonNode node;
        if (JsonNode::parse(str, node)) {
            StringArray names;
            node.getAttributeNames(names);
            int offset = -1;
            int index;
            String value, key, from, to;
            for (size_t i = 0; i < names.count(); i++) {
                String name = names[i];
                if (node.getAttribute(name, value)) {
                    if ((index = (int) name.find("From")) > 0) {
                        from = value;
                        key = name.substr(0, index);
                    } else if ((index = (int) name.find("To")) > 0) {
                        to = value;
                        String temp = name.substr(0, index);
                        if (temp == key)
                            filter.addRange(key, from, to);
                    } else if (String::equals(name, "orderBy", true)) {
                        filter._orderBy = value;
                    } else {
                        key = name;
                        from.empty();
                        to.empty();

                        if (name == "page" || name == "pageNo") {
                            Int32::parse(value, filter._page);
                            if (filter._page <= 0)
                                filter._page = 1;
                            offset = -1;
                        } else if (name == "pageSize") {
                            Int32::parse(value, filter._pageSize);
                        } else if (name == "offset") {
                            Int32::parse(value, offset);
                        } else if (name == "limit") {
                            Int32::parse(value, filter._pageSize);
                        } else {
                            filter.add(key, value);
                        }
                    }
                }
            }
            if (offset >= 0)
                filter._page = offset / filter.pageSize() + 1;
            return true;
        }
        return false;
    }

    const StringMap &SqlSelectFilter::values() const {
        return _values;
    }
}
