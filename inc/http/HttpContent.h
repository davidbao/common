//
//  HttpContent.h
//  common
//
//  Created by baowei on 2020/3/24.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef HttpContent_h
#define HttpContent_h

#include "data/ValueType.h"
#include "data/PrimitiveInterface.h"
#include "data/StringArray.h"
#include "data/PList.h"
#include "data/Map.h"
#include "data/NetType.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"

namespace Common {
    enum HttpStatus {
        HttpOk = 200,
        HttpCreated = 201,
        HttpAccepted = 202,
        HttpNonAuthoritativeInformation = 203,
        HttpNoContent = 204,
        HttpResetContent = 205,
        HttpPartialContent = 206,
        HttpMultipleChoices = 300,
        HttpMovedPermanently = 301,
        HttpFound = 302,
        HttpSeeOther = 303,
        HttpNotModified = 304,
        HttpUseProxy = 305,
        HttpTemporaryRedirect = 307,
        HttpBadRequest = 400,
        HttpUnauthorized = 401,
        HttpPaymentRequired = 402,
        HttpForbidden = 403,
        HttpNotFound = 404,
        HttpMethodNotAllowed = 405,
        HttpNotAcceptable = 406,
        HttpProxyAuthenticationRequired = 407,
        HttpRequestTimeout = 408,
        HttpConflict = 409,
        HttpGone = 410,
        HttpLengthRequired = 411,
        HttpPreconditionFailed = 412,
        HttpRequestEntityTooLarge = 413,
        HttpRequestUriTooLarge = 414,
        HttpUnsupportedMediaType = 415,
        HttpRequestedRangeNotSatisfiable = 416,
        HttpExpectationFailed = 417,
        HttpInternalServerError = 500,
        HttpNotImplemented = 501,
        HttpBadGateway = 502,
        HttpServiceUnavailable = 503,
        HttpGatewayTimeout = 504,
        HttpVersionNotSupported = 505
    };

    class HttpHeader {
    public:
        String name;
        String value;

        HttpHeader(const String &name = String::Empty, const String &value = String::Empty);

        HttpHeader(const HttpHeader &value);

        void copyFrom(const HttpHeader *value);

        void operator=(const HttpHeader &value);

        bool operator==(const HttpHeader &value) const;

        bool operator!=(const HttpHeader &value) const;

        String toString() const;
    };

    class HttpHeaders : public IIndexGetter<HttpHeader *> {
    public:
        HttpHeaders();

        HttpHeaders(const HttpHeader *item, ...);

        HttpHeaders(const HttpHeaders &value);

        template<class T>
        bool getValue(const String &name, T &value) const {
            String valueStr = getValueStr(name);
            return T::parse(valueStr, value);
        }

        bool getValue(const String &name, String &value) const;

        size_t count() const;

        bool contains(const String &name) const;

        void add(const String &name, const String &value);

        void add(const HttpHeader &header);

        void addRange(const HttpHeaders &headers);

        void clear();

        String at(const String &name) const;

        HttpHeader *at(size_t pos) const override;

        void operator=(const HttpHeaders &value);

        bool operator==(const HttpHeaders &value) const;

        bool operator!=(const HttpHeaders &value) const;

        bool isTextContent() const;

    public:
        static bool parse(const String &str, HttpHeaders &headers);

    private:
        void addInner(HttpHeader *header);

        String getValueStr(const String &name) const;

    private:
        CopyPList<HttpHeader> _values;

    public:
        static const HttpHeaders JsonTypeHeaders;
    };

    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
    class HttpCookie {
    public:
        enum SameSites {
            None,
            Strict,
            Lax
        };

        HttpCookie();

        void add(const String &name, const String &value);

        bool at(const String &name, String &value) const;

        bool isEmpty() const;

        const String toString() const;

        const String &domain() const;

        const String &path() const;

        const String &expires() const;

        int maxAge() const;

        bool secure() const;

        bool httpOnly() const;

        SameSites sameSite() const;

        void setDomain(const String &value);

        void setPath(const String &value);

        void setExpires(const String &value);

        void setMaxAge(const int &value);

        void setSecure(const bool &value);

        void setHttpOnly(const bool &value);

