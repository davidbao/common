//
//  HttpContent.h
//  common
//
//  Created by baowei on 2020/3/24.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef HttpContent_h
#define HttpContent_h

#include "data/String.h"
#include "data/DataInterface.h"
#include "data/StringArray.h"
#include "data/PList.h"
#include "data/Map.h"
#include "net/NetType.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"

using namespace Net;
using namespace Json;

namespace Http {
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

    class HttpHeader : public IEquatable<HttpHeader>, public IEvaluation<HttpHeader> {
    public:
        String name;
        String value;

        explicit HttpHeader(const String &name = String::Empty, const String &value = String::Empty);

        HttpHeader(const HttpHeader &other);

        ~HttpHeader() override;

        bool equals(const HttpHeader &other) const override;

        void evaluates(const HttpHeader &other) override;

        HttpHeader &operator=(const HttpHeader &other);

        String toString() const;
    };

    class HttpHeaders : public List<HttpHeader> {
    public:
        using List::at;
        using List::add;
        using List::contains;

        explicit HttpHeaders(size_t capacity = DefaultCapacity);

        HttpHeaders(const HttpHeaders &array);

        HttpHeaders(HttpHeaders &&array) noexcept;

        HttpHeaders(const HttpHeaders &array, off_t offset, size_t count);

        HttpHeaders(const HttpHeader *array, size_t count, size_t capacity = DefaultCapacity);

        HttpHeaders(std::initializer_list<HttpHeader> list);

        ~HttpHeaders() override;

        HttpHeaders &operator=(const HttpHeaders &other);

        bool getValue(const String &name, String &value) const;

        bool contains(const String &name) const;

        void add(const String &name, const String &value);

        String at(const String &name) const;

        bool isTextContent() const;

    public:
        static bool parse(const String &str, HttpHeaders &headers);

    public:
        static const HttpHeaders JsonTypeHeaders;
    };

    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
    class HttpCookie : public IEquatable<HttpCookie>, public IEvaluation<HttpCookie> {
    public:
        enum SameSites {
            None,
            Strict,
            Lax
        };

        HttpCookie();

        HttpCookie(const HttpCookie &other);

        ~HttpCookie() override;

        bool equals(const HttpCookie &other) const override;

        void evaluates(const HttpCookie &other) override;

        HttpCookie &operator=(const HttpCookie &other);

        void add(const String &name, const String &value);

        bool getValue(const String &name, String &value) const;

        String at(const String &name) const;

        bool isEmpty() const;

        String toString() const;

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
        // Controls whether a cookie is sent with cross-origin requests,
        // providing some protection against cross-site request forgery attacks (CSRF).
        SameSites _sameSite;
    };

    class HttpSession : public IEquatable<HttpSession>, public IEvaluation<HttpSession> {
    public:
        HttpSession(const String &token, const String &name,
                    const TimeSpan &expiredTime = TimeSpan::Zero, bool kickout = false);

        HttpSession(const HttpSession &other);

        ~HttpSession() override;

        bool equals(const HttpSession &other) const override;

        void evaluates(const HttpSession &other) override;

        HttpSession &operator=(const HttpSession &other);

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

        uint32_t _start;
    };

    class HttpSessions {
    public:
        HttpSessions() = default;

        void add(const String &token, HttpSession *session);

        bool remove(const String &token);

        bool contains(const String &token) const;

        HttpSession *at(const String &token) const;

        HttpSession *atByName(const String &name) const;

        void checkSessions();

        void update(const String &token);

        size_t count() const;

    private:
        Map<String, HttpSession> _sessions;
    };

    class HttpContent : public IEquatable<HttpContent>,
                        public IEvaluation<HttpContent>,
                        public ICloneable<HttpContent> {
    public:
        HttpContent();

        ~HttpContent() override;

        virtual void write(void *buffer, size_t size, size_t nmemb) = 0;

        virtual ssize_t read(void *buffer, size_t size, size_t nmemb) = 0;

        virtual size_t size() = 0;

    public:
//        #       %23
//        %       %25
//        &       %26
//        +       %2B
//        /       %2F
//        \       %5C
//        =       %3D
//        ?       %3F
//        Space   %20
//        .       %2E
//        :       %3A
        static String escape(const String &str);

        static String unescape(const String &str);
    };

    class HttpStringContent : public HttpContent {
    public:
        HttpStringContent();

        explicit HttpStringContent(const String &value);

        HttpStringContent(const HttpStringContent &content);

        ~HttpStringContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        ssize_t read(void *buffer, size_t size, size_t nmemb) override;

        size_t size() override;

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

        explicit HttpJsonContent(const String &value);

        explicit HttpJsonContent(const JsonNode &value);

        HttpJsonContent(const HttpJsonContent &content);

        HttpContent *clone() const override;

        JsonNode node() const;
    };

    class HttpByteArrayContent : public HttpContent {
    public:
        HttpByteArrayContent();

