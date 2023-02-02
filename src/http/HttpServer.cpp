//
//  HttpServer.cpp
//  common
//
//  Created by baowei on 2019/7/23.
//  Copyright © 2019 com. All rights reserved.
//

#include "http/HttpServer.h"
#include "diag/Trace.h"
#include "system/Math.h"
#include "system/Application.h"
#include "thread/TickTimeout.h"
#include "net/TcpServer.h"
#include "system/Regex.h"

#ifdef WIN32
#define TAILQ_EMPTY(head)       ((head)->tqh_first == NULL)

#define TAILQ_FIRST(head)       ((head)->tqh_first)

#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define TAILQ_FOREACH(var, head, field)                                 \
    for ((var) = TAILQ_FIRST((head));                               \
        (var);                                                      \
        (var) = TAILQ_NEXT((var), field))
#else

#include <sys/queue.h>

#endif

using namespace System;

namespace Http {
    HttpServer::Actions::Actions(void *owner, action_process_action processAction) {
        this->owner = owner;
        this->processAction = processAction;
        this->action = nullptr;
    }

    HttpServer::Actions::Actions(void *owner, action_send_document_cb action) : Actions(owner) {
        this->action = action;
    }

    HttpServer::Actions::Actions(const Actions &actions) {
        this->operator=(actions);
    }

    bool HttpServer::Actions::isEmpty() const {
        return this->processAction == nullptr;
    }

    void HttpServer::Actions::operator=(const Actions &value) {
        this->owner = value.owner;
        this->processAction = value.processAction;
        this->action = value.action;
    }

    bool HttpServer::Actions::operator==(const Actions &value) const {
        return this->owner == value.owner &&
               this->processAction == value.processAction &&
               this->action == value.action;
    }

    bool HttpServer::Actions::operator!=(const Actions &value) const {
        return !operator==(value);
    }

    HttpServer::Context::Context(const Endpoint &endpoint, const Secure &secure, TimeSpan timeout) {
        this->endpoint = endpoint;
        this->secure = secure;
        static const TimeSpan MinTimeout = TimeSpan::fromSeconds(3);
        this->timeout = timeout > MinTimeout ? timeout : MinTimeout;
    }

    HttpServer::Context::Context(const Context &context) {
        this->operator=(context);
    }

    bool HttpServer::Context::isEmpty() const {
        return this->endpoint.isEmpty();
    }

    void HttpServer::Context::operator=(const Context &value) {
        this->endpoint = value.endpoint;
        this->secure = value.secure;
        this->timeout = value.timeout;
    }

    bool HttpServer::Context::operator==(const Context &value) const {
        return this->endpoint == value.endpoint &&
               this->secure == value.secure &&
               this->timeout == value.timeout;
    }

    bool HttpServer::Context::operator!=(const Context &value) const {
        return !operator==(value);
    }

    HttpServer::ContextEntry::ContextEntry() : base(nullptr), loopExit(true) {
    }

    HttpServer::HttpServer() {
        _httpThread = new Thread("http_server");
        _httpsThread = new Thread("https_server");
    }

    HttpServer::~HttpServer() {
        stop();

        delete _httpThread;
        _httpThread = nullptr;

        delete _httpsThread;
        _httpsThread = nullptr;
    }

    bool HttpServer::startHttpServer(const Context &context, Actions actions) {
        _httpContext.actions = actions;
        return startHttpServer(context);
    }

    bool HttpServer::startHttpServer(const Context &context) {
        if (!context.isEmpty() && !_httpThread->isAlive()) {
            _httpContext.context = context;
            _httpThread->start(httpServerStart, &_httpContext);
            return true;
        }
        return false;
    }

    bool HttpServer::startHttpsServer(const Context &context, Actions actions) {
        _httpsContext.actions = actions;
        return startHttpsServer(context);
    }

    bool HttpServer::startHttpsServer(const Context &context) {
        if (!context.isEmpty() && !_httpsThread->isAlive()) {
            _httpsContext.context = context;
            _httpsThread->start(httpServerStart, &_httpsContext);
            return true;
        }
        return false;
    }

//    static bool isThreadDead(void* parameter)
//    {
//        HttpServer::ContextEntry* entry = (HttpServer::ContextEntry*)parameter;
//        return entry->loopExit;
//    }
    void HttpServer::stop() {
        event_base_loopbreak(_httpContext.base);
//        TickTimeout::msdelay(30 * 1000, isThreadDead, &_httpContext);

        event_base_loopbreak(_httpsContext.base);
//        TickTimeout::msdelay(10 * 1000, isThreadDead, &_httpsContext);
    }