        void setSameSite(const SameSites &value);

    public:
        static bool parse(const String &str, HttpCookie &cookie);

    private:
        static bool parseKeyValue(const String &str, String &key, String &value);

        static String getParameter(const String &str);

        static SameSites parseSameSite(const String &str);

        static String toSameSiteStr(SameSites value);

    private:
        StringMap _values;

        // Defines the host to which the cookie will be sent.
        String _domain;
        // Indicates the path that must exist in the requested URL for the browser to send the Cookie header.
        String _path;
        // Indicates the maximum lifetime of the cookie as an HTTP-date timestamp.
        String _expires;
        // Indicates the number of seconds until the cookie expires.
        // A zero or negative number will expire the cookie immediately.
        // If both Expires and Max-Age are set, Max-Age has precedence.
        int _maxAge;
        bool _secure;
        bool _httpOnly;
        // Controls whether or not a cookie is sent with cross-origin requests,
        // providing some protection against cross-site request forgery attacks (CSRF).
        SameSites _sameSite;
    };

    class HttpSession {
    public:
        HttpSession(const String &token, const String &name, const TimeSpan &expiredTime = TimeSpan::Zero,
                    bool kickout = false);

        const String &token() const;

        const String &name() const;

        const TimeSpan &expiredTime() const;

        bool kickout() const;

        void update();

        bool isExpired() const;

    private:
        String _token;
        String _name;
        TimeSpan _expiredTime;
        bool _kickout;

        uint _start;
    };

    class HttpSessions {
    public:
        HttpSessions();

        void add(const String &token, HttpSession *session);

        bool remove(const String &token);

        bool contains(const String &token) const;

        HttpSession *at(const String &token) const;

        HttpSession *atByName(const String &name) const;

        void checkSessions();

        void update(const String &token);

    private:
        Map<String, HttpSession> _sessions;
    };

    class HttpContent
            : public IEquatable<HttpContent>, public IEvaluation<HttpContent>, public ICloneable<HttpContent> {
    public:
        HttpContent();

        ~HttpContent() override;

        virtual void write(void *buffer, size_t size, size_t nmemb) = 0;

        virtual int64_t read(void *buffer, size_t size, size_t nmemb) = 0;

        virtual int64_t size() = 0;

    public:
        static String escape(const String &str);

        static String unescape(const String &str);
    };

    class HttpStringContent : public HttpContent {
    public:
        HttpStringContent();

        HttpStringContent(const String &value);

        HttpStringContent(const HttpStringContent &content);

        ~HttpStringContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        int64_t read(void *buffer, size_t size, size_t nmemb) override;

        int64_t size() override;

        HttpContent *clone() const override;

        const String &value() const;

    protected:
        String _value;

    private:
        MemoryStream *_stream;
    };

    class HttpJsonContent : public HttpStringContent {
    public:
        HttpJsonContent();

        HttpJsonContent(const String &value);

        HttpJsonContent(const JsonNode &value);

        HttpJsonContent(const HttpJsonContent &content);

        HttpContent *clone() const override;

        JsonNode node() const;
    };

    class HttpByteArrayContent : public HttpContent {
    public:
        HttpByteArrayContent();

        HttpByteArrayContent(const ByteArray &value);

        HttpByteArrayContent(const HttpByteArrayContent &content);

        ~HttpByteArrayContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        int64_t read(void *buffer, size_t size, size_t nmemb) override;

        int64_t size() override;

        HttpContent *clone() const override;

        const ByteArray &value() const;

    private:
        ByteArray _value;
        MemoryStream *_stream;
    };

    class HttpStreamContent : public HttpContent {
    public:
        HttpStreamContent();

        HttpStreamContent(Stream *stream);

        HttpStreamContent(const HttpStreamContent &content);

        ~HttpStreamContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        int64_t read(void *buffer, size_t size, size_t nmemb) override;

        int64_t size() override;

        HttpContent *clone() const override;

        const String &value() const;

        Stream *stream() const;

    private:
        Stream *_stream;
        String _value;
    };

    class HttpMethod : public IEquatable<HttpMethod>, public IEvaluation<HttpMethod> {
    public:
        HttpMethod(const String &method);

