//
//  SqlSelectFilterTest.cpp
//  common
//
//  Created by baowei on 2022/11/30.
//  Copyright © 2022 com. All rights reserved.
//

#include "database/SqlSelectFilter.h"

using namespace Database;

bool testConstructor() {
    {
        SqlSelectFilter test;
        if (!(test.page() == 1 && test.pageSize() == 1)) {
            return false;
        }
    }
    {
        SqlSelectFilter test(1, 100, {
                {"name", "Xu"},
                {"age", "90"}
        });
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
        if(test.getValue("name") != "Xu") {
            return false;
        }
        if(test.getValue("age") != "90") {
            return false;
        }
    }
    {
        SqlSelectFilter test(1, 100);
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
    }
    {
        SqlSelectFilter test(1, 100);
        SqlSelectFilter test2(test);
        if (!(test2.offset() == 0 && test2.limit() == 100)) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        SqlSelectFilter test(1, 100);
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
    }

    {
        SqlSelectFilter test(1, 100, {
                {"name", "Xu"},
                {"age", "90"}
        });
        if(test.toLikeStr("name") != "name like '%Xu%'") {
            return false;
        }
        if(test.toLikeStr("name", "fullName") != "fullName like '%Xu%'") {
            return false;
        }
        if(test.toLikeStr("name2") != "1=1") {
            return false;
        }
        if(test.toEqualTextStr("name") != "name='Xu'") {
            return false;
        }
        if(test.toEqualTextStr("name", "fullName") != "fullName='Xu'") {
            return false;
        }
        if(test.toEqualTextStr("name2") != "1=1") {
            return false;
        }
        if(test.toEqualNumberStr("age") != "age=90") {
            return false;
        }
    }

    {
        SqlSelectFilter test(1, 100, {
                {"name", "Xu"},
                {"age",  "90"}
        });
        test.addRange("time", "2010-01-01", "2010-01-02");
        if(test.toRangeTextStr("time") != "(time>='2010-01-01' AND time<='2010-01-02')") {
            return false;
        }
    }

    {
        SqlSelectFilter test(1, 100, {
                {"name", "Xu"}
        });
        test.addRange("age", "1", "18");
        if(test.toRangeNumberStr("age") != "(age>=1 AND age<=18)") {
            return false;
        }
    }

    return true;
}

bool testKeyValue() {
    {
        SqlSelectFilter test(1, 100);
        test.add("name", "Xu");
        if(test.getValue("name") != "Xu") {
            return false;
        }
    }
    {
        SqlSelectFilter test(1, 100);
        test.add("name", "Xu");
        test.addRange("time", "2010-01-01", "2010-01-02");
        if(test.getValue("time.from") != "2010-01-01") {
            return false;
        }
        if(test.getValue("time.to") != "2010-01-02") {
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        String str = "{\n"
                     "    \"name\":\"Xu\",\n"
                     "    \"age\":90,\n"
                     "    \"page\":1,\n"
                     "    \"pageSize\":100\n"
                     "}";
        SqlSelectFilter test;
        if (!SqlSelectFilter::parse(str, test)) {
            return false;
        }
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
        if(test.getValue("name") != "Xu") {
            return false;
        }
        if(test.getValue("age") != "90") {
            return false;
        }
    }

    {
        String str = "{\n"
                     "    \"name\":\"Xu\",\n"
                     "    \"age\":90,\n"
                     "    \"page\":0,\n"
                     "    \"pageSize\":0\n"
                     "}";
        SqlSelectFilter test;
        if (!SqlSelectFilter::parse(str, test)) {
            return false;
        }
        if (!(test.page() == 1 && test.pageSize() == 1)) {
            return false;
        }
        if(test.getValue("name") != "Xu") {
            return false;
        }
        if(test.getValue("age") != "90") {
            return false;
        }
    }

    {
        String str = "{\n"
                     "    \"name\":\"Xu\",\n"
                     "    \"age\":90,\n"
                     "    \"offset\":0,\n"
                     "    \"limit\":100\n"
                     "}";
        SqlSelectFilter test;
        if (!SqlSelectFilter::parse(str, test)) {
            return false;
        }
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
        if(test.getValue("name") != "Xu") {
            return false;
        }
        if(test.getValue("age") != "90") {
            return false;
        }
    }

    {
        String str = "{\n"
                     "    \"name\":\"Xu\",\n"
                     "    \"ageFrom\":1,\n"
                     "    \"ageTo\":18,\n"
                     "    \"offset\":0,\n"
                     "    \"limit\":100\n"
                     "}";
        SqlSelectFilter test;
        if (!SqlSelectFilter::parse(str, test)) {
            return false;
        }
        if (!(test.page() == 1 && test.pageSize() == 100)) {
            return false;
        }
        if(test.getValue("name") != "Xu") {
            return false;
        }
        if(test.getValue("age.from") != "1") {
            return false;
        }
        if(test.getValue("age.to") != "18") {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testProperty()) {
        return 2;
    }
    if (!testKeyValue()) {
        return 3;
    }
    if (!testParse()) {
        return 4;
    }

    return 0;
}