        explicit HttpByteArrayContent(const ByteArray &value);

        HttpByteArrayContent(const HttpByteArrayContent &content);

        ~HttpByteArrayContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        ssize_t read(void *buffer, size_t size, size_t nmemb) override;

        size_t size() override;

        HttpContent *clone() const override;

        const ByteArray &value() const;

    private:
        ByteArray _value;
        MemoryStream *_stream;
    };

    class HttpStreamContent : public HttpContent {
    public:
        HttpStreamContent();

        explicit HttpStreamContent(Stream *stream);

        HttpStreamContent(const HttpStreamContent &content);

        ~HttpStreamContent() override;

        bool equals(const HttpContent &other) const override;

        void evaluates(const HttpContent &other) override;

        void write(void *buffer, size_t size, size_t nmemb) override;

        ssize_t read(void *buffer, size_t size, size_t nmemb) override;

        size_t size() override;

        HttpContent *clone() const override;

        const String &value() const;

        Stream *stream() const;

    private:
        Stream *_stream;
        String _value;
    };

    class HttpMethod : public IEquatable<HttpMethod>, public IEvaluation<HttpMethod> {
    public:
        explicit HttpMethod(const String &method);

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
            : public IEquatable<HttpProperties>,
              public IEvaluation<HttpProperties>,
              public IEvaluation<StringMap>,
              public PairIterator<String, String> {
    public:
        HttpProperties();

        explicit HttpProperties(const StringMap &properties);

        HttpProperties(const HttpProperties &properties);

        bool equals(const HttpProperties &other) const override;

        void evaluates(const HttpProperties &other) override;

        void evaluates(const StringMap &other) override;

        HttpProperties &operator=(const HttpProperties &other);

        HttpProperties &operator=(const StringMap &other);

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

        PairIterator<String, String>::const_iterator begin() const override {
            return PairIterator<String, String>::const_iterator(_values.begin());
        }

        PairIterator<String, String>::const_iterator end() const override {
            return PairIterator<String, String>::const_iterator(_values.end());
        }

        PairIterator<String, String>::iterator begin() override {
            return PairIterator<String, String>::iterator(_values.begin());
        }

        PairIterator<String, String>::iterator end() override {
            return PairIterator<String, String>::iterator(_values.end());
        }

        PairIterator<String, String>::const_reverse_iterator rbegin() const override {
            return PairIterator<String, String>::const_reverse_iterator(_values.rbegin());
        }

        PairIterator<String, String>::const_reverse_iterator rend() const override {
            return PairIterator<String, String>::const_reverse_iterator(_values.rend());
        }

        PairIterator<String, String>::reverse_iterator rbegin() override {
            return PairIterator<String, String>::reverse_iterator(_values.rbegin());
        }

        PairIterator<String, String>::reverse_iterator rend() override {
            return PairIterator<String, String>::reverse_iterator(_values.rend());
        }

    public:
        static bool parse(const String &value, HttpProperties &properties);

    private:
        StringMap _values;
    };

    class HttpRequest : public IEquatable<HttpRequest>, public IEvaluation<HttpRequest> {
    public:
        HttpRequest(const Url &url, const HttpMethod &method);

        HttpRequest(const Url &url, const HttpMethod &method,
                    const HttpHeaders &headers, HttpContent *content = nullptr);

        HttpRequest(const Url &url, const HttpMethod &method,
                    const HttpHeaders &headers, const StringMap &properties, HttpContent *content = nullptr);

        HttpRequest(const HttpRequest &request);

        ~HttpRequest() override;

        bool equals(const HttpRequest &other) const override;

        void evaluates(const HttpRequest &other) override;

        size_t contentSize() const;

        const String &text() const;

        bool match(const String &path) const;

        bool getPropValue(const String &key, String &value) const;

        String getPropValue(const String &key) const;

        String getPathSegment(int segment) const;

        bool findHeader(const String &name, const String &value) const;

        String toPropsStr() const;

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
        static const char PathSplitSymbol;
    };

    class HttpResponse {
    public:
        explicit HttpResponse(HttpContent *content = nullptr);

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

    class HttpCode {
    public:
        enum Code {
            Success = 0,
            Ok = Success,
            JsonParseError = 1,         // Json string parse error.
            DbError = 2,                // Error occurred while trying to connect to the database.
            ParameterIncorrect = 3,     // The request parameters is incorrect.
            Unknown = 9
        };

        class Item {
        public:
            int code;
            String msg;

            Item();

            Item(int code, const String &msg);

            Item(const Item &item);
        };

        ~HttpCode() = default;

        static void registerCode(int code, const String &msg);

        static void registerCode(std::initializer_list<Item> list);

        static String getMessage(int code);

        static StringMap at(int code);

        static StringMap okCode();

    private:
        HttpCode();

    private:
        static Dictionary<int, String> _codes;

        static HttpCode _staticInstance;
    };
}

#endif  // HttpContent_h