        HttpMethod(const HttpMethod &method);

        bool equals(const HttpMethod &other) const override;

        void evaluates(const HttpMethod &other) override;

        HttpMethod &operator=(const HttpMethod &other);

    public:
        //
        // Summary:
        //     Represents an HTTP GET protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Get;
        //
        // Summary:
        //     Represents an HTTP PUT protocol method that is used to replace an entity identified
        //     by a URI.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Put;
        //
        // Summary:
        //     Represents an HTTP POST protocol method that is used to post a new entity as
        //     an addition to a URI.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Post;
        //
        // Summary:
        //     Represents an HTTP DELETE protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Delete;
        //
        // Summary:
        //     Represents an HTTP HEAD protocol method. The HEAD method is identical to GET
        //     except that the server only returns message-headers in the response, without
        //     a message-body.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Head;
        //
        // Summary:
        //     Represents an HTTP OPTIONS protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Options;
        //
        // Summary:
        //     Represents an HTTP TRACE protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Trace;
        //
        // Summary:
        //     Represents an HTTP CONNECT protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Connect;
        //
        // Summary:
        //     Represents an HTTP PATCH protocol method.
        //
        // Returns:
        //     Returns HttpMethod.
        static const HttpMethod Patch;

    public:
        //
        // Summary:
        //     An HTTP method.
        //
        // Returns:
        //     An HTTP method represented as a String.
        String method;
    };

    class HttpProperties
            : public IEquatable<HttpProperties>, public IEvaluation<HttpProperties>, public IEvaluation<StringMap> {
    public:
        HttpProperties();

        HttpProperties(const StringMap &properties);

        HttpProperties(const HttpProperties &properties);

        bool equals(const HttpProperties &other) const override;

        void evaluates(const HttpProperties &other) override;

        void evaluates(const StringMap &other) override;

        template<class T>
        void add(const String &key, const T &value) {
            add(key, value.toString());
        }

        void add(const String &key, const String &value);

        template<class T>
        bool at(const String &key, T &value) const {
            String str;
            if (at(key, str))
                return T::parse(str, value);
            return false;
        }

        bool at(const String &key, String &value) const;

        String operator[](const String &key) const;

        String toString() const;

        size_t count() const;

    public:
        static bool parse(const String &value, HttpProperties &properties);

    private:
        StringMap _values;
    };

    class HttpRequest : public IEquatable<HttpRequest>, public IEvaluation<HttpRequest> {
    public:
        HttpRequest(const Url &url, const HttpMethod &method);

        HttpRequest(const Url &url, const HttpMethod &method, const HttpHeaders &headers,
                    HttpContent *content = nullptr);

        HttpRequest(const Url &url, const HttpMethod &method, const HttpHeaders &headers, const StringMap &properties,
                    HttpContent *content = nullptr);

        HttpRequest(const HttpRequest &request);

        ~HttpRequest() override;

        bool equals(const HttpRequest &other) const override;

        void evaluates(const HttpRequest &other) override;

        int64_t contentSize() const;

        const String &text() const;

        bool match(const String &path) const;

        bool getPropValue(const String &key, String &value) const;

        String getPropValue(const String &key) const;

        String getPathSegment(int segment) const;

        bool findHeader(const String &name, const String &value) const;

    private:
        static bool parseVarName(const String &value, String &varName);

        static bool isVarName(const String &value);

    public:
        Url url;
        HttpMethod method;
        HttpHeaders headers;
        HttpProperties properties;
        HttpContent *content;
        Version version;
        String userAgent;
        bool verb;
        HttpCookie cookie;

    public:
        static const char PathSplitSymbol = '/';
    };

    class HttpResponse {
    public:
        HttpResponse(HttpContent *content = nullptr);

        ~HttpResponse();

        bool isTextContent() const;

        void setContent(const JsonNode &node);

        void setContent(const String &text);

        void setContent(Stream *stream);

    public:
        const HttpRequest *request;
        HttpHeaders headers;
        HttpContent *content;
        HttpStatus status;
        Version version;
        HttpCookie cookie;
    };
}

#endif  // HttpContent_h
