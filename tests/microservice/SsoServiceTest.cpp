//
//  SsoServiceTest.cpp
//  common
//
//  Created by baowei on 2023/7/19.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "microservice/SsoService.h"
#include "crypto/SmProvider.h"
#include "configuration/ConfigService.h"
#include "http/HttpClient.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Microservice;
using namespace Crypto;
using namespace Config;
using namespace Http;

String _accessToken1;
String _accessToken2;
String _accessToken3;

bool testConstructor() {
    {
        ByteArray privateKey, publicKey;
        Sm2Provider::generateKey(publicKey, privateKey);
        Sm2Provider sm2(publicKey, privateKey);
        String input, output;
        sm2.encrypt("abc", output);
        sm2.decrypt(output, input);
        if (input != "abc") {
            return false;
        }
    }
    {
        ByteArray privateKey;
        ByteArray::parseHexString("0127E2238F923C4530CC2EC87B9C3839497CCB11FD519E4F13C211BC49D86DB9", privateKey);
        ByteArray publicKey;
        ByteArray::parseHexString(
                "FABB215EA4483A3686040F07A9EFCC1E86C6851AE68DCE9F39AF704EC72B310FE810C83E68140557A4DA10390DDEDF3CF6C00EFF6DB6B6996171662AE010330B",
                publicKey);
        Sm2Provider sm2(publicKey, privateKey);

//        String input, output;
//        Sm2Provider sm21(publicKey, privateKey);
//        sm21.encrypt("abc", output);
//        sm21.decrypt(output, input);

        String cipherStr = "306D022100F1B02FDEEB120DD1D4B633000FC397417BFB18B3E00BAC40377E95D2B21A68EF022100BFA34AA9B61E6D2A98DF264EC1E0F7E40DD446815C6EB7BAB1F340E59DE0CD7F04208F81ECF21B6F99E24A9249E0E2556E98A54ADE72D5BCB9CAF6EDE3E5F63325DF0403E67AE9";
        ByteArray in, out;
        ByteArray::parseHexString(cipherStr, in);
        if (!sm2.decrypt(in, out)) {
            return false;
        }
        String outStr((const char *) out.data(), out.count());
        if (outStr != "abc") {
            return false;
        }
    }

    return true;
}

bool testLogin() {
    {
        HttpClient client;
        JsonNode encryptNode;
        if (!client.get(Url("http://127.0.0.1:5438/v1/auth/users/encrypt"), HttpHeaders::JsonTypeHeaders,
                        encryptNode)) {
            return false;
        }
//    printf("encrypt: '%s'\n", encryptNode.toString().c_str());
        String code = encryptNode.getAttribute("code");
        if (code != "0") {
            return false;
        }
        ByteArray key, publicKey;
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("code"), key)) {
            return false;
        }
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("publicKey"), publicKey)) {
            return false;
        }

        HttpHeaders headers({HttpHeader("Content-Type", "application/x-www-form-urlencoded")});
        String request, response;
        String name = "user", password = "123.com";
        JsonNode node{
                {"name",     name},
                {"password", password},
        };
        String sm4Encode;
        Sm4Provider sm4(key.toHexString());
        sm4.setMode(CypherMode::ECB);
        sm4.encrypt(node.toString(), sm4Encode);
        sm4Encode = sm4Encode.toLower();
//    printf("SM4 key: %s\n", key.toHexString().c_str());
//    printf("SM4 encode str: %s\n", sm4Encode.c_str());

        String sm3Hash;
        Sm3Provider sm3;
        sm3.computeHash(sm4Encode, sm3Hash);
        sm3Hash = sm3Hash.toLower();
        JsonNode node2{
                {"data", sm4Encode},
                {"sign", sm3Hash},
                {"code", key.toHexString()},
        };
        String sm2Encode;
        Sm2Provider sm2(publicKey);
        sm2.encrypt(node2.toString(), sm2Encode);

        request = String::format("publicKey=%s&cryptograph=%s", publicKey.toHexString().c_str(), sm2Encode.c_str());
        if (!client.post(Url("http://127.0.0.1:5438/v1/auth/users/login"), headers, request, response)) {
            return false;
        }
        if (response.find("Login successfully") <= 0) {
            return false;
        }

        JsonNode responseNode;
        if (!JsonNode::parse(response, responseNode)) {
            return false;
        }
        _accessToken1 = responseNode.getAttribute("accessToken");
        if (_accessToken1.isNullOrEmpty()) {
            return false;
        }
    }

    {
        HttpClient client;
        JsonNode encryptNode;
        if (!client.get(Url("http://127.0.0.1:5438/v1/auth/users/encrypt"), HttpHeaders::JsonTypeHeaders,
                        encryptNode)) {
            return false;
        }
//    printf("encrypt: '%s'\n", encryptNode.toString().c_str());
        String code = encryptNode.getAttribute("code");
        if (code != "0") {
            return false;
        }
        ByteArray key, publicKey;
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("code"), key)) {
            return false;
        }
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("publicKey"), publicKey)) {
            return false;
        }

        HttpHeaders headers({HttpHeader("Content-Type", "application/x-www-form-urlencoded")});
        String request, response;
        String name = "user", password = "123.com";
        JsonNode node{
                {"name",     name},
                {"password", password},
        };
        String sm4Encode;
        Sm4Provider sm4(key.toHexString());
        sm4.setMode(CypherMode::ECB);
        sm4.encrypt(node.toString(), sm4Encode);
        sm4Encode = sm4Encode.toLower();
