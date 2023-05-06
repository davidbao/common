//
//  HttpContentTest.cpp
//  common
//
//  Created by baowei on 2023/5/5.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "http/HttpContent.h"

using namespace Http;

bool testHttpHeader() {
    {
        HttpHeader test;
        if (!(test.name.isNullOrEmpty() && test.value.isNullOrEmpty())) {
            return false;
        }
    }
    {
        HttpHeader test("test");
        if (!(test.name == "test" && test.value.isNullOrEmpty())) {
            return false;
        }
    }
    {
        HttpHeader test("test", "test2");
        if (!(test.name == "test" && test.value == "test2")) {
            return false;
        }
    }
    {
        HttpHeader test("test", "test2");
        HttpHeader test2(test);
        if (!(test2.name == "test" && test2.value == "test2")) {
            return false;
        }
    }
    {
        HttpHeader test("test", "test2");
        HttpHeader test2 = test;
        if (!(test2.name == "test" && test2.value == "test2")) {
            return false;
        }
    }

    {
        HttpHeader test("test", "test2");
        HttpHeader test2;
        test2.evaluates(test);
        if (!(test2.name == "test" && test2.value == "test2")) {
            return false;
        }
    }
    {
        HttpHeader test("test", "test2");
        HttpHeader test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        HttpHeader test("test", "test2");
        HttpHeader test2;
        test2.evaluates(test);
        if (test2 != test) {
            return false;
        }
    }

    {
        HttpHeader test("test", "test2");
        if (test.toString() != "test:test2") {
            return false;
        }
    }

    return true;
}

bool testHttpHeaders() {
    {
        HttpHeaders test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        HttpHeaders test(100);
        if (!test.isEmpty()) {
            return false;
        }
        if (test.capacity() != 100) {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        if (test.isEmpty()) {
            return false;
        }
        if (test.at("Content-Type") != "application/json") {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        const HttpHeaders &test2(test);
        if (test2.isEmpty()) {
            return false;
        }
        if (test2.at("Content-Type") != "application/json") {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        const HttpHeaders &test2 = test;
        if (test2.isEmpty()) {
            return false;
        }
        if (test2.at("Content-Type") != "application/json") {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json"),
                          HttpHeader("Access-Control-Allow-Origin", "*")});
        HttpHeaders test2(test, 1, 1);
        if (test2.count() != 1) {
            return false;
        }
        if (test2.at("Access-Control-Allow-Origin") != "*") {
            return false;
        }
    }
    {
        HttpHeader test[] = {HttpHeader("Content-Type", "application/json"),
                             HttpHeader("Access-Control-Allow-Origin", "*")};
        HttpHeaders test2(test + 1, 1);
        if (test2.count() != 1) {
            return false;
        }
        if (test2.at("Access-Control-Allow-Origin") != "*") {
            return false;
        }
    }

    {
        const HttpHeaders &test = HttpHeaders::JsonTypeHeaders;
        if (test.at("Content-Type") != "application/json") {
            return false;
        }
    }

    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        String value;
        if (!(test.getValue("Content-Type", value) && value == "application/json")) {
            return false;
        }
    }

    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        if (!test.contains("Content-Type")) {
            return false;
        }
    }

    {
        HttpHeaders test;
        test.add("Content-Type", "application/json");
        if (!test.contains("Content-Type")) {
            return false;
        }
    }
    {
        HttpHeaders test;
        test.add("Content-Type", "application/json");
        test.add("Content-Type", "application/json");
        if (test.count() != 1) {
            return false;
        }
        if (!test.contains("Content-Type")) {
            return false;
        }
    }

    {
        HttpHeaders test({HttpHeader("Content-Type", "application/json")});
        if (!test.isTextContent()) {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "text/xml")});
        if (!test.isTextContent()) {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "javascript")});
        if (!test.isTextContent()) {
            return false;
        }
    }
    {
        HttpHeaders test({HttpHeader("Content-Type", "application/x-www-form-urlencoded")});
        if (!test.isTextContent()) {
            return false;
        }
    }

    {
        HttpHeaders test;
        if (!HttpHeaders::parse("Content-Type: application/x-www-form-urlencoded\n"
                                "Access-Control-Allow-Origin: *", test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test.at("Content-Type") != "application/x-www-form-urlencoded") {
            return false;
        }
        if (test.at("Access-Control-Allow-Origin") != "*") {
            return false;
        }
    }

    return true;
}

