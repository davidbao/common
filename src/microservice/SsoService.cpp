//
//  SsoService.cpp
//  common
//
//  Created by baowei on 2022/5/31.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "microservice/SsoService.h"
#include "crypto/SmProvider.h"
#include "IO/File.h"
#include "IO/Path.h"
#include "diag/Trace.h"
#include "diag/Process.h"
#include "configuration/ConfigService.h"
#include "microservice/DataSourceService.h"

using namespace Crypto;
using namespace Config;
using namespace Diag;

namespace Microservice {
    const char *SsoService::AccessTokenId = "accessToken";
    const char *SsoService::Sm2String = "SM2_";

    SsoService::SsoService() : _connection(nullptr) {
    }

    SsoService::~SsoService() {
        delete _connection;
        _connection = nullptr;
    }

    bool SsoService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        auto hs = factory->getService<IHttpRegister>();
        assert(hs);
#define BasePath "v1/auth/users"
        hs->registerMapping(HttpMethod::Get, BasePath "/encrypt",
                            HttpCallback<SsoService>(this, &SsoService::onEncrypt));
        hs->registerMapping(HttpMethod::Post, BasePath "/login",
                            HttpCallback<SsoService>(this, &SsoService::onLogin));
        hs->registerMapping(HttpMethod::Post, BasePath "/logout",
                            HttpCallback<SsoService>(this, &SsoService::onLogout));
        hs->registerMapping(HttpMethod::Post, BasePath "/password",
                            HttpCallback<SsoService>(this, &SsoService::onModifyPassword));

        // session.
        bool sessionEnabled = false;
        if (cs->getProperty("server.http.session.enabled", sessionEnabled) && sessionEnabled) {
            auto hss = factory->getService<IHttpSession>();
            assert(hss);
            hss->registerTokenId(AccessTokenId);
        }

        // white list.
        bool whitelistEnabled = false;
        if (cs->getProperty("server.http.whitelist.enabled", whitelistEnabled) && whitelistEnabled) {
            auto hi = factory->getService<IHttpInterceptor>();
            assert(hi);
            StringArray list;
            cs->getProperty("server.http.whitelist.paths", list);
            hi->addWhitelist(list);
        }

        String securityType;
        cs->getProperty(SecurityPrefix "type", securityType);
        if (securityType == "database") {
            String connectionStr = DataSourceService::createConnectionStr(SecurityPrefix "database.");
            if (!connectionStr.isNullOrEmpty()) {
                _connection = new SqlConnection(connectionStr);
                _connection->open();
            }
        }

