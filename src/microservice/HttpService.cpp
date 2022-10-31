//
//  HttpService.cpp
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/HttpService.h"
#include "configuration/ConfigService.h"
#include "IO/Directory.h"

namespace Microservice {
    IHttpAction::IHttpAction() {
    }

    IHttpAction::~IHttpAction() {
    }

    HttpStatus IHttpAction::onAction(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Post && hasPostAction(request)) {
            JsonNode node;
            SqlSelectFilter filter;
            if (SqlSelectFilter::parse(request.text(), filter)) {
                DataTable table;
                if (query(request, filter, table)) {
                    JsonNode dataNode;
                    table.toJsonNode(dataNode);
                    node.add(JsonNode("code", "200"));
                    node.add(JsonNode("data", dataNode.toString()));
                } else {
                    node.add(JsonNode("code", "601"));
                    node.add(JsonNode("msg", "Failed to query."));
                }
            } else {
                node.add(JsonNode("code", "600"));
                node.add(JsonNode("msg", "The filter is incorrect."));
            }
            response.setContent(node);
            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    bool IHttpAction::hasPostAction(const HttpRequest &request) {
        return false;
    }

    bool IHttpAction::query(const HttpRequest &request, const SqlSelectFilter &filter, DataTable &table) {
        return false;
    }

    BaseHttpMapping::BaseHttpMapping(const HttpMethod &method, const String &path) : _method(method), _path(path) {
    }

    BaseHttpMapping::~BaseHttpMapping() {
    }

    bool BaseHttpMapping::match(const HttpRequest &request) const {
        return request.method == method() && request.match(_path);
    }

    const HttpMethod &BaseHttpMapping::method() const {
        return _method;
    }

    void IHttpSession::registerTokenId(const String &tokenId) {
        _tokenId = tokenId;
    }

    void IHttpInterceptor::addWhitelist(const StringArray &list) {
        for (uint i = 0; i < list.count(); i++) {
            String path = list[i].trim();
            _whitelist.add(path, path);
        }
    }

    HttpService::HttpService() : _actions(false), _sessionTimer(nullptr) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IHttpRegister>(this);
        factory->addService<IHttpSession>(this);
        factory->addService<IHttpInterceptor>(this);

        _httpHeaders.add("Cache-Control", "no-cache");
        _httpHeaders.add("Strict-Transport-Security", "max-age=63072000; includeSubdomains; preload");
        _httpHeaders.add("Content-Security-Policy",
                         "default-src 'self'; img-src 'self' data:; script-src 'self' 'unsafe-inline' 'unsafe-eval'; style-src 'self' 'unsafe-inline'; connect-src 'self'; font-src 'self' data:;");
        _httpHeaders.add("X-Content-Type-Options", "nosniff");
        _httpHeaders.add("X-XSS-Protection", "1; mode=block");
    }