    bool HttpServer::isHttpServerAlive() const {
        return _httpThread->isAlive();
    }

    bool HttpServer::isHttpsServerAlive() const {
        return _httpsThread->isAlive();
    }

    void HttpServer::httpServerStart(void *parameter) {
        ContextEntry *entry = static_cast<ContextEntry *>(parameter);
        assert(entry);

        const Context &context = entry->context;
        const Endpoint &endpoint = context.endpoint;
        const String http_addr = endpoint.isAnyAddress() ? String("0.0.0.0") : endpoint.address;

        Application *app = Application::instance();
        String path = app->rootPath();
        const Secure &secure = context.secure;
        String keyFile = Path::isPathRooted(secure.keyFile) ? secure.keyFile : Path::combine(path, secure.keyFile);
        String certFile = Path::isPathRooted(secure.certFile) ? secure.certFile : Path::combine(path, secure.certFile);
//        String cacertFile = Path::isPathRooted(secure.cacertFile) ? secure.cacertFile : Path::combine(path, secure.cacertFile);

#ifdef WIN32
        //evthread_use_windows_threads();
#elif __linux__
        //        evthread_use_ptheads();
#endif

        struct event_base *base;
        base = event_base_new();
        if (!base) {
            Trace::writeLine("Couldn't create an event_base!");
            return;
        }

        struct evhttp *http;
        http = evhttp_new(base);
        if (!http) {
            event_base_free(base);
            Trace::writeLine("couldn't create evhttp.");
            return;
        }

        if (secure.enabled) {
            SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
            SSL_CTX_set_options(ctx,
                                SSL_OP_SINGLE_DH_USE |
                                SSL_OP_SINGLE_ECDH_USE |
                                SSL_OP_NO_SSLv2);

            /* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
             * We just hardcode a single curve which is reasonably decent.
             * See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */
            EC_KEY *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
            if (!ecdh)
                Trace::writeLine("EC_KEY_new_by_curve_name.");
            if (1 != SSL_CTX_set_tmp_ecdh (ctx, ecdh))
                Trace::writeLine("SSL_CTX_set_tmp_ecdh.");

            if (!server_setup_certs(ctx, certFile, keyFile)) {
                evhttp_free(http);
                event_base_free(base);
                return;
            }

            evhttp_set_bevcb(http, bevcb, ctx);
        }

        evhttp_set_timeout(http, (int) context.timeout.totalSeconds()); // seconds

        evhttp_set_allowed_methods(http, EVHTTP_REQ_GET | EVHTTP_REQ_POST |
                                         EVHTTP_REQ_PUT | EVHTTP_REQ_OPTIONS |
                                         EVHTTP_REQ_DELETE | EVHTTP_REQ_PATCH);

        if (entry->actions.action != nullptr)
            evhttp_set_gencb(http, entry->actions.action, parameter);
        else
            evhttp_set_gencb(http, send_document_cb, parameter);

        struct evhttp_bound_socket *handle;
        handle = evhttp_bind_socket_with_handle(http, http_addr, endpoint.port);
        if (!handle) {
            Trace::writeFormatLine("Couldn't bind to port %d.", endpoint.port);
            evhttp_free(http);
            event_base_free(base);
            return;
        }

        {
            sock_hop ss;
            evutil_socket_t fd;
            ev_socklen_t socklen = sizeof(ss);
            char addrbuf[128];
            void *inaddr;
            const char *addr;
//            int got_port = -1;

            fd = evhttp_bound_socket_get_fd(handle);
            memset(&ss, 0, sizeof(ss));

            if (getsockname(fd, &ss.sa, &socklen)) {
                Trace::writeLine("getsockname() failed");
                evhttp_free(http);
                event_base_free(base);
                return;
            }
            if (ss.ss.ss_family == AF_INET) {
//                got_port = ntohs (ss.in.sin_port);
                inaddr = &ss.in.sin_addr;
            } else if (ss.ss.ss_family == AF_INET6) {
//                got_port = ntohs (ss.i6.sin6_port);
                inaddr = &ss.i6.sin6_addr;
            } else {
                Trace::writeFormatLine("Weird address family %d", ss.ss.ss_family);
                evhttp_free(http);
                event_base_free(base);
                return;
            }
            addr = evutil_inet_ntop(ss.ss.ss_family, inaddr, addrbuf,
                                    sizeof(addrbuf));
            if (addr) {
                //                Debug::writeFormatLine("Listening on %s:%d", addr, got_port);
            } else {
                Trace::writeLine("evutil_inet_ntop failed.");
                evhttp_free(http);
                event_base_free(base);
                return;
            }
        }

        Trace::writeLine(String::convert("http%s server start successfully. endpoint('%s')",
                                         secure.enabled ? "s" : "",
                                         endpoint.toString().c_str()), Trace::Info);

        entry->base = base;
        entry->loopExit = false;

        event_base_dispatch(base);

        evhttp_free(http);
        event_base_free(base);
        entry->base = nullptr;
        entry->loopExit = true;
        Trace::writeLine(String::convert("http%s server stop successfully. endpoint('%s')",
                                         secure.enabled ? "s" : "",
                                         endpoint.toString().c_str()), Trace::Info);
    }