bool testHttpCookie() {
    {
        HttpCookie test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        HttpCookie test;
        if (!HttpCookie::parse("a=1;b=2;c=3;", test)) {
            return false;
        }
        if (test.isEmpty()) {
            return false;
        }
        HttpCookie test2(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        HttpCookie test;
        if (!HttpCookie::parse("a=1;b=2;c=3;", test)) {
            return false;
        }
        if (test.isEmpty()) {
            return false;
        }
        HttpCookie test2 = test;
        if (test2 != test) {
            return false;
        }
    }
    {
        HttpCookie test;
        if (!HttpCookie::parse("a=1;b=2;c=3;", test)) {
            return false;
        }
        if (test.isEmpty()) {
            return false;
        }
        HttpCookie test2 = test;
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        HttpCookie test;
        if (!HttpCookie::parse("a=1;b=2;c=3;", test)) {
            return false;
        }
        if (test.isEmpty()) {
            return false;
        }
        HttpCookie test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    {
        HttpCookie test;
        if (!HttpCookie::parse("a=1;b=2;c=3;", test)) {
            return false;
        }
        test.setDomain("domain");
        if (test.domain() != "domain") {
            return false;
        }
        test.setPath("path");
        if (test.path() != "path") {
            return false;
        }
        test.setExpires("expires");
        if (test.expires() != "expires") {
            return false;
        }
        test.setMaxAge(123);
        if (test.maxAge() != 123) {
            return false;
        }
        test.setSecure(true);
        if (!test.secure()) {
            return false;
        }
        test.setSecure(false);
        if (test.secure()) {
            return false;
        }
        test.setHttpOnly(true);
        if (!test.httpOnly()) {
            return false;
        }
        test.setHttpOnly(false);
        if (test.httpOnly()) {
            return false;
        }
        test.setSameSite(HttpCookie::Lax);
        if (test.sameSite() != HttpCookie::Lax) {
            return false;
        }
    }

    {
        HttpCookie test;
        test.add("a", "1");
        if (test.at("a") != "1") {
            return false;
        }
    }
    {
        HttpCookie test;
        test.add("a", "1");
        String value;
        if (!test.getValue("a", value)) {
            return false;
        }
        if (value != "1") {
            return false;
        }
    }

    {
        HttpCookie test;
        test.add("a", "1");
        if (test.toString() != "a=1") {
            return false;
        }
    }

    return true;
}

bool testHttpSession() {
    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test");
        if (test.token() != token) {
            return false;
        }
        if (test.name() != "test") {
            return false;
        }
        if (test.expiredTime() != TimeSpan::Zero) {
            return false;
        }
        if (test.kickout()) {
            return false;
        }
    }
    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test", TimeSpan::fromMinutes(1), true);
        if (test.token() != token) {
            return false;
        }
        if (test.name() != "test") {
            return false;
        }
        if (test.expiredTime() != TimeSpan::fromMinutes(1)) {
            return false;
        }
        if (!test.kickout()) {
            return false;
        }
    }
    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test", TimeSpan::fromMinutes(1), true);
        HttpSession test2(test);
        if (test2.token() != token) {
            return false;
        }
        if (test2.name() != "test") {
            return false;
        }
        if (test2.expiredTime() != TimeSpan::fromMinutes(1)) {
            return false;
        }
        if (!test2.kickout()) {
            return false;
        }
    }
    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test", TimeSpan::fromMinutes(1), true);
        HttpSession test2 = test;
        if (test2.token() != token) {
            return false;
        }
        if (test2.name() != "test") {
            return false;
        }
        if (test2.expiredTime() != TimeSpan::fromMinutes(1)) {
            return false;
        }
        if (!test2.kickout()) {
            return false;
        }
    }

    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test", TimeSpan::fromMinutes(1), true);
        HttpSession test2 = test;
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        String token = Uuid::generate().toString();
        HttpSession test(token, "test", TimeSpan::fromMinutes(1), true);
        HttpSession test2(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testHttpSessions() {
    {
        HttpSessions test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        HttpSessions test;
        String token = Uuid::generate().toString();
        auto session = new HttpSession(token, "test");
        test.add(token, session);
        if (test.count() != 1) {
            return false;
        }
    }

    {
        HttpSessions test;
        String token = Uuid::generate().toString();
        auto session = new HttpSession(token, "test");
        test.add(token, session);
        if (!test.contains(token)) {
            return false;
        }
    }
    {
        HttpSessions test;
        String token = Uuid::generate().toString();
        auto session = new HttpSession(token, "test");
        test.add(token, session);
        if (test.at(token) == nullptr) {
            return false;
        }
    }
    {
        HttpSessions test;
        String token = Uuid::generate().toString();
        auto session = new HttpSession(token, "test");
        test.add(token, session);
        if (test.atByName("test") == nullptr) {
            return false;
        }
    }

    return true;
}

bool testHttpContent() {
    {
        String str = HttpContent::escape("http://127.0.0.1/1#");
        if (str != "http%3A%2F%2F127.0.0.1%2F1%23") {
            return false;
        }
    }
    {
        String str = HttpContent::unescape("http%3A%2F%2F127.0.0.1%2F1%23");
        if (str != "http://127.0.0.1/1#") {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testHttpHeader()) {
        return 1;
    }

    if (!testHttpHeaders()) {
        return 2;
    }

    if (!testHttpCookie()) {
        return 3;
    }

    if (!testHttpSession()) {
        return 4;
    }

    if (!testHttpSessions()) {
        return 5;
    }

    if (!testHttpContent()) {
        return 6;
    }

    return 0;
}