//    printf("SM4 key: %s\n", key.toHexString().c_str());
//    printf("SM4 encode str: %s\n", sm4Encode.c_str());

        String sm3Hash;
        Sm3Provider sm3;
        sm3.computeHash(sm4Encode, sm3Hash);
        sm3Hash = sm3Hash.toLower();
        JsonNode node2{
                {"data", sm4Encode},
                {"sign", sm3Hash},
                {"code", key.toHexString()},
        };
        String sm2Encode;
        Sm2Provider sm2(publicKey);
        sm2.encrypt(node2.toString(), sm2Encode);

        JsonNode node3{
                {"publicKey",   publicKey.toHexString()},
                {"cryptograph", sm2Encode},
        };
        request = node3.toString();
        if (!client.post(Url("http://127.0.0.1:5438/v1/auth/users/login"), headers, request, response)) {
            return false;
        }
        if (response.find("Login successfully") <= 0) {
            return false;
        }

        JsonNode responseNode;
        if (!JsonNode::parse(response, responseNode)) {
            return false;
        }
        _accessToken2 = responseNode.getAttribute("accessToken");
        if (_accessToken2.isNullOrEmpty()) {
            return false;
        }
    }

    {
        HttpClient client;
        JsonNode encryptNode;
        if (!client.get(Url("http://127.0.0.1:5438/v1/auth/users/encrypt"), HttpHeaders::JsonTypeHeaders,
                        encryptNode)) {
            return false;
        }
//    printf("encrypt: '%s'\n", encryptNode.toString().c_str());
        String code = encryptNode.getAttribute("code");
        if (code != "0") {
            return false;
        }
        ByteArray key, publicKey;
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("code"), key)) {
            return false;
        }
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("publicKey"), publicKey)) {
            return false;
        }

        HttpHeaders headers({HttpHeader("Content-Type", "application/x-www-form-urlencoded")});
        String request, response;
        String name = "user", password = "123.com";
        JsonNode node{
                {"name",     name},
                {"password", password},
        };
        String sm4Encode;
        Sm4Provider sm4(key.toHexString());
        sm4.setMode(CypherMode::ECB);
        sm4.encrypt(node.toString(), sm4Encode);
        sm4Encode = sm4Encode.toLower();
