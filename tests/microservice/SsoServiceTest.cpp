//
//  SsoServiceTest.cpp
//  common
//
//  Created by baowei on 2023/7/19.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "microservice/SsoService.h"
#include "microservice/DataSourceService.h"
#include "crypto/SmProvider.h"
#include "configuration/ConfigService.h"
#include "http/HttpClient.h"
#include "system/Application.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Microservice;
using namespace Crypto;
using namespace Config;
using namespace Http;

String _accessToken1;
String _accessToken2;
String _accessToken3;

static String _host = "192.166.1.3";
static String _port = "3306";
static String _baseUrl(String::format("mysql://%s:%s", _host.c_str(), _port.c_str()));
static String _database = "SsoServiceTest_db";
static String _url = _baseUrl + "/" + _database;
static String _username = "root";
static String _password = "123.com";

void setUp() {
    {
        SqlConnection connection(Url(_baseUrl), _username, _password);
        if (!connection.open()) {
            return;
        }
        connection.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
        if (!connection.executeSql(String::format("CREATE DATABASE %s;", _database.c_str()))) {
            return;
        }
    }

    {
        // Create catalog_user table and fill admin & user.
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return;
        }
        String sql = "SET NAMES utf8mb4; SET FOREIGN_KEY_CHECKS = 0; DROP TABLE IF EXISTS `catalog_user`;"
                     " CREATE TABLE `catalog_user`( `id` bigint(20) NOT NULL, `name` varchar(128)"
                     " CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL, `password` varchar(128)"
                     " CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL, `update_user` varchar(128)"
                     " DEFAULT NULL, `update_time` datetime DEFAULT NULL, PRIMARY KEY (`id`))"
                     " ENGINE=InnoDB DEFAULT CHARSET=utf8; BEGIN; INSERT INTO `catalog_user`"
                     " VALUES (1, 'admin', '4bXMiNc+1YZ81tzpD2ZBkWjADzejfvbLC4vpZLQo0iQ81LL9APomCJJi3Vx8iRO1',"
                     " NULL, NULL); INSERT INTO `catalog_user` VALUES (2, 'user',"
                     " 'jw5chyjSvQQdjAxA3CMPhYTcKbGoRLrfW2CKsIiGAAL6tryNM0/1loiyubDfwrWZ', NULL, NULL); COMMIT;"
                     " SET FOREIGN_KEY_CHECKS = 1;";
        connection.executeSql(sql);
    }

    {
        String text = "# -*- coding:utf-8 -*-\n"
                      "\n"
                      "import sys\n"
                      "\n"
                      "\n"
                      "def _check(user_name, password):\n"
                      "    if user_name == 'user':\n"
                      "        if password == '123.com' or password == '1234.com' or password is None:\n"
                      "            return True\n"
                      "    return False\n"
                      "\n"
                      "\n"
                      "def _modify(user_name, old_password, new_password):\n"
                      "    if user_name == 'user':\n"
                      "        if old_password == '123.com' and new_password == '1234.com':\n"
                      "            return True\n"
                      "        if old_password == '1234.com' and new_password == '123.com':\n"
                      "            return True\n"
                      "    return False\n"
                      "\n"
                      "\n"
                      "if __name__ == '__main__':\n"
                      "    result = False\n"
                      "\n"
                      "    if len(sys.argv) > 2:\n"
                      "        task_name = sys.argv[1]\n"
                      "        if task_name == 'check':\n"
                      "            user_name = sys.argv[2]\n"
                      "            if len(sys.argv) > 3:\n"
                      "                password = sys.argv[3]\n"
                      "            else:\n"
                      "                password = None\n"
                      "            result = _check(user_name, password)\n"
                      "        elif task_name == 'modify':\n"
                      "            user_name = sys.argv[2]\n"
                      "            if len(sys.argv) > 4:\n"
                      "                old_password = sys.argv[3]\n"
                      "                new_password = sys.argv[4]\n"
                      "                result = _modify(user_name, old_password, new_password)\n"
                      "\n"
                      "    print(result)\n";
        String fileName = Path::combine(Path::getAppPath(), "sso.py");
        FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(text);
        fs.close();
        File::chmod(fileName);
    }
}

void cleanUp() {
    SqlConnection connection(Url(_baseUrl), _username, _password);
    if (connection.open()) {
        connection.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
    }

    {
        String fileName = Path::combine(Path::getAppPath(), "sso.py");
        if (File::exists(fileName)) {
            File::deleteFile(fileName);
        }
    }
}

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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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

