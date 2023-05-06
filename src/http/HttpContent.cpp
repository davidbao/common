//
//  HttpContent.cpp
//  common
//
//  Created by baowei on 2020/3/24.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "http/HttpContent.h"
#include "thread/TickTimeout.h"
#include "diag/Trace.h"
#include "curl/curl.h"

using namespace Diag;

namespace Http {
    HttpHeader::HttpHeader(const String &name, const String &value) : name(name), value(value) {
    }

    HttpHeader::HttpHeader(const HttpHeader &other) {
        HttpHeader::evaluates(other);
    }

    HttpHeader::~HttpHeader() = default;

    bool HttpHeader::equals(const HttpHeader &other) const {
        return this->name == other.name && this->value == other.value;
    }

    void HttpHeader::evaluates(const HttpHeader &other) {
        this->name = other.name;
        this->value = other.value;
    }

    HttpHeader &HttpHeader::operator=(const HttpHeader &other) {
        if (this != &other) {
            HttpHeader::evaluates(other);
        }
        return *this;
    }

    String HttpHeader::toString() const {
        return String::format("%s:%s", name.c_str(), value.c_str());
    }

    const HttpHeaders HttpHeaders::JsonTypeHeaders({HttpHeader("Content-Type", "application/json")});

    HttpHeaders::HttpHeaders(size_t capacity) : List(capacity) {
    }

    HttpHeaders::HttpHeaders(const HttpHeaders &array) = default;

    HttpHeaders::HttpHeaders(HttpHeaders &&array) noexcept = default;

    HttpHeaders::HttpHeaders(const HttpHeaders &array, off_t offset, size_t count) :
            List(array, offset, count) {
    }

    HttpHeaders::HttpHeaders(const HttpHeader *array, size_t count, size_t capacity) :
            List(array, count, capacity) {
    }

    HttpHeaders::HttpHeaders(std::initializer_list<HttpHeader> list) : List(list) {
    }

    HttpHeaders::~HttpHeaders() = default;

    HttpHeaders &HttpHeaders::operator=(const HttpHeaders &other) {
        if (this != &other) {
            HttpHeaders::evaluates(other);
        }
        return *this;
    }

    bool HttpHeaders::parse(const String &str, HttpHeaders &headers) {
        StringArray lines;
        StringArray::parse(str, lines, Char::NewLine);
        if (lines.count() > 0) {
//            const String& status = lines[0];
            for (size_t i = 0; i < lines.count(); i++) {
//                Debug::writeFormatLine("%s", lines[i].c_str());

                const String &line = lines[i].trim('\r', ' ');
                if (line.isNullOrEmpty())
                    break;

                ssize_t index;
                if ((index = line.find(':')) > 0) {
                    String name = line.substr(0, index).trim(' ');
                    String value = line.substr(index + 1, line.length() - index - 1).trim(' ');
                    headers.add(name, value);
                }
            }
            return true;
        }
        return false;
    }

    bool HttpHeaders::getValue(const String &name, String &value) const {
        for (size_t i = 0; i < count(); i++) {
            const HttpHeader &header = at(i);
            if (header.name == name) {
                value = header.value;
                return true;
            }
        }
        return false;
    }

    bool HttpHeaders::contains(const String &name) const {
        for (size_t i = 0; i < count(); i++) {
            const HttpHeader &header = at(i);
            if (name == header.name)
                return true;
        }
        return false;
    }

    void HttpHeaders::add(const String &name, const String &value) {
        if (!contains(name)) {
            List::add(HttpHeader(name, value));
        }
    }

    String HttpHeaders::at(const String &name) const {
        for (size_t i = 0; i < count(); i++) {
            const HttpHeader &header = at(i);
            if (name == header.name)
                return header.value;
        }
        return String::Empty;
    }

    bool HttpHeaders::isTextContent() const {
        String contentType;
        getValue("Content-Type", contentType);
        contentType = contentType.toLower();
        if (contentType.isNullOrEmpty() ||
            contentType.find("text") >= 0 || contentType.find("charset=") >= 0 ||
            contentType.find("json") >= 0 || contentType.find("xml") >= 0 ||
            contentType.find("javascript") >= 0 ||
            contentType.find("application/x-www-form-urlencoded") >= 0) {
            return true;
        }
        return false;
    }

    HttpCookie::HttpCookie() : _maxAge(0), _secure(false), _httpOnly(false), _sameSite(SameSites::None) {
    }

