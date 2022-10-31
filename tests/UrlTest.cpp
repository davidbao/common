//
//  UrlTest.cpp
//  common
//
//  Created by baowei on 2022/9/24.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/NetType.h"

using namespace Common;

bool testParse_1() {
    String str = "https://www.baidu.com/";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "https") && (url.endpoint() == Endpoint("www.baidu.com", 443));
}

bool testParse_2() {
    String str = "https://www.baidu.com";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "https") && (url.endpoint() == Endpoint("www.baidu.com", 443));
}

bool testParse_3() {
    String str = "https://www.baidu.com:443/";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "https") && (url.endpoint() == Endpoint("www.baidu.com", 443));
}

bool testParse_4() {
    String str = "https://www.baidu.com:443";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "https") && (url.endpoint() == Endpoint("www.baidu.com", 443));
}

bool testParse_5() {
        String str = "https://www.baidu.com:443/test.html";
        Url url(str);
        return(!url.isEmpty()) && (url.scheme() == "https") && (url.endpoint() == Endpoint("www.baidu.com", 443));
}

bool testParse_6() {
        String str = "http://www.baidu.com:8080/test.html";
        Url url(str);
        return(!url.isEmpty()) && (url.scheme() == "http") && (url.endpoint() == Endpoint("www.baidu.com", 8080));
}

bool testParse_7() {
    String str = "http://localhost:8080/test.html";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "http") && (url.endpoint() == Endpoint("localhost", 8080));
}

bool testParse_8() {
        String str = "http://localhost:8080/Debug/test.html?accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
        Url url(str);
        return(!url.isEmpty()) && (url.scheme() == "http") && (url.endpoint() == Endpoint("localhost", 8080));
}

bool testParse_9() {
    String str = "http://192.168.1.6:8080/Debug/test.html?address=192.168.1.125&accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
    Url url(str);
    return (!url.isEmpty()) && (url.scheme() == "http") && (url.endpoint() == Endpoint("192.168.1.6", 8080));
}

int main() {
    if(!testParse_1())
        return 1;
    if(!testParse_2())
        return 2;
    if(!testParse_3())
        return 3;
    if(!testParse_4())
        return 4;
    if(!testParse_5())
        return 5;
    if(!testParse_6())
        return 6;
    if(!testParse_7())
        return 7;
    if(!testParse_8())
        return 8;
    if(!testParse_9())
        return 9;
    return 0;
}