bool testModifyPassword(const String &oldPassword, const String &newPassword) {
    {
        HttpClient client{
                {"connectionTimeout", "00:00:05"},
                {"receiveTimeout",    "00:00:05"},
        };
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
        String name = "user";
        JsonNode node{
                {"name",        name},
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

int testInit() {
    HttpService hs;
    hs.initialize();

    SsoService ss;
    ss.initialize();

    auto func = [](HttpService *hs) {
        return hs->isAlive();
    };
    Thread::delay(3000, Func<bool>(func, &hs));
    Thread::msleep(500);
//#if defined(WIN32) && defined(_X86_)
//    Thread::msleep(3000);
//#endif

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testLogin()) {
        result = 2;
    }
    if (!testModifyPassword("123.com", "1234.com")) {
        result = 3;
    }
    if (!testModifyPassword("1234.com", "123.com")) {
        result = 4;
    }
    if (!testLogout()) {
        result = 5;
    }

    ss.unInitialize();
    hs.unInitialize();
    return result;
}

int testYml() {
    ConfigService cs;
    cs.setProperty("server.enabled", true);
    cs.setProperty("server.http.session.enabled", true);
    cs.setProperty("server.scheme", "http");
    cs.setProperty("server.port", 5438);
    cs.setProperty("server.enabled", true);

    cs.setProperty("summer.security.users[0].name", "user");
    cs.setProperty("summer.security.users[0].password", "123.com");

    return testInit();
}

int testDatabase() {
    ConfigService cs;
    cs.setProperty("server.enabled", true);
    cs.setProperty("server.http.session.enabled", true);
    cs.setProperty("server.scheme", "http");
    cs.setProperty("server.port", 5438);
    cs.setProperty("server.enabled", true);

    cs.setProperty("summer.security.type", "database");
    cs.setProperty("summer.datasource.enabled", true);
    cs.setProperty("summer.datasource.username", _username);
    cs.setProperty("summer.datasource.password", _password);
    cs.setProperty("summer.datasource.url", _url);
    cs.setProperty("summer.datasource.scheme", "");

    DataSourceService ds;
    ds.initialize();

    return testInit();
}

int testPython() {
    ConfigService cs;
    cs.setProperty("server.enabled", true);
    cs.setProperty("server.http.session.enabled", true);
    cs.setProperty("server.scheme", "http");
    cs.setProperty("server.port", 5438);
    cs.setProperty("server.enabled", true);

    cs.setProperty("summer.security.type", "python");
    cs.setProperty("summer.security.python.check.file", "sso.py");
    cs.setProperty("summer.security.python.check.param", "check");
    cs.setProperty("summer.security.python.modify.file", "sso.py");
    cs.setProperty("summer.security.python.modify.param", "modify");

    return testInit();
}

bool parseArguments(const Application &app) {
    const Application::Arguments &arguments = app.arguments();
    String host, userName, password, database;
    Port port;
    if (arguments.contains("help") || arguments.contains("?")) {
        puts("Usage:");
        puts("-?, --help            Display this help and exit.");
        puts("-h, --host=name       Connect to host.");
        puts("-P, --port=#          Port number to use for connection.");
        puts("-u, --user=name       User for login if not current user.");
        puts("-p, --password[=name] Password to use when connecting to server.");
        puts("-d, --database=name   Database for connection.");
        return false;
    }

    if (arguments.contains("host") || arguments.contains("h")) {
        host = arguments["host"];
        if (host.isNullOrEmpty()) {
            host = arguments["h"];
        }
    }
    if (arguments.contains("port") || arguments.contains("P")) {
        if (!Port::parse(arguments["port"], port)) {
            Port::parse(arguments["P"], port);
        }
    }
    if (arguments.contains("user") || arguments.contains("u")) {
        userName = arguments["user"];
        if (userName.isNullOrEmpty()) {
            userName = arguments["u"];
        }
    }
    if (arguments.contains("password") || arguments.contains("p")) {
        password = arguments["password"];
        if (password.isNullOrEmpty()) {
            password = arguments["p"];
        }
    }
    if (arguments.contains("database") || arguments.contains("d")) {
        database = arguments["database"];
        if (database.isNullOrEmpty()) {
            database = arguments["d"];
        }
    }

    _baseUrl = String::format("mysql://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : _host.c_str(),
                              !port.isEmpty() ? port.toString().c_str() : _port.c_str());
    _database = !database.isNullOrEmpty() ? database : _database;
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : _username;
    _password = !password.isNullOrEmpty() ? password : _password;

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -d=connection_db
int main(int argc, const char *argv[]) {
//    String cypher1 = ConfigService::computeCypherText(R"({"name":"user","password":"123.com"})");
//    String cypher2 = ConfigService::computeCypherText(R"({"name":"admin","password":"123.com"})");

    Application app(argc, argv);
    if (!parseArguments(app)) {
        return 0;
    }

    setUp();

    int result;

    result = testYml();
    if (result != 0) {
        cleanUp();
        return result;
    }

    result = testDatabase();
    if (result != 0) {
        cleanUp();
        return result + 10;
    }

    result = testPython();
    if (result != 0) {
        cleanUp();
        return result + 20;
    }

    cleanUp();
    return result;
}