    HttpCookie::HttpCookie(const HttpCookie &other) :
            _maxAge(0), _secure(false), _httpOnly(false), _sameSite(SameSites::None) {
        HttpCookie::evaluates(other);
    }

    HttpCookie::~HttpCookie() = default;

    bool HttpCookie::equals(const HttpCookie &other) const {
        return this->_values == other._values &&
               this->_domain == other._domain &&
               this->_path == other._path &&
               this->_expires == other._expires &&
               this->_maxAge == other._maxAge &&
               this->_secure == other._secure &&
               this->_httpOnly == other._httpOnly &&
               this->_sameSite == other._sameSite;
    }

    void HttpCookie::evaluates(const HttpCookie &other) {
        this->_values = other._values;
        this->_domain = other._domain;
        this->_path = other._path;
        this->_expires = other._expires;
        this->_maxAge = other._maxAge;
        this->_secure = other._secure;
        this->_httpOnly = other._httpOnly;
        this->_sameSite = other._sameSite;
    }

    HttpCookie &HttpCookie::operator=(const HttpCookie &other) {
        if (this != &other) {
            HttpCookie::evaluates(other);
        }
        return *this;
    }

    void HttpCookie::add(const String &name, const String &value) {
        _values.add(name, value);
    }

    bool HttpCookie::getValue(const String &name, String &value) const {
        return _values.at(name, value);
    }

    String HttpCookie::at(const String &name) const {
        String value;
        getValue(name, value);
        return value;
    }

    bool HttpCookie::isEmpty() const {
        return _values.count() == 0;
    }

    String HttpCookie::toString() const {
        if (_values.count() == 0)
            return String::Empty;

        String str;
        StringArray names;
        _values.keys(names);
        for (size_t i = 0; i < names.count(); i++) {
            const String &name = names[i];
            String value;
            _values.at(name, value);
            str.append(String::format("%s=%s", name.c_str(), value.c_str()));
            if (i < names.count() - 1)
                str.append("; ");
        }

        if (!_expires.isNullOrEmpty() && _maxAge == 0) {
            str.append("; ");
            str.append(String::format("%s=%s", "Expires", _expires.c_str()));
        }
        if (_maxAge > 0) {
            str.append("; ");
            str.append(String::format("%s=%s", "Max-Age", Int32(_maxAge).toString().c_str()));
        }
        if (!_domain.isNullOrEmpty()) {
            str.append("; ");
            str.append(String::format("%s=%s", "Domain", _domain.c_str()));
        }
        if (!_path.isNullOrEmpty()) {
            str.append("; ");
            str.append(String::format("%s=%s", "Path", _path.c_str()));
        }
        if (_secure) {
            str.append("; ");
            str.append("Secure");
        }
        if (_httpOnly) {
            str.append("; ");
            str.append("HttpOnly");
        }
        if (_sameSite != SameSites::None) {
            str.append("; ");
            str.append(String::format("%s=%s", "SameSite", toSameSiteStr(_sameSite).c_str()));
        }
        return str;
    }

    const String &HttpCookie::domain() const {
        return _domain;
    }

    const String &HttpCookie::path() const {
        return _path;
    }

    const String &HttpCookie::expires() const {
        return _expires;
    }

    int HttpCookie::maxAge() const {
        return _maxAge;
    }

    bool HttpCookie::secure() const {
        return _secure;
    }

    bool HttpCookie::httpOnly() const {
        return _httpOnly;
    }

    HttpCookie::SameSites HttpCookie::sameSite() const {
        return _sameSite;
    }

    void HttpCookie::setDomain(const String &value) {
        _domain = value;
    }

    void HttpCookie::setPath(const String &value) {
        _path = value;
    }

    void HttpCookie::setExpires(const String &value) {
        _expires = value;
    }

    void HttpCookie::setMaxAge(const int &value) {
        _maxAge = value;
    }

    void HttpCookie::setSecure(const bool &value) {
        _secure = value;
    }

    void HttpCookie::setHttpOnly(const bool &value) {
        _httpOnly = value;
    }

    void HttpCookie::setSameSite(const SameSites &value) {
        _sameSite = value;
    }