    char *HttpServer::find_http_header(struct evhttp_request *req, struct evkeyvalq *params, const char *query_char) {
        if (req == NULL || params == NULL || query_char == NULL) {
            Debug::writeFormatLine("====line:%d,%s", __LINE__, "input params is null.");
            return NULL;
        }

        struct evhttp_uri *decoded = NULL;
        char *query = NULL;
        char *query_result = NULL;
        const char *path;
        const char *uri = evhttp_request_get_uri(req);

        if (uri == NULL) {
            Debug::writeFormatLine("====line:%d,evhttp_request_get_uri return null", __LINE__);
            return NULL;
        } else {
            //            Debug::writeFormatLine("====line:%d,Got a GET request for <%s>",__LINE__,uri);
        }

        decoded = evhttp_uri_parse(uri);
        if (!decoded) {
            Debug::writeFormatLine("====line:%d,It's not a good URI. Sending BADREQUEST", __LINE__);
            evhttp_send_error(req, HTTP_BADREQUEST, 0);
            return nullptr;
        }

        path = evhttp_uri_get_path(decoded);
        if (path == NULL) {
            path = "/";
        } else {
            //            Debug::writeFormatLine("====line:%d,path is:%s",__LINE__,path);
        }

        query = (char *) evhttp_uri_get_query(decoded);
        if (query == NULL) {
            Debug::writeFormatLine("====line:%d,evhttp_uri_get_query return null", __LINE__);
            return NULL;
        }

        evhttp_parse_query_str(query, params);
        query_result = (char *) evhttp_find_header(params, query_char);
        evhttp_clear_headers(params);

        return query_result;
    }

    bool HttpServer::server_setup_certs(SSL_CTX *ctx,
                                        const char *certificate_chain,
                                        const char *private_key) {
        Trace::writeFormatLine("Loading certificate chain from '%s' and private key from '%s'",
                               certificate_chain, private_key);

        if (File::exists(certificate_chain)) {
            if (SSL_CTX_use_certificate_file(ctx, certificate_chain, SSL_FILETYPE_PEM) <= 0) {
                Trace::writeLine("Failed to use the cert!", Trace::Error);
                return false;
            }
        } else {
            if (!TcpSSLClient::useCertificate(ctx, TcpSSLServer::ServerCert)) {
                Trace::writeLine("Failed to check the cert!", Trace::Error);
                return false;
            }
        }

        if (File::exists(private_key)) {
            if (SSL_CTX_use_PrivateKey_file(ctx, private_key, SSL_FILETYPE_PEM) <= 0) {
                Trace::writeLine("Failed to use the key!", Trace::Error);
                return false;
            }
        } else {
            if (!TcpSSLClient::usePrivateKey(ctx, TcpSSLServer::ServerKey)) {
                Trace::writeLine("Failed to check the key!", Trace::Error);
                return false;
            }
        }

        if (!SSL_CTX_check_private_key(ctx)) {
            Trace::writeLine("The cert and the key is not match!", Trace::Error);
            SSL_CTX_free(ctx);
            return false;
        }
        return true;

        //        if (1 != SSL_CTX_use_certificate_chain_file (ctx, certificate_chain))
        //            Trace::writeLine("SSL_CTX_use_certificate_chain_file");
        //
        //        if (1 != SSL_CTX_use_PrivateKey_file (ctx, private_key, SSL_FILETYPE_PEM))
        //            Trace::writeLine ("SSL_CTX_use_PrivateKey_file");
        //
        //        if (1 != SSL_CTX_check_private_key (ctx))
        //            Trace::writeLine ("SSL_CTX_check_private_key");
    }