    HttpService::~HttpService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IHttpRegister>();
        factory->removeService<IHttpSession>();
        factory->removeService<IHttpInterceptor>();
    }

    bool HttpService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        bool enable = true;
        if (cs->getProperty("server.enabled", enable) && !enable) {
            return false;
        }
        String scheme;
        if (!(cs->getProperty("server.scheme", scheme) && (scheme == "http" || scheme == "https"))) {
            return false;
        }
        Port serverPort;
        if (!cs->getProperty("server.port", serverPort)) {
            Trace::error("server.port is incorrect.");
            return false;
        }

        HttpServer::Context context(Endpoint("any", serverPort));
        HttpServer::Actions actions(this, onAction);
        if (!_httpServer.startHttpServer(context, actions)) {
            Trace::writeLine("Can not start http server.", Trace::Error);
            return false;
        }

        // add http header from yml file.
        static const uint MaxHeader = 200;
        for (uint i = 0; i < MaxHeader; i++) {
            const String prefix = String::format("server.http.headers[%d]", i);
            const String k = String::format("%s.name", prefix.c_str());
            const String v = String::format("%s.value", prefix.c_str());
            String key, value;
            if (cs->getProperty(k, key) && cs->getProperty(v, value)) {
                if (!key.isNullOrEmpty() && !value.isNullOrEmpty())
                    _httpHeaders.add(key, value);
            } else {
                break;
            }
        }

        // session
        bool sessionEnabled = false;
        if (cs->getProperty("server.http.session.enabled", sessionEnabled) && sessionEnabled) {
            TimeSpan timeout = TimeSpan::fromHours(1);
            cs->getProperty("server.http.session.timeout", timeout);
            const TimeSpan interval = TimeSpan::fromMinutes(1);
            _sessionTimer = new Timer("server.http.session.timer",
                                      TimerCallback<HttpService>(this, &HttpService::sessionTimeUp),
                                      interval);
        }

        // ext mine types.
        static const uint MaxTypes = 200;
        for (uint i = 0; i < MaxTypes; i++) {
            const String prefix = String::format("server.http.mineTypes[%d]", i);
            const String k = String::format("%s.extName", prefix.c_str());
            const String v = String::format("%s.type", prefix.c_str());
            String key, value;
            if (cs->getProperty(k, key) && cs->getProperty(v, value)) {
                if (!key.isNullOrEmpty() && !value.isNullOrEmpty()) {
                    if (key[0] != '.')
                        key = String(".") + key;
                    _extMineTypes.add(key, value);
                }
            } else {
                break;
            }
        }

        return true;
    }

    bool HttpService::unInitialize() {
        if (_sessionTimer != nullptr) {
            delete _sessionTimer;
            _sessionTimer = nullptr;
        }

        _httpServer.stop();

        return true;
    }

    void HttpService::registerAction(IHttpAction *action) {
        Locker locker(&_actionsMutex);
        _actions.add(action);
    }

    void HttpService::deregisterAction(IHttpAction *action) {
        Locker locker(&_actionsMutex);
        _actions.remove(action);
    }

    void HttpService::registerWebServer(const String &webPath, const StringArray &homePages) {
        _webPath = webPath;
        if (homePages.count() > 0)
            _homePages = homePages;
        else {
            _homePages.addArray("index.html", "index.htm", nullptr);
        }
    }

    HttpStatus HttpService::onAction(void *parameter, const HttpRequest &request, HttpResponse &response) {
        HttpService *service = (HttpService *) parameter;
        assert(service);
        return service->onAction(request, response);
    }

    HttpStatus HttpService::onAction(const HttpRequest &request, HttpResponse &response) {
        response.headers.addRange(_httpHeaders);

        if (!isInWhitelist(request)) {
            if (!isAuthorized(request, _tokenId)) {
                response.setContent(String::format("{\"msg\":\"Request method '%s' not authorized!\",\"code\":\"401\"}",
                                                   request.method.method.c_str()));
                return HttpStatus::HttpUnauthorized;
            }
        }

        HttpStatus status;
        status = onActionProcess(request, response);
        if (status != HttpStatus::HttpNotFound) {
            updateSession(request, _tokenId);
            return status;
        }

        status = onMappingProcess(request, response);
        if (status != HttpStatus::HttpNotFound) {
            updateSession(request, _tokenId);
            return status;
        }

        status = onWebServerProcess(request, response);
        if (status != HttpStatus::HttpNotFound)
            return status;

        response.setContent(String::format("{\"msg\":\"Request method '%s' not found!\",\"code\":\"404\"}",
                                           request.method.method.c_str()));
        return HttpStatus::HttpNotFound;
    }

    HttpStatus HttpService::onActionProcess(const HttpRequest &request, HttpResponse &response) {
        Locker locker(&_actionsMutex);
        for (uint i = 0; i < _actions.count(); i++) {
            IHttpAction *action = _actions[i];
            HttpStatus status = action->onAction(request, response);
            if (status != HttpStatus::HttpNotFound)
                return status;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus HttpService::onMappingProcess(const HttpRequest &request, HttpResponse &response) {
        Locker locker(&_mappingsMutex);
        for (uint i = 0; i < _mappings.count(); i++) {
            BaseHttpMapping *mapping = _mappings[i];
            if (mapping->match(request)) {
                if (mapping->method() == HttpMethod::Options) {
                    return HttpStatus::HttpOk;
                } else {
                    HttpStatus status = mapping->execute(request, response);
                    if (status != HttpStatus::HttpNotFound)
                        return status;
                }
            }
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus HttpService::onWebServerProcess(const HttpRequest &request, HttpResponse &response) {
        // process web server.
        if (Directory::exists(_webPath)) {
            StringArray files;
            String fileName = Path::combine(_webPath,
                                            !request.url.relativeUrl().isNullOrEmpty() ? request.url.relativeUrl()
                                                                                       : existHomePage());
            if (File::exists(fileName)) {
                String type;
                if (getMimeType(fileName, type)) {
                    response.setContent(
                            new FileStream(fileName, FileMode::FileOpenWithoutException, FileAccess::FileRead));

                    type = String::format("%s; charset=UTF-8", type.c_str());
                    response.headers.add("Content-Type", type);
                } else {
                    Trace::error(String::format("Can not find file'%s' mime type!", fileName.c_str()));
                }
                return HttpStatus::HttpOk;
            } else {
                Trace::error(String::format("Can not find file'%s', request url: '%s'!", fileName.c_str(),
                                            request.url.toString().c_str()));
            }
        } else {
            Trace::error(String::format("Can not find root path'%s'!", _webPath.c_str()));
        }
        return HttpStatus::HttpNotFound;
    }

    String HttpService::existHomePage() const {
        for (uint i = 0; i < _homePages.count(); i++) {
            const String &homePage = _homePages[i];
            if (File::exists(Path::combine(_webPath, homePage)))
                return homePage;
        }
        return String::Empty;
    }

    String HttpService::addSession(const String &name, const TimeSpan &expiredTime, bool kickout) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        TimeSpan timeout = expiredTime;
        if (timeout == TimeSpan::Zero)
            cs->getProperty("server.http.session.timeout", timeout);
        cs->getProperty("server.http.session.kickout", kickout);

        String token = Uuid::generate().toString().replace("-", "").toLower();
        HttpSession *session = new HttpSession(token, name, timeout, kickout);
        Locker locker(&_sessionsMutex);
        _sessions.add(token, session);
        return token;
    }

    bool HttpService::removeSession(const HttpRequest &request) {
        const String &tokenId = _tokenId;
        String token;
        if (!request.headers.getValue(tokenId, token) || token.isNullOrEmpty()) {
            if (!request.properties.at(tokenId, token) || token.isNullOrEmpty())
                return false;
        }

        return removeSession(token);
    }

    bool HttpService::removeSession(const String &token) {
        Locker locker(&_sessionsMutex);
        return _sessions.remove(token);
    }

    bool HttpService::isAuthorized(const HttpRequest &request, const String &tokenId) {
        if (!tokenId.isNullOrEmpty()) {
            // check session token.
            String token;
            if (!request.headers.getValue(tokenId, token) || token.isNullOrEmpty()) {
                if (!request.properties.at(tokenId, token) || token.isNullOrEmpty())
                    return false;
            }

            Locker locker(&_sessionsMutex);
            if (!_sessions.contains(token)) {
                return false;
            }
        }
        return true;
    }

    void HttpService::updateSession(const HttpRequest &request, const String &tokenId) {
        if (!tokenId.isNullOrEmpty()) {
            // check session token.
            String token;
            if (!request.properties.at(tokenId, token))
                return;

            Locker locker(&_sessionsMutex);
            if (_sessions.contains(token)) {
                _sessions.update(token);
            }
        }
    }

    void HttpService::sessionTimeUp() {
        Locker locker(&_sessionsMutex);
        _sessions.checkSessions();
    }

    bool HttpService::isInWhitelist(const HttpRequest &request) {
        if (_whitelist.count() == 0)
            return true;

        const String path = !request.url.relativeUrl().isNullOrEmpty() ? request.url.relativeUrl() : existHomePage();
//        Trace::debug(String::format("http: %s", path.c_str()));

        if (_whitelist.contains(path))
            return true;

        StringArray paths;
        _whitelist.keys(paths);
        for (uint i = 0; i < paths.count(); i++) {
            const String &p = paths[i];
            String directoryName = Path::getDirectoryName(p);
            String fileName = Path::getFileName(p);
            if (fileName == "*") {
                if (path.find(directoryName + "/") >= 0)
                    return true;
            }
        }
        return false;
    }

    /**
     * Get MIME type header
     *
     */
    bool HttpService::getMimeType(const String &name, String &value) {
        int index = name.findLastOf('.');
        if (index <= 0)
            return false;

        String extName = name.substr(index, name.length() - index);

        /* Text */
        if (String::equals(extName, ".txt", true)) {
            value = "text/plain";
        } else if (String::equals(extName, ".css", true)) {
            value = "text/css";
        } else if (String::equals(extName, ".js", true)) {
            value = "text/javascript";
        } else if (String::equals(extName, ".xml", true) || String::equals(extName, ".xsl", true)) {
            value = "text/xml";
        } else if (String::equals(extName, ".xhtm", true) || String::equals(extName, ".xhtml", true) ||
                   String::equals(extName, ".xht", true)) {
            value = "application/xhtml+xml";
        } else if (String::equals(extName, ".html", true) || String::equals(extName, ".htm", true) ||
                   String::equals(extName, ".shtml", true) || String::equals(extName, ".hts", true)) {
            value = "text/html";

            /* Images */
        } else if (String::equals(extName, ".gif", true)) {
            value = "image/gif";
        } else if (String::equals(extName, ".png", true)) {
            value = "image/png";
        } else if (String::equals(extName, ".bmp", true)) {
            value = "application/x-MS-bmp";
        } else if (String::equals(extName, ".jpg", true) || String::equals(extName, ".jpeg", true) ||
                   String::equals(extName, ".jpe", true) || String::equals(extName, ".jpz", true) ||
                   String::equals(extName, ".jfif", true)) {
            value = "image/jpeg";
        } else if (String::equals(extName, ".ico", true)) {
            value = "image/x-icon";
        } else if (String::equals(extName, ".tif", true) || String::equals(extName, ".tiff", true)) {
            value = "image/tiff";
        } else if (String::equals(extName, ".wbmp", true)) {
            value = "image/vnd.wap.wbmp";
        } else if (String::equals(extName, ".fax", true)) {
            value = "image/fax";
        } else if (String::equals(extName, ".net", true)) {
            value = "image/pnetvue";
        } else if (String::equals(extName, ".rp", true)) {
            value = "image/vnd.rn-realpix";

            /* Audio & Video */
        } else if (String::equals(extName, ".wav", true)) {
            value = "audio/wav";
        } else if (String::equals(extName, ".wma", true)) {
            value = "audio/x-ms-wma";
        } else if (String::equals(extName, ".wmv", true)) {
            value = "audio/x-ms-wmv";
        } else if (String::equals(extName, ".au", true) || String::equals(extName, ".snd", true)) {
            value = "audio/basic";
        } else if (String::equals(extName, ".midi", true) || String::equals(extName, ".mid", true)) {
            value = "audio/midi";
        } else if (String::equals(extName, ".mp3", true) || String::equals(extName, ".mp2", true)) {
            value = "audio/x-mpeg";
        } else if (String::equals(extName, ".rm", true) || String::equals(extName, ".rmvb", true) ||
                   String::equals(extName, ".rmm", true)) {
            value = "audio/x-pn-realaudio";
        } else if (String::equals(extName, ".avi", true)) {
            value = "video/x-msvideo";
        } else if (String::equals(extName, ".3gp", true)) {
            value = "video/3gpp";
        } else if (String::equals(extName, ".mov", true)) {
            value = "video/quicktime";
        } else if (String::equals(extName, ".wmx", true)) {
            value = "video/x-ms-wmx";
        } else if (String::equals(extName, ".asf", true) || String::equals(extName, ".asx", true)) {
            value = "video/x-ms-asf";
        } else if (String::equals(extName, ".mp4", true) || String::equals(extName, ".mpg4", true)) {
            value = "video/mp4";
        } else if (String::equals(extName, ".mpe", true) || String::equals(extName, ".mpeg", true) ||
                   String::equals(extName, ".mpg", true) || String::equals(extName, ".mpga", true)) {
            value = "video/mpeg";

            /* Documents */
        } else if (String::equals(extName, ".pdf", true)) {
            value = "application/pdf";
        } else if (String::equals(extName, ".rtf", true)) {
            value = "application/rtf";
        } else if (String::equals(extName, ".doc", true) || String::equals(extName, ".dot", true)) {
            value = "application/msword";
        } else if (String::equals(extName, ".xls", true) || String::equals(extName, ".xla", true)) {
            value = "application/msexcel";
        } else if (String::equals(extName, ".hlp", true) || String::equals(extName, ".chm", true)) {
            value = "application/mshelp";
        } else if (String::equals(extName, ".swf", true) || String::equals(extName, ".swfl", true) ||
                   String::equals(extName, ".cab", true)) {
            value = "application/x-shockwave-flash";
        } else if (String::equals(extName, ".ppt", true) || String::equals(extName, ".ppz", true) ||
                   String::equals(extName, ".pps", true) || String::equals(extName, ".pot", true)) {
            value = "application/mspowerpoint";

            /* Binary & Packages */
        } else if (String::equals(extName, ".zip", true)) {
            value = "application/zip";
        } else if (String::equals(extName, ".rar", true)) {
            value = "application/x-rar-compressed";
        } else if (String::equals(extName, ".gz", true)) {
            value = "application/x-gzip";
        } else if (String::equals(extName, ".jar", true)) {
            value = "application/java-archive";
        } else if (String::equals(extName, ".tgz", true) || String::equals(extName, ".tar", true)) {
            value = "application/x-tar";
        } else if (String::equals(extName, ".ttf", true)) {
            value = "application/x-font-truetype";
        } else if (String::equals(extName, ".woff", true)) {
            value = "application/x-font-woff";
        } else if (String::equals(extName, ".woff2", true)) {
            value = "application/x-font-woff2";
        } else if (String::equals(extName, ".wasm", true)) {
            value = "application/wasm";

            /* wasm file & project file */
        } else if (String::equals(extName, ".epk", true)) {
            value = "application/zip";
        } else if (String::equals(extName, ".data", true)) {
            value = "application/data";

            /* ext files */
        } else {
            return _extMineTypes.at(extName, value);
        }
        return true;
    }
}