    bool HttpCookie::parse(const String &str, HttpCookie &cookie) {
        StringArray texts;
        StringArray::parse(str, texts, ';');
        for (size_t i = 0; i < texts.count(); i++) {
            String text = texts[i];
            text = text.trimStart(' ');
            text = text.trimEnd(' ');
            if (text.find("Expires") >= 0) {
                cookie._expires = getParameter(text);
            } else if (text.find("Max-Age") >= 0) {
                Int32::parse(getParameter(text), cookie._maxAge);
            } else if (text.find("Domain") >= 0) {
                cookie._domain = getParameter(text);
            } else if (text.find("Path") >= 0) {
                cookie._path = text;
            } else if (String::equals(text, "Secure")) {
                cookie._secure = true;
            } else if (String::equals(text, "HttpOnly")) {
                cookie._httpOnly = true;
            } else if (text.find("SameSite") >= 0) {
                cookie._sameSite = parseSameSite(getParameter(text));
            } else {
                String key, value;
                if (parseKeyValue(text, key, value)) {
                    cookie.add(key, value);
                }
            }
        }
        return !cookie.isEmpty();
    }

    bool HttpCookie::parseKeyValue(const String &str, String &key, String &value) {
        StringArray texts;
        StringArray::parse(str, texts, '=');
        if (texts.count() == 2) {
            key = texts[0];
            value = texts[1];
            return true;
        }
        return false;
    }

    String HttpCookie::getParameter(const String &str) {
        String key, value;
        parseKeyValue(str, key, value);
        return value;
    }

    HttpCookie::SameSites HttpCookie::parseSameSite(const String &str) {
        if (String::equals(str, "Strict"))
            return SameSites::Strict;
        else if (String::equals(str, "Lax"))
            return SameSites::Lax;
        else
            return SameSites::None;
    }

    String HttpCookie::toSameSiteStr(SameSites value) {
        switch (value) {
            case SameSites::Strict:
                return "Strict";
            case SameSites::Lax:
                return "Lax";
            default:
                return "None";
        }
    }

    HttpSession::HttpSession(const String &token, const String &name, const TimeSpan &expiredTime, bool kickout) :
            _token(token), _name(name), _expiredTime(expiredTime), _kickout(kickout), _start(0) {
        update();
    }

    HttpSession::HttpSession(const HttpSession &other) :
            _kickout(false), _start(0) {
        HttpSession::evaluates(other);
    }

    HttpSession::~HttpSession() = default;

    bool HttpSession::equals(const HttpSession &other) const {
        return this->_token == other._token &&
               this->_name == other._name &&
               this->_expiredTime == other._expiredTime &&
               this->_kickout == other._kickout &&
               this->_start == other._start;
    }

    void HttpSession::evaluates(const HttpSession &other) {
        this->_token = other._token;
        this->_name = other._name;
        this->_expiredTime = other._expiredTime;
        this->_kickout = other._kickout;
        this->_start = other._start;
    }

    HttpSession &HttpSession::operator=(const HttpSession &other) {
        if (this != &other) {
            HttpSession::evaluates(other);
        }
        return *this;
    }

    const String &HttpSession::token() const {
        return _token;
    }

    const String &HttpSession::name() const {
        return _name;
    }

    const TimeSpan &HttpSession::expiredTime() const {
        return _expiredTime;
    }

    bool HttpSession::kickout() const {
        return _kickout;
    }

    void HttpSession::update() {
        _start = TickTimeout::getCurrentTickCount();
//        Trace::debug(String::format("update a session, name: %s, token: %s, expired: %s, kickout: %s",
//                     name().c_str(), token().c_str(),
//                     expiredTime().toString().c_str(),
//                     Boolean(kickout()).toString().c_str()));
    }

    bool HttpSession::isExpired() const {
        if (_expiredTime == TimeSpan::Zero)
            return false;
        else
            return TickTimeout::isTimeout(_start, _expiredTime);
    }

    void HttpSessions::add(const String &token, HttpSession *session) {
        const HttpSession *sameName = atByName(session->name());
        if (sameName != nullptr && sameName->kickout())
            _sessions.remove(sameName->token());
        _sessions.add(token, session);

        Trace::debug(String::format("Add a session, name: %s, token: %s, expired: %s, kickout: %s",
                                    session->name().c_str(), session->token().c_str(),
                                    session->expiredTime().toString().c_str(),
                                    Boolean(session->kickout()).toString().c_str()));
    }