    /**
     * This callback is responsible for creating a new SSL connection
     * and wrapping it in an OpenSSL bufferevent.  This is the way
     * we implement an https server instead of a plain old http server.
     */
    struct bufferevent *HttpServer::bevcb(struct event_base *base, void *arg) {
        struct bufferevent *r;
        SSL_CTX *ctx = (SSL_CTX *) arg;

        r = bufferevent_openssl_socket_new(base,
                                           -1,
                                           SSL_new(ctx),
                                           BUFFEREVENT_SSL_ACCEPTING,
                                           BEV_OPT_CLOSE_ON_FREE);
        return r;
    }

    bool HttpServer::getMessage(struct evhttp_request *req, String &buffer) {
        size_t post_size = 0;

        post_size = evbuffer_get_length(req->input_buffer);
        //        Debug::writeFormatLine("====line:%d,post len:%ld",__LINE__,post_size);
        if (post_size <= 0) {
//            Debug::writeFormatLine("====line:%d,post msg is empty!", __LINE__);
            return false;
        } else {
            buffer = String((const char *) evbuffer_pullup(req->input_buffer, -1), post_size);
            return true;
        }
    }

    bool HttpServer::getFileName(struct evhttp_request *req, String &fileName) {
        int n, r;
        struct evbuffer_iovec *v;

        n = evbuffer_peek(req->input_buffer, -1, nullptr, nullptr, 0);
        if (n <= 0)
            return false;

        bool multiData = false;
//        struct evkeyvalq* headers = req->input_headers;
//        String contentType = evhttp_find_header(headers, "Content-Type");
//        if(contentType.find("multipart/form-data") >= 0)
//        {
//            multiData = true;
//        }

        fileName = Path::getTempFileName("http_server");
        FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);

        v = (struct evbuffer_iovec *) malloc(sizeof(struct evbuffer_iovec) * n);
        evbuffer_peek(req->input_buffer, -1, nullptr, v, n);
        if (n > 0) {
            static const uint8_t MultiHeader[] = {0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D};
            static const ByteArray MultiHeaderArray(MultiHeader, 6);
            ByteArray source((const uint8_t *) v[0].iov_base, v[0].iov_len);
            int pos = source.find(MultiHeaderArray);
            if (pos >= 0)
                multiData = true;
        }
        for (int i = 0; i < n; ++i) {
            if (multiData && i == 0)  // trim header.
            {
                off_t offset = 0;
                static const uint8_t TailStr[] = {0x0D, 0x0A, 0x0D, 0x0A};
                static const ByteArray TailArray(TailStr, 4);
                ByteArray source((const uint8_t *) v[i].iov_base, v[i].iov_len);
                int pos = source.find(TailArray);
                if (pos > 0)
                    offset = pos + (int) TailArray.count();
                r = fs.write((const uint8_t *) v[i].iov_base + offset, 0, v[i].iov_len - offset);
            } else if (multiData && i == n - 1)  // trim tail.
            {
                static const uint8_t HeaderStr[] = {0x0D, 0x0A, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D};
                static const ByteArray HeaderArray(HeaderStr, 8);
                ByteArray source((const uint8_t *) v[i].iov_base, v[i].iov_len);
                int pos = source.find(HeaderArray);
                if (pos > 0)
                    r = fs.write((const uint8_t *) v[i].iov_base, 0, pos);
                else
                    r = fs.write((const uint8_t *) v[i].iov_base, 0, v[i].iov_len);
//                ByteArray temp((const uint8_t*)v[i].iov_base + v[i].iov_len - 64, 64);
//                Trace::debug(temp.toString());
            } else {
                r = fs.write((const uint8_t *) v[i].iov_base, 0, v[i].iov_len);
            }

            if (r <= 0)
                break;
        }
        free(v);
        fs.close();