        return true;
    }

    bool SsoService::unInitialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);

        auto hs = factory->getService<IHttpRegister>();
        assert(hs);
        hs->removeMapping(this);

        delete _connection;
        _connection = nullptr;

        return true;
    }

    HttpStatus SsoService::onEncrypt(const HttpRequest &request, HttpResponse &response) {
        JsonNode data("data"), result;
        ByteArray key, publicKey, privateKey;
        Sm4Provider sm4;
        sm4.setMode(CypherMode::ECB);
        sm4.generateKey(key);
        Sm2Provider::generateKey(publicKey, privateKey);

//        // for test
//#ifdef DEBUG
//        Trace::writeFormatLine("SM2 private key: %s", privateKey.toHexString().c_str());
//        Trace::writeFormatLine("SM2 public key: %s", publicKey.toHexString().c_str());
//        String name = "user", password = "123.com";
//        const JsonNode::KeyValue kvs[] = {
//            {"name", name},
//            {"password", password},
//            nullptr
//        };
//        JsonNode node(kvs);
//        String sm4Encode;
//        Sm4Provider sm4(key);
//        sm4.setMode(CypherMode::ECB);
//        sm4.encrypt(node.toString(), sm4Encode);
//        sm4Encode = sm4Encode.toLower();
//        Trace::writeFormatLine("SM4 key: %s", key.toHexString().c_str());
//        Trace::writeFormatLine("SM4 encode str: %s", sm4Encode.c_str());
//        String sm3Hash;
//        Sm3Provider::computeHash(sm4Encode, sm3Hash);
//        sm3Hash = sm3Hash.toLower();
//        Trace::writeFormatLine("SM3 hash str: %s", sm3Hash.c_str());
//        const JsonNode::KeyValue kvs2[] = {
//            {"data", sm4Encode},
//            {"sign", sm3Hash},
//            {"code", key.toHexString()},
//            nullptr
//        };
//        JsonNode node2(kvs2);
//        String sm2Encode;
//        Sm2Provider sm2(publicKey, privateKey);
//        sm2.encrypt(node2.toString(), sm2Encode);
//        Trace::writeFormatLine("SM2 encode str: %s", sm2Encode.c_str());
//#endif
//        // for test

        addCache(String::format("%s%s", Sm2String, publicKey.toHexString().c_str()), privateKey.toHexString().c_str());

        data.add(JsonNode("code", key.toHexString()));
        data.add(JsonNode("publicKey", publicKey.toHexString()));
        result.add(JsonNode("code", "0"));
        result.add(data);
        response.setContent(result);
        return HttpStatus::HttpOk;
    }

    HttpStatus SsoService::onLogin(const HttpRequest &request, HttpResponse &response) {
        if (request.findHeader("Content-Type", "application/x-www-form-urlencoded")) {
            JsonNode node, result;
            ErrorCode code = decode(request, node);
            if (code == ErrorCode::Success) {
                String name, password;
                node.getAttribute("name", name);
                if (!name.isNullOrEmpty() && node.getAttribute("password", password)) {
                    if (check(name, password)) {
                        // Login successfully.
                        result.add(JsonNode("code", "0"));
                        result.add(JsonNode("msg", "Login successfully."));

                        ServiceFactory *factory = ServiceFactory::instance();
                        assert(factory);
                        auto hs = factory->getService<IHttpSession>();
                        assert(hs);
                        String token = hs->addSession(name);
                        result.add(JsonNode(AccessTokenId, token));
                    } else {
                        // The username or password is incorrect.
                        result.add(JsonNode("code", "518"));
                        result.add(JsonNode("msg", "The user name or password is incorrect."));
                    }
                } else {
                    // The username or password is incorrect.
                    result.add(JsonNode("code", "518"));
                    result.add(JsonNode("msg", "The user name or password is incorrect."));
                }
            } else {
                errorNode(code, result);
            }
            response.setContent(result);
        }
        return HttpStatus::HttpOk;
    }

    HttpStatus SsoService::onLogout(const HttpRequest &request, HttpResponse &response) {
        String name;
        JsonNode node, result;
        if (JsonNode::parse(request.text(), node)) {
            if (!node.getAttribute("name", name))
                node.getAttribute("userName", name);
        } else if (request.getPropValue("name", name)) {
        } else {
            result.add(JsonNode("code", "1100"));
            result.add(JsonNode("msg", String::format("The field'%s' is incorrect.", "name")));
            response.setContent(result);
            return HttpStatus::HttpOk;
        }

        if (!name.isNullOrEmpty()) {
            if (check(name)) {
                // Logout successfully.
                ServiceFactory *factory = ServiceFactory::instance();
                assert(factory);
                auto hs = factory->getService<IHttpSession>();
                assert(hs);
                hs->removeSession(request);

                result.add(JsonNode("code", "0"));
                result.add(JsonNode("msg", "Logout successfully."));
            } else {
                // The username or password is incorrect.
                result.add(JsonNode("code", "518"));
                result.add(JsonNode("msg", "The user name or password is incorrect."));
            }
        } else {
            // The username or password is incorrect.
            result.add(JsonNode("code", "518"));
            result.add(JsonNode("msg", "The user name or password is incorrect."));
        }

        response.setContent(result);
        return HttpStatus::HttpOk;
    }

    HttpStatus SsoService::onModifyPassword(const HttpRequest &request, HttpResponse &response) {
        if (request.findHeader("Content-Type", "application/x-www-form-urlencoded")) {
            JsonNode node, result;
            ErrorCode code = decode(request, node);
            if (code == ErrorCode::Success) {
                String name;
                if (!node.getAttribute("name", name)) {
                    result.add(JsonNode("code", "1100"));
                    result.add(JsonNode("msg", String::format("The field'%s' is incorrect.", "name")));
                    response.setContent(result);
                    return HttpStatus::HttpOk;
                }
                String oldPassword;
                if (!node.getAttribute("oldPassword", oldPassword)) {
                    result.add(JsonNode("code", "1100"));
                    result.add(JsonNode("msg", String::format("The field'%s' is incorrect.", "oldPassword")));
                    response.setContent(result);
                    return HttpStatus::HttpOk;
                }
                String newPassword;
                if (!node.getAttribute("newPassword", newPassword)) {
                    result.add(JsonNode("code", "1100"));
                    result.add(JsonNode("msg", String::format("The field'%s' is incorrect.", "newPassword")));
                    response.setContent(result);
                    return HttpStatus::HttpOk;
                }

                if (check(name, oldPassword)) {
                    if (modifyPassword(name, oldPassword, newPassword)) {
                        result.add(JsonNode("code", "0"));
                        result.add(JsonNode("msg", "Modify successfully."));
                    } else {
                        result.add(JsonNode("code", "1101"));
                        result.add(JsonNode("msg", "Failed to save config file."));
                    }
                } else {
                    // The username or password is incorrect.
                    result.add(JsonNode("code", "518"));
                    result.add(JsonNode("msg", "The user name or password is incorrect."));
                }
            } else {
                errorNode(code, result);
            }
            response.setContent(result);
        }
        return HttpStatus::HttpOk;
    }

    SsoService::ErrorCode SsoService::decode(const HttpRequest &request, String &content) {
        JsonNode node;
        ByteArray publicKey;
        String publicKeyStr, cryptograph;
        if (JsonNode::parse(request.text(), node)) {
            node.getAttribute("publicKey", publicKeyStr);
            ByteArray::parseHexString(publicKeyStr, publicKey);
            if (!node.getAttribute("cryptograph", cryptograph))
                node.getAttribute("encrypt", cryptograph);
        } else {
            HttpProperties properties;
            HttpProperties::parse(request.text(), properties);
            if (properties.count() == 0) {
                properties = request.properties;
            }
            properties.at("publicKey", publicKeyStr);
            ByteArray::parseHexString(publicKeyStr, publicKey);
            if (!properties.at("cryptograph", cryptograph))
                properties.at("encrypt", cryptograph);
        }
        if (!(!publicKey.isEmpty() && !cryptograph.isNullOrEmpty())) {
            // The request parameters is incorrect.
            return ErrorCode::ParameterIncorrect;
        }

        ByteArray privateKey;
        String privateKeyStr;
        if (getCache(String::format("%s%s", Sm2String, publicKeyStr.c_str()), privateKeyStr)) {
            ByteArray::parseHexString(privateKeyStr, privateKey);
        }
        if (privateKey.isEmpty()) {
            // Can not find the private key.
            return ErrorCode::NotFoundPrivateKey;
        }

        // sm2 decoded.
        String sm2Decode;
        Sm2Provider sm2(publicKey, privateKey);
        if (!sm2.decrypt(cryptograph, sm2Decode))
            return ErrorCode::Sm2DecodeError;
//        Trace::writeFormatLine("SM2 decode str: %s", sm2Decode.c_str());
        JsonNode dataNode;
        if (JsonNode::parse(sm2Decode, dataNode)) {
            String data, sign, code;
            dataNode.getAttribute("data", data);
            dataNode.getAttribute("sign", sign);
            dataNode.getAttribute("code", code);

            // check hash.
            String sm3Hash;
            Sm3Provider sm3;
            sm3.computeHash(data, sm3Hash);
            sm3Hash = sm3Hash.toLower();
//            Trace::writeFormatLine("SM3 hash str: %s", sm3Hash.c_str());
            if (sign != sm3Hash) {
                // The request parameters hash error.
                return ErrorCode::HashError;
            }

            Sm4Provider sm4(code);
            sm4.setMode(CypherMode::ECB);
            if (sm4.decrypt(data, content)) {
                JsonNode contentNode;
                if (JsonNode::parse(content, contentNode)) {
//                    Trace::writeFormatLine("content: %s", content.c_str());
                } else {
                    return ErrorCode::JsonError;
                }
            } else {
                // Can not decode by sm4.
                return ErrorCode::Sm4DecodeError;
            }
        } else {
            return ErrorCode::JsonError;
        }
        return ErrorCode::Success;
    }

    SsoService::ErrorCode SsoService::decode(const HttpRequest &request, JsonNode &content) {
        String str;
        ErrorCode code = decode(request, str);
        if (code == ErrorCode::Success) {
            return JsonNode::parse(str, content) ? ErrorCode::Success : ErrorCode::JsonError;
        }
        return code;
    }

    bool SsoService::addCache(const String &name, const String &value) {
        _caches.add(name, value);
        return true;
    }

    bool SsoService::getCache(const String &name, String &value) {
        return _caches.at(name, value);
    }

    SqlConnection *SsoService::connection() {
        if (_connection != nullptr) {
            return _connection;
        } else {
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            auto ds = factory->getService<IDataSourceService>();
            if (ds != nullptr) {
                return ds->connection();
            }
            return nullptr;
        }
    }

    bool SsoService::check(const String &name, const String &password) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String securityType;
        cs->getProperty(SecurityPrefix "type", securityType);
        if (securityType == "database") {
            return checkByDb(name, password);
        } else if (securityType == "python") {
            return checkByPython(name, password);
        } else {
            return checkByYml(name, password);
        }
    }

    bool SsoService::checkByYml(const String &name, const String &password) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String oname, opassword;
        static const uint32_t maxUserCount = 8;
        bool correct = false;
        for (size_t i = 0; i < maxUserCount; i++) {
            cs->getProperty(String::format(SecurityPrefix "users[%d].name", i), oname);
            cs->getProperty(String::format(SecurityPrefix "users[%d].password", i), opassword);
            correct = name == oname && password == opassword;
            if (correct)
                break;
        }
        if (!correct) {
            cs->getProperty(SecurityPrefix "user.name", oname);
            cs->getProperty(SecurityPrefix "user.password", opassword);
            correct = name == oname && password == opassword;
        }
        return correct;
    }

    bool SsoService::checkByDb(const String &name, const String &password) {
        SqlConnection *connection = this->connection();
        if (connection != nullptr) {
            // {"name":"user","password":"123.com"}
            DataTable table("user");
            String tableName = connection->schema().isNullOrEmpty() ?
                               "CATALOG_USER" : String::format("%s.CATALOG_USER", connection->schema().c_str());
            String sql = String::format("SELECT PASSWORD FROM %s WHERE NAME='%s'", tableName.c_str(), name.c_str());
            if (connection->executeSqlQuery(sql, table) && table.rowCount() == 1) {
                String cypherText = table.rows()[0].cells()[0].valueStr();
                String sm4PlainText = ConfigService::computePlainText(cypherText);
                JsonNode node;
                if (JsonNode::parse(sm4PlainText, node)) {
                    return node.getAttribute("password") == password;
                }
            }
        }
        return false;
    }

    bool SsoService::checkByPython(const String &name, const String &password) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String file, param;
        cs->getProperty(SecurityPrefix "python.check.file", file);
        cs->getProperty(SecurityPrefix "python.check.param", param);
        String fileName;
        if (Path::isPathRooted(file)) {
            if (File::exists(file)) {
                fileName = file;
            }
        } else {
            fileName = Path::combine(Path::getAppPath(), file);
        }
        if (!File::exists(fileName)) {
            Trace::debug("Can not file python file in sso service!");
            return false;
        }

        param = String::format("%s %s %s %s", fileName.c_str(), param.c_str(), name.c_str(), password.c_str());
        Process process;
        process.setRedirectStdout(true);
        if (Process::start("python", param, &process)) {
            const String &str = process.stdoutStr();
            return str.find("Check successfully.") >= 0;
        }
        return false;
    }

    bool SsoService::check(const String &name) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String securityType;
        cs->getProperty(SecurityPrefix "type", securityType);
        if (securityType == "database") {
            return checkByDb(name);
        } else if (securityType == "python") {
            return checkByPython(name);
        } else {
            return checkByYml(name);
        }
    }

    bool SsoService::checkByYml(const String &name) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String oname, opassword;
        static const uint32_t maxUserCount = 8;
        bool correct = false;
        for (size_t i = 0; i < maxUserCount; i++) {
            cs->getProperty(String::format(SecurityPrefix "users[%d].name", i), oname);
            correct = name == oname;
            if (correct)
                break;
        }
        if (!correct) {
            cs->getProperty(SecurityPrefix "user.name", oname);
            correct = name == oname;
        }
        return correct;
    }

    bool SsoService::checkByDb(const String &name) {
        SqlConnection *connection = this->connection();
        if (connection != nullptr) {
            // {"name":"user","password":"123.com"}
            DataTable table("user");
            String tableName = connection->schema().isNullOrEmpty() ?
                               "CATALOG_USER" : String::format("%s.CATALOG_USER", connection->schema().c_str());
            String sql = String::format("SELECT PASSWORD FROM %s WHERE NAME='%s'", tableName.c_str(), name.c_str());
            if (connection->executeSqlQuery(sql, table) && table.rowCount() == 1) {
                return true;
            }
        }
        return false;
    }

    bool SsoService::checkByPython(const String &name) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String file, param;
        cs->getProperty(SecurityPrefix "python.check.file", file);
        cs->getProperty(SecurityPrefix "python.check.param", param);
        String fileName;
        if (Path::isPathRooted(file)) {
            if (File::exists(file)) {
                fileName = file;
            }
        } else {
            fileName = Path::combine(Path::getAppPath(), file);
        }
        if (!File::exists(fileName)) {
            Trace::debug("Can not file python file in sso service!");
            return false;
        }

        param = String::format("%s %s %s", fileName.c_str(), param.c_str(), name.c_str());
        Process process;
        process.setRedirectStdout(true);
        if (Process::start("python", param, &process)) {
            const String &str = process.stdoutStr();
            return str.find("Check successfully.") >= 0;
        }
        return false;
    }

    bool SsoService::modifyPassword(const String &name, const String &oldPassword, const String &newPassword) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String securityType;
        cs->getProperty(SecurityPrefix "type", securityType);
        if (securityType == "database") {
            return modifyPasswordByDb(name, oldPassword, newPassword);
        } else if (securityType == "python") {
                return modifyPasswordByPython(name, oldPassword, newPassword);
        } else {
            return modifyPasswordByYml(name, oldPassword, newPassword);
        }
    }

    bool SsoService::modifyPasswordByYml(const String &name, const String &oldPassword, const String &newPassword) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String oname, opassword, key;
        static const uint32_t maxUserCount = 8;
        bool correct = false;
        for (size_t i = 0; i < maxUserCount; i++) {
            cs->getProperty(String::format(SecurityPrefix "users[%d].name", i), oname);
            cs->getProperty(String::format(SecurityPrefix "users[%d].password", i), opassword);
            correct = name == oname && oldPassword == opassword;
            if (correct) {
                key = String::format(SecurityPrefix "users[%d].password", i);
                break;
            }
        }
        if (!correct) {
            cs->getProperty(SecurityPrefix "user.name", oname);
            cs->getProperty(SecurityPrefix "user.password", opassword);
            correct = name == oname && oldPassword == opassword;
            if (correct) {
                key = SecurityPrefix "user.password";
            }
        }
        if (correct) {
            YmlNode::Properties properties;
            properties.add(key, ConfigService::createEncText(newPassword));
            return cs->updateConfigFile(properties);
        }
        return false;
    }

    bool SsoService::modifyPasswordByDb(const String &name, const String &oldPassword, const String &newPassword) {
        if (!checkByDb(name, oldPassword)) {
            return false;
        }

        SqlConnection *connection = this->connection();
        if (connection != nullptr) {
            // {"name":"user","password":"123.com"}
            String tableName = connection->schema().isNullOrEmpty() ?
                               "CATALOG_USER" : String::format("%s.CATALOG_USER", connection->schema().c_str());
            String cypher = ConfigService::computeCypherText(
                    String::format(R"({"name":"%s","password":"%s"})", name.c_str(), newPassword.c_str()));
            String sql = String::format("UPDATE %s SET PASSWORD='%s',UPDATE_USER='%s',UPDATE_TIME=NOW() WHERE NAME='%s'",
                                        tableName.c_str(), cypher.c_str(), name.c_str(), name.c_str());
            return connection->executeSql(sql);
        }
        return false;
    }

    bool SsoService::modifyPasswordByPython(const String &name, const String &oldPassword, const String &newPassword) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        String file, param;
        cs->getProperty(SecurityPrefix "python.modify.file", file);
        cs->getProperty(SecurityPrefix "python.modify.param", param);
        String fileName;
        if (Path::isPathRooted(file)) {
            if (File::exists(file)) {
                fileName = file;
            }
        } else {
            fileName = Path::combine(Path::getAppPath(), file);
        }
        if (!File::exists(fileName)) {
            Trace::debug("Can not file python file in sso service!");
            return false;
        }

        param = String::format("%s %s %s %s %s", fileName.c_str(), param.c_str(), name.c_str(),
                               oldPassword.c_str(), newPassword.c_str());
        Process process;
        process.setRedirectStdout(true);
        if (Process::start("python", param, &process)) {
            const String &str = process.stdoutStr();
            return str.find("Modify successfully.") >= 0;
        }
        return false;
    }

    void SsoService::errorNode(ErrorCode code, JsonNode &node) {
        switch (code) {
            case Success:
                break;
            case ParameterIncorrect:
                node.add(JsonNode("code", "513"));
                node.add(JsonNode("msg", "The request parameters is incorrect."));
                break;
            case NotFoundPrivateKey:
                node.add(JsonNode("code", "514"));
                node.add(JsonNode("msg", "The public key of the encrypt interface is not used for encryption."));
                break;
            case Sm2DecodeError:
                node.add(JsonNode("code", "515"));
                node.add(JsonNode("msg", "Failed to decode with SM2."));
                break;
            case JsonError:
                node.add(JsonNode("code", "511"));
                node.add(JsonNode("msg", "Json string parse error."));
                break;
            case HashError:
                node.add(JsonNode("code", "516"));
                node.add(JsonNode("msg", "Failed to verify hash with SM3."));
                break;
            case Sm4DecodeError:
                node.add(JsonNode("code", "517"));
                node.add(JsonNode("msg", "Failed to decode with SM4."));
                break;
            default:
                break;
        }
    }
}