    bool HttpSessions::remove(const String &token) {
        HttpSession *session = nullptr;
        if (_sessions.at(token, session) && session != nullptr) {
            Trace::debug(String::format("Remove a session, name: %s, token: %s, expired: %s, kickout: %s",
                                        session->name().c_str(), session->token().c_str(),
                                        session->expiredTime().toString().c_str(),
                                        Boolean(session->kickout()).toString().c_str()));
        }
        return _sessions.remove(token);
    }

    bool HttpSessions::contains(const String &token) const {
        return _sessions.contains(token);
    }

    HttpSession *HttpSessions::at(const String &token) const {
        HttpSession *session = nullptr;
        if (_sessions.at(token, session))
            return session;
        else
            return nullptr;
    }

    HttpSession *HttpSessions::atByName(const String &name) const {
        Vector<String> keys;
        _sessions.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            HttpSession *value = nullptr;
            if (_sessions.at(key, value) && value != nullptr && value->name() == name) {
                return value;
            }
        }
        return nullptr;
    }

    void HttpSessions::checkSessions() {
        Vector<String> keys;
        _sessions.keys(keys);
        StringArray removed;
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            HttpSession *session = nullptr;
            if (_sessions.at(key, session) && session != nullptr && session->isExpired()) {
                removed.add(key);
            }
        }

        for (size_t i = 0; i < removed.count(); i++) {
            remove(removed[i]);
        }
    }

    void HttpSessions::update(const String &token) {
        HttpSession *session = at(token);
        if (session != nullptr)
            session->update();
    }

    size_t HttpSessions::count() const {
        return _sessions.count();
    }

    HttpContent::HttpContent() = default;

    HttpContent::~HttpContent() = default;

    String HttpContent::escape(const String &str) {
        String result;
        CURL *curl = curl_easy_init();
        if (curl) {
            char *output = curl_easy_escape(curl, str, (int) str.length());
            if (output) {
                result = output;
                curl_free(output);
            } else {
                result = str;
            }
            curl_easy_cleanup(curl);
        } else {
            result = str;
        }
        return result;
    }

    String HttpContent::unescape(const String &str) {
        String result;
        CURL *curl = curl_easy_init();
        if (curl) {
            int decodelen;
            char *decoded = curl_easy_unescape(curl, str, (int) str.length(), &decodelen);
            if (decoded) {
                result = String(decoded, decodelen);
                curl_free(decoded);
            } else {
                result = str;
            }
            curl_easy_cleanup(curl);
        } else {
            result = str;
        }
        return result;
    }

    HttpStringContent::HttpStringContent() {
        _stream = new MemoryStream();
    }

    HttpStringContent::HttpStringContent(const String &value) : _value(value) {
        _stream = new MemoryStream((const uint8_t *) value.c_str(), value.length());
    }

    HttpStringContent::HttpStringContent(const HttpStringContent &content) : _stream(nullptr) {
        HttpStringContent::evaluates(content);
    }

    HttpStringContent::~HttpStringContent() {
        delete _stream;
        _stream = nullptr;
    }

    bool HttpStringContent::equals(const HttpContent &other) const {
        auto content = dynamic_cast<const HttpStringContent *>(&other);
        return content != nullptr && _value == content->_value;
    }

    void HttpStringContent::evaluates(const HttpContent &other) {
        auto content = dynamic_cast<const HttpStringContent *>(&other);
        if (content != nullptr) {
            _value = content->_value;
            _stream = new MemoryStream((const uint8_t *) _value.c_str(), _value.length());
        }
    }

    void HttpStringContent::write(void *buffer, size_t size, size_t nmemb) {
        _value.append(String((const char *) buffer, (uint32_t) (size * nmemb)));
    }

    ssize_t HttpStringContent::read(void *buffer, size_t size, size_t nmemb) {
        return _stream->read((uint8_t *) buffer, 0, (uint32_t) (size * nmemb));
    }

    size_t HttpStringContent::size() {
        return _value.length();
    }

    HttpContent *HttpStringContent::clone() const {
        return new HttpStringContent(*this);
    }

    const String &HttpStringContent::value() const {
        return _value;
    }

    HttpJsonContent::HttpJsonContent() = default;

    HttpJsonContent::HttpJsonContent(const String &value) : HttpStringContent(value) {
    }

    HttpJsonContent::HttpJsonContent(const JsonNode &value) : HttpJsonContent(value.toString()) {
    }

    HttpJsonContent::HttpJsonContent(const HttpJsonContent &content) {
        HttpJsonContent::evaluates(content);
    }

    JsonNode HttpJsonContent::node() const {
        JsonNode node;
        JsonNode::parse(_value, node);
        return node;
    }

    HttpContent *HttpJsonContent::clone() const {
        return new HttpJsonContent(*this);
    }

    HttpByteArrayContent::HttpByteArrayContent() {
        _stream = new MemoryStream();
    }

    HttpByteArrayContent::HttpByteArrayContent(const ByteArray &value) : _value(value) {
        _stream = new MemoryStream(&_value, false);
    }

    HttpByteArrayContent::HttpByteArrayContent(const HttpByteArrayContent &content) : _stream(nullptr) {
        HttpByteArrayContent::evaluates(content);
    }

    HttpByteArrayContent::~HttpByteArrayContent() {
        delete _stream;
        _stream = nullptr;
    }

    bool HttpByteArrayContent::equals(const HttpContent &other) const {
        auto content = dynamic_cast<const HttpByteArrayContent *>(&other);
        return content != nullptr && _value == content->_value;
    }

    void HttpByteArrayContent::evaluates(const HttpContent &other) {
        auto content = dynamic_cast<const HttpByteArrayContent *>(&other);
        if (content != nullptr) {
            _value = content->_value;
        }
    }

    void HttpByteArrayContent::write(void *buffer, size_t size, size_t nmemb) {
        _value.addRange((const unsigned char *) buffer, 0, (uint32_t) (size * nmemb));
    }

    ssize_t HttpByteArrayContent::read(void *buffer, size_t size, size_t nmemb) {
        return _stream->read((uint8_t *) buffer, 0, (uint32_t) (size * nmemb));
    }

    size_t HttpByteArrayContent::size() {
        return _value.count();
    }

    HttpContent *HttpByteArrayContent::clone() const {
        return new HttpByteArrayContent(*this);
    }

    const ByteArray &HttpByteArrayContent::value() const {
        return _value;
    }

    HttpStreamContent::HttpStreamContent() {
        _stream = nullptr;
    }

    HttpStreamContent::HttpStreamContent(Stream *stream) : _stream(stream) {
    }

    HttpStreamContent::HttpStreamContent(const HttpStreamContent &content) : _stream(nullptr) {
        HttpStreamContent::evaluates(content);
    }

    HttpStreamContent::~HttpStreamContent() {
        _stream = nullptr;
    }

    bool HttpStreamContent::equals(const HttpContent &other) const {
        auto content = dynamic_cast<const HttpStreamContent *>(&other);
        return content != nullptr && _stream == content->_stream;
    }

    void HttpStreamContent::evaluates(const HttpContent &other) {
        auto content = dynamic_cast<const HttpStreamContent *>(&other);
        if (content != nullptr) {
            _stream = content->_stream;
        }
    }

    void HttpStreamContent::write(void *buffer, size_t size, size_t nmemb) {
        if (_stream != nullptr)
            _stream->write((const uint8_t *) buffer, 0, (uint32_t) (size * nmemb));
    }

    ssize_t HttpStreamContent::read(void *buffer, size_t size, size_t nmemb) {
        if (_stream != nullptr)
            return _stream->read((uint8_t *) buffer, 0, (uint32_t) (size * nmemb));
        return 0;
    }

    size_t HttpStreamContent::size() {
        return _stream != nullptr ? _stream->length() : 0;
    }

    HttpContent *HttpStreamContent::clone() const {
        return new HttpStreamContent(*this);
    }

    const String &HttpStreamContent::value() const {
        return _value;
    }

    Stream *HttpStreamContent::stream() const {
        return _stream;
    }

    const HttpMethod HttpMethod::Get("GET");
    const HttpMethod HttpMethod::Put("PUT");
    const HttpMethod HttpMethod::Post("POST");
    const HttpMethod HttpMethod::Delete("DELETE");
    const HttpMethod HttpMethod::Head("HEAD");
    const HttpMethod HttpMethod::Options("OPTIONS");
    const HttpMethod HttpMethod::Trace("TRACE");
    const HttpMethod HttpMethod::Connect("CONNECT");
    const HttpMethod HttpMethod::Patch("PATCH");

    HttpMethod::HttpMethod(const String &method) : method(method) {
    }

    HttpMethod::HttpMethod(const HttpMethod &method) : method(method.method) {
    }

    bool HttpMethod::equals(const HttpMethod &other) const {
        return String::equals(this->method, other.method, true);
    }

    void HttpMethod::evaluates(const HttpMethod &other) {
        this->method = other.method;
    }

    HttpMethod &HttpMethod::operator=(const HttpMethod &other) {
        evaluates(other);
        return *this;
    }

    HttpProperties::HttpProperties() : _values(false) {
    }

    HttpProperties::HttpProperties(const StringMap &properties) : HttpProperties() {
        _values.addRange(properties);
    }

    HttpProperties::HttpProperties(const HttpProperties &properties) {
        HttpProperties::evaluates(properties);
    }

    bool HttpProperties::equals(const HttpProperties &other) const {
        return _values == other._values;
    }

    void HttpProperties::evaluates(const HttpProperties &other) {
        _values = other._values;
    }

    void HttpProperties::evaluates(const StringMap &other) {
        _values = other;
    }

    HttpProperties &HttpProperties::operator=(const HttpProperties &other) {
        if (this != &other) {
            HttpProperties::evaluates(other);
        }
        return *this;
    }

    HttpProperties &HttpProperties::operator=(const StringMap &other) {
        HttpProperties::evaluates(other);
        return *this;
    }

    void HttpProperties::add(const String &key, const String &value) {
        _values.add(key, value);
    }

    bool HttpProperties::at(const String &key, String &value) const {
        return _values.at(key, value);
    }

    String HttpProperties::operator[](const String &key) const {
        return _values[key];
    }

    String HttpProperties::toString() const {
        String result;
        StringArray keys;
        _values.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (_values.at(key, value)) {
                static const char *valueFmt = "%s=%s";
                if (!result.isNullOrEmpty())
                    result.append('&');
                result.append(String::format(valueFmt, key.c_str(), HttpContent::escape(value).c_str()));
            }
        }
        return result;
    }

    size_t HttpProperties::count() const {
        return _values.count();
    }

    bool HttpProperties::parse(const String &value, HttpProperties &properties) {
        StringArray texts;
        StringArray::parse(value, texts, '&');
        for (size_t i = 0; i < texts.count(); i++) {
            const String &text = texts[i];
            StringArray texts2;
            StringArray::parse(text, texts2, '=');
            if (texts2.count() == 2) {
                properties.add(texts2[0], texts2[1]);
            }
        }
        return properties.count() > 0;
    }

    const char HttpRequest::PathSplitSymbol = '/';

    HttpRequest::HttpRequest(const Url &url, const HttpMethod &method) : url(url), method(method), content(nullptr),
                                                                         version("1.1"), verb(false) {
        headers.add("Accept", "*/*");
        headers.add("Accept-Encoding", "gzip, deflate, br");
        headers.add("Connection", "keep-alive");
        headers.add("Cache-Control", "no-cache");

//        // update cookie.
//        String cookieValue;
//        if(headers.getValue("Cookie", cookieValue) && !cookieValue.isNullOrEmpty())
//        {
//            HttpCookie::parse(cookieValue, _cookie);
//        }
    }

    HttpRequest::HttpRequest(const Url &url, const HttpMethod &method, const HttpHeaders &headers, HttpContent *content)
            : HttpRequest(url, method) {
        this->content = content;
        this->headers.addRange(headers);
    }

    HttpRequest::HttpRequest(const Url &url, const HttpMethod &method, const HttpHeaders &headers,
                             const StringMap &properties, HttpContent *content) : HttpRequest(url, method) {
        this->content = content;
        this->headers.addRange(headers);
        this->properties = properties;
    }

    HttpRequest::HttpRequest(const HttpRequest &request) : method(request.method), content(nullptr), verb(false) {
        HttpRequest::evaluates(request);
    }

    HttpRequest::~HttpRequest() {
        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
    }

    bool HttpRequest::equals(const HttpRequest &other) const {
        return url == other.url &&
               method == other.method &&
               headers == other.headers &&
               properties == other.properties &&
               (content != nullptr && other.content != nullptr) ? content->equals(*other.content) :
               (content == nullptr && other.content == nullptr) &&
               version == other.version &&
               userAgent == other.userAgent &&
               verb == other.verb;
    }

    void HttpRequest::evaluates(const HttpRequest &other) {
        url = other.url;
        method = other.method;
        headers = other.headers;
        properties = other.properties;
        content = other.content != nullptr ? other.content->clone() : nullptr;
        version = other.version;
        userAgent = other.userAgent;
        verb = other.verb;
    }

    size_t HttpRequest::contentSize() const {
        return content != nullptr ? content->size() : 0;
    }

    const String &HttpRequest::text() const {
        auto sc = dynamic_cast<const HttpStringContent *>(this->content);
        return sc != nullptr ? sc->value() : String::Empty;
    }

    bool HttpRequest::match(const String &path) const {
        StringArray paths, inputs;
        if (StringArray::parse(path, paths, PathSplitSymbol) &&
            StringArray::parse(url.relativeUrl(), inputs, PathSplitSymbol)) {
            for (size_t i = 0; i < paths.count(); i++) {
                const String &text = paths[i];
                const String &input = i < inputs.count() ? inputs[i] : String::Empty;
                if (text != input) {
                    if (isVarName(text))
                        return true;
                    return false;
                }
            }
            return inputs.count() == paths.count();
        }
        return false;
    }

    bool HttpRequest::getPropValue(const String &key, String &value) const {
        return this->properties.at(key, value);
    }

    String HttpRequest::getPropValue(const String &key) const {
        String value;
        this->properties.at(key, value);
        return value;
    }

    String HttpRequest::getPathSegment(int segment) const {
        StringArray inputs;
        if (StringArray::parse(url.relativeUrl(), inputs, PathSplitSymbol)) {
            return (size_t) segment < inputs.count() ? inputs[segment] : String::Empty;
        }
        return String::Empty;
    }

    bool HttpRequest::parseVarName(const String &value, String &varName) {
        ssize_t start = value.find("{");
        if (start >= 0)  // has variables
        {
            ssize_t end = value.find("}");
            if (end > start) {
                varName = value.substr(start + 1, end - start - 1);
                return true;
            }
        }
        return false;
    }

    bool HttpRequest::isVarName(const String &value) {
        String varName;
        return parseVarName(value, varName);
    }

    bool HttpRequest::findHeader(const String &name, const String &value) const {
        String v;
        if (headers.getValue(name, v))
            return v == value;
        return false;
    }

    String HttpRequest::toPropsStr() const {
        if (properties.count() == 0) {
            return String::Empty;
        }

        JsonNode node;
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            const String &value = it.value();
            if (value.find(';') >= 0) {
                // This is an array node.
                StringArray texts;
                StringArray::parse(value, texts, ';');
                node.add(JsonNode(it.key(), texts));
            } else {
                node.add(JsonNode(it.key(), value));
            }
        }
        return node.toString();
    }

    HttpResponse::HttpResponse(HttpContent *content) : request(nullptr), content(content), status(HttpNotImplemented),
                                                       version("1.1") {
    }

    HttpResponse::~HttpResponse() {
        if (request != nullptr)
            request = nullptr;

        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
    }

    bool HttpResponse::isTextContent() const {
        return headers.isTextContent();
    }

    void HttpResponse::setContent(const JsonNode &node) {
        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
        this->content = new HttpJsonContent(node);
    }

    void HttpResponse::setContent(const String &text) {
        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
        this->content = new HttpStringContent(text);
    }

    void HttpResponse::setContent(Stream *stream) {
        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
        this->content = new HttpStreamContent(stream);
    }

    HttpCode::Item::Item() : code(Unknown) {
    }

    HttpCode::Item::Item(int code, const String &msg) : code(code), msg(msg) {
    }

    HttpCode::Item::Item(const Item &item) = default;

    Dictionary<int, String> HttpCode::_codes;

    HttpCode HttpCode::_staticInstance;

    HttpCode::HttpCode() {
        registerCode({
                             {Success,            String::Empty},
                             {JsonParseError,     "Json string parse error."},
                             {DbError,            "Error occurred while trying to connect to the database."},
                             {ParameterIncorrect, "The request parameters is incorrect."},
                             {Unknown,            "Unknown"}
                     });
    }

    void HttpCode::registerCode(int code, const String &msg) {
        _codes[code] = msg;
    }

    void HttpCode::registerCode(std::initializer_list<Item> list) {
        for (const Item *code = list.begin(); code < list.end(); ++code) {
            _codes[code->code] = code->msg;
        }
    }

    String HttpCode::getMessage(int code) {
        return _codes[code];
    }

    StringMap HttpCode::at(int code) {
        StringMap result;
        if (_codes.contains(code)) {
            result["code"] = Int32(code).toString();
            result["msg"] = _codes[code];
        }
        return result;
    }

    StringMap HttpCode::okCode() {
        return at(Ok);
    }
}