        return true;
    }

    void HttpServer::send_document_cb(struct evhttp_request *req, void *arg) {
        struct evbuffer *evb = NULL;
        const char *uri = evhttp_request_get_uri(req);
        struct evhttp_uri *decoded = NULL;
        const char *uri_path = NULL;
        char *decoded_path = NULL;
        int code = HTTP_NOTFOUND;

        evhttp_cmd_type type = evhttp_request_get_command(req);

        ContextEntry *entry = static_cast<ContextEntry *>(arg);
        assert(entry);

        /* Decode the URI */
        decoded = evhttp_uri_parse(uri);
        if (!decoded) {
            Debug::writeLine("It's not a good URI. Sending BADREQUEST");
            evhttp_send_error(req, HTTP_BADREQUEST, 0);
            return;
        }
        /* Let's see what path the user asked for. */
        uri_path = evhttp_uri_get_path(decoded);
        if (!uri_path) uri_path = "/";

        /* We need to decode it, to see what path the user really wanted. */
        decoded_path = evhttp_uridecode(uri_path, 0, NULL);
        if (decoded_path == nullptr) {
            evhttp_send_error(req, HTTP_NOTFOUND, "Document was not found");
        } else {
            String path = decoded_path;
            if (path[0] == '/')
                path = path.substr(1, path.length() - 1);

            HttpHeaders inputHeaders;
            struct evkeyval *header;
            struct evkeyvalq *headers = req->input_headers;
            TAILQ_FOREACH(header, headers, next) {
                inputHeaders.add(header->key, header->value);
            }

            StringMap parameters;
            struct evkeyvalq params;
            struct evkeyval *param;
            if (evhttp_parse_query(uri, &params) == 0) {
                TAILQ_FOREACH(param, &params, next) {
                    parameters.add(param->key, param->value);
                }
            }
            evhttp_clear_headers(&params);

            // filter the url.
            if (!isIllegal(req, path)) {
                if (decoded)
                    evhttp_uri_free(decoded);
                if (decoded_path)
                    free(decoded_path);
                return;
            }

            const char *host = evhttp_request_get_host(req);
            Url baseUrl(entry->context.secure.enabled ? "https" : "http", Endpoint(host, entry->context.endpoint.port));
            Url url(baseUrl, path);
            HttpMethod method = HttpMethod::Get;
            switch (type) {
                case EVHTTP_REQ_GET:
                    method = HttpMethod::Get;
                    break;
                case EVHTTP_REQ_POST:
                    method = HttpMethod::Post;
                    break;
                case EVHTTP_REQ_HEAD:
                    method = HttpMethod::Head;
                    break;
                case EVHTTP_REQ_PUT:
                    method = HttpMethod::Put;
                    break;
                case EVHTTP_REQ_DELETE:
                    method = HttpMethod::Delete;
                    break;
                case EVHTTP_REQ_OPTIONS:
                    method = HttpMethod::Options;
                    break;
                case EVHTTP_REQ_TRACE:
                    method = HttpMethod::Trace;
                    break;
                case EVHTTP_REQ_CONNECT:
                    method = HttpMethod::Connect;
                    break;
                case EVHTTP_REQ_PATCH:
                    method = HttpMethod::Patch;
                    break;
                default:
                    method = HttpMethod::Get;
                    break;
            };

            HttpResponse response;
            if (type == EVHTTP_REQ_GET) {
                if (entry->actions.processAction != nullptr) {
                    HttpRequest request(url, method, inputHeaders, parameters);
                    code = entry->actions.processAction(entry->actions.owner, request, response);
                }
            } else if (type == EVHTTP_REQ_POST ||
                       type == EVHTTP_REQ_PUT ||
                       type == EVHTTP_REQ_DELETE ||
                       type == EVHTTP_REQ_TRACE ||
                       type == EVHTTP_REQ_PATCH) {
                if (entry->actions.processAction != nullptr) {
                    if (inputHeaders.isTextContent()) {
                        String body;
                        HttpServer::getMessage(req, body);
                        // filter the request body.
                        if (!isIllegal(req, body)) {
                            if (decoded)
                                evhttp_uri_free(decoded);
                            if (decoded_path)
                                free(decoded_path);
                            return;
                        }

                        HttpRequest request(url, method, inputHeaders, parameters, new HttpStringContent(body));
                        code = entry->actions.processAction(entry->actions.owner, request, response);
                    } else {
                        String fileName;
                        if (HttpServer::getFileName(req, fileName)) {
                            FileStream fs(fileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
                            HttpRequest request(url, method, inputHeaders, parameters, new HttpStreamContent(&fs));
                            code = entry->actions.processAction(entry->actions.owner, request, response);

                            if (File::exists(fileName))
                                File::deleteFile(fileName);
                        }
                    }
                }
            } else {
                if (entry->actions.processAction != nullptr) {
                    HttpRequest request(url, method, inputHeaders, parameters);
                    code = entry->actions.processAction(entry->actions.owner, request, response);
                }
            }

            bool hasContentType = false;
            for (uint32_t i = 0; i < response.headers.count(); i++) {
                const HttpHeader *header = response.headers[i];
                if (header->name == "Content-Type")
                    hasContentType = true;
                evhttp_add_header(req->output_headers, header->name, header->value);
            }

            if (!hasContentType) {
                if (dynamic_cast<HttpStringContent *>(response.content) != nullptr)
                    evhttp_add_header(req->output_headers, "Content-Type", "text/json; charset=UTF-8");
            }

            // process response cookie.
            if (!response.cookie.isEmpty()) {
                evhttp_add_header(req->output_headers, "Set-Cookie", response.cookie.toString());
            }

            String reason;
            switch (code) {
                case HTTP_OK:
                    reason = "OK";
                    break;
                case HTTP_BADREQUEST:
                    reason = "badrequest";
                    break;
                case HTTP_BADMETHOD:
                    reason = "badmethod";
                    break;
                case HTTP_NOTFOUND:
                default:
                    reason = "notfound";
                    break;
            }

            evb = evbuffer_new();
            HttpStreamContent *streamContent = dynamic_cast<HttpStreamContent *>(response.content);
            if (streamContent == nullptr) {
                HttpStringContent *stringContent = dynamic_cast<HttpStringContent *>(response.content);
                const String &body = stringContent != nullptr ? stringContent->value() : String::Empty;
//                Debug::writeLine(body);
                evbuffer_add(evb, body.c_str(), body.length());
                evhttp_send_reply(req, code, reason, evb);
            } else {
                FileStream *fs = dynamic_cast<FileStream *>(streamContent->stream());
                if (fs != nullptr) {
                    int fd = fs->fd();
                    struct stat st;
                    if (fstat(fd, &st) < 0) {
                        evhttp_send_error(req, HttpStatus::HttpNotFound, "Document was not found");
                    } else {
                        evbuffer_add_file(evb, fd, 0, st.st_size);
                        evhttp_send_reply(req, code, reason, evb);
                    }
                } else if (streamContent->stream() != nullptr) {
                    ByteArray body;
                    streamContent->stream()->readToEnd(body);
                    evbuffer_add(evb, body.data(), body.count());
                    evhttp_send_reply(req, code, reason, evb);
                }
            }
        }

        if (decoded)
            evhttp_uri_free(decoded);
        if (decoded_path)
            free(decoded_path);
        if (evb)
            evbuffer_free(evb);
    }

    bool HttpServer::isIllegal(struct evhttp_request *req, const String &str) {
        static Regex filterRegex(
                "\b(and|exec|insert|select|drop|grant|alter|delete|update|count|chr|mid|master|truncate|char|declare|or)\b|(\\*|;|\\+|'|%)");
        if (filterRegex.match(str)) {
            returnIllegalInfo(req);
            return false;
        }
        return true;
    }

    void HttpServer::returnIllegalInfo(struct evhttp_request *req) {
        JsonNode node;
        node.add(JsonNode("code", "500"));
        node.add(JsonNode("msg", "The url or request body is illegal."));

        struct evbuffer *evb = evbuffer_new();
        //evbuffer_add_printf(evb, "%s", node.toString().c_str());
        const String response = node.toString();
        evhttp_add_header(req->output_headers, "Content-Type", "text/json; charset=UTF-8");
        evbuffer_add(evb, response, response.length());
        evhttp_send_reply(req, HTTP_OK, "OK", evb);
        evbuffer_free(evb);
    }
}