//    printf("SM4 key: %s\n", key.toHexString().c_str());
//    printf("SM4 encode str: %s\n", sm4Encode.c_str());

        String sm3Hash;
        Sm3Provider sm3;
        sm3.computeHash(sm4Encode, sm3Hash);
        sm3Hash = sm3Hash.toLower();
        JsonNode node2{
                {"data", sm4Encode},
                {"sign", sm3Hash},
                {"code", key.toHexString()},
        };
        String sm2Encode;
        Sm2Provider sm2(publicKey);
        sm2.encrypt(node2.toString(), sm2Encode);

        Url baseUrl("http://127.0.0.1:5438/v1/auth/users/login");
        Url url(baseUrl, String::format("?publicKey=%s&cryptograph=%s",
                                        publicKey.toHexString().c_str(), sm2Encode.c_str()));
        if (!client.post(url, headers, request, response)) {
            return false;
        }
        if (response.find("Login successfully") <= 0) {
            return false;
        }

        JsonNode responseNode;
        if (!JsonNode::parse(response, responseNode)) {
            return false;
        }
        _accessToken3 = responseNode.getAttribute("accessToken");
        if (_accessToken3.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testLogout() {
    {
        HttpClient client;
        String request, response;
        JsonNode node{
                {"name", "user"},
        };
        request = node.toString();
        Url baseUrl("http://127.0.0.1:5438/v1/auth/users/logout");
        Url url(baseUrl, String::format("?accessToken=%s&", _accessToken1.c_str()));
        if (!client.post(url, HttpHeaders::JsonTypeHeaders, request, response)) {
            return false;
        }
        if (response.find("Logout successfully") <= 0) {
            return false;
        }
    }

    {
        HttpClient client;
        String request, response;
        Url baseUrl("http://127.0.0.1:5438/v1/auth/users/logout");
        Url url(baseUrl, String::format("?accessToken=%s&name=%s", _accessToken2.c_str(), "user"));
        if (!client.post(url, HttpHeaders::JsonTypeHeaders, request, response)) {
            return false;
        }
        if (response.find("Logout successfully") <= 0) {
            return false;
        }
    }

    {
        HttpClient client;
        String request, response;
        Url baseUrl("http://127.0.0.1:5438/v1/auth/users/logout");
        Url url(baseUrl, String::format("?name=%s", "user"));
        HttpHeaders headers({HttpHeader("Content-Type", "application/json"),
                             HttpHeader("accessToken", _accessToken3)});
        if (!client.post(url, headers, request, response)) {
            return false;
        }
        if (response.find("Logout successfully") <= 0) {
            return false;
        }
    }

    return true;
}

bool testModifyPassword() {
    {
        HttpClient client;
        JsonNode encryptNode;
        if (!client.get(Url("http://127.0.0.1:5438/v1/auth/users/encrypt"), HttpHeaders::JsonTypeHeaders,
                        encryptNode)) {
            return false;
        }
//    printf("encrypt: '%s'\n", encryptNode.toString().c_str());
        String code = encryptNode.getAttribute("code");
        if (code != "0") {
            return false;
        }
        ByteArray key, publicKey;
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("code"), key)) {
            return false;
        }
        if (!ByteArray::parseHexString(encryptNode["data"].getAttribute("publicKey"), publicKey)) {
            return false;
        }

        HttpHeaders headers({HttpHeader("Content-Type", "application/x-www-form-urlencoded")});
        String request, response;
        String name = "user", oldPassword = "123.com", newPassword = "1234.com";
        JsonNode node{
                {"name",     name},
                {"oldPassword", oldPassword},
                {"newPassword", newPassword},
        };
        String sm4Encode;
        Sm4Provider sm4(key.toHexString());
        sm4.setMode(CypherMode::ECB);
        sm4.encrypt(node.toString(), sm4Encode);
        sm4Encode = sm4Encode.toLower();
//    printf("SM4 key: %s\n", key.toHexString().c_str());
//    printf("SM4 encode str: %s\n", sm4Encode.c_str());

        String sm3Hash;
        Sm3Provider sm3;
        sm3.computeHash(sm4Encode, sm3Hash);
        sm3Hash = sm3Hash.toLower();
        JsonNode node2{
                {"data", sm4Encode},
                {"sign", sm3Hash},
                {"code", key.toHexString()},
        };
        String sm2Encode;
        Sm2Provider sm2(publicKey);
        sm2.encrypt(node2.toString(), sm2Encode);

        request = String::format("publicKey=%s&cryptograph=%s", publicKey.toHexString().c_str(), sm2Encode.c_str());
        if (!client.post(Url("http://127.0.0.1:5438/v1/auth/users/password"), headers, request, response)) {
            return false;
        }
        if (response.find("Modify successfully") <= 0) {
            return false;
        }

        // clean up.
        String fileName = Path::combine(Path::getAppPath(), "application.yml");
        if (File::exists(fileName)) {
            File::deleteFile(fileName);
        }
    }

    return true;
}

int main() {
    ConfigService cs;
    cs.setProperty("server.enabled", true);
    cs.setProperty("server.http.session.enabled", true);
    cs.setProperty("server.scheme", "http");
    cs.setProperty("server.port", 5438);
    cs.setProperty("server.enabled", true);
    cs.setProperty("summer.security.users[0].name", "user");
    cs.setProperty("summer.security.users[0].password", "123.com");

    HttpService hs;
    hs.initialize();

    SsoService ss;
    ss.initialize();

    auto func = [](HttpService *hs) {
        return hs->isAlive();
    };
    Thread::delay(1500, Func<bool>(func, &hs));
    Thread::msleep(500);

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testLogin()) {
        result = 2;
    }
    if (!testModifyPassword()) {
        result = 3;
    }
    if (!testLogout()) {
        result = 4;
    }

    ss.unInitialize();
    hs.unInitialize();

    return result;
}