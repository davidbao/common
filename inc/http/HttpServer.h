//
//  HttpServer.h
//  common
//
//  Created by baowei on 2019/7/4.
//  Copyright (c) 2019 com. All rights reserved.
//

#ifndef HttpServer_h
#define HttpServer_h

#include "http/HttpContent.h"
#include "thread/Thread.h"

#include <evhttp.h>
#include <event.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/keyvalq_struct.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef __ANDROID__
#include <linux/in.h>
#include <linux/in6.h>
#endif

using namespace Threading;

namespace Http {
    class HttpServer {
    public:
        typedef void (*action_send_document_cb)(struct evhttp_request *, void *);

        typedef bool (*action_check_access_key)(void *, const String &);

        typedef HttpStatus (*action_process_action)(void *, const HttpRequest &, HttpResponse &);

        struct Actions : IEvaluation<Actions>, IEquatable<Actions> {
        public:
            void *owner;
            action_process_action processAction;

            action_send_document_cb action;

            explicit Actions(void *owner = nullptr, action_process_action processAction = nullptr);

            Actions(void *owner, action_send_document_cb action);

            Actions(const Actions &other);

            ~Actions() override;

            bool isEmpty() const;

            Actions &operator=(const Actions &other);

            void evaluates(const Actions &other) override;

            bool equals(const Actions &other) const override;
        };

        class Secure : IEvaluation<Secure>, IEquatable<Secure> {
        public:
            String certFile;
            String keyFile;
            String cacertFile;
            bool enabled;

            explicit Secure(bool enabled = false);

            Secure(const Secure &other);

            ~Secure() override;

            Secure &operator=(const Secure &other);

            void evaluates(const Secure &other) override;

            bool equals(const Secure &other) const override;

        public:
            static const Secure None;
        };

        class Context {
        public:
            Endpoint endpoint;
            Secure secure;
            TimeSpan timeout;

            explicit Context(const Endpoint &endpoint = Endpoint::Empty, const Secure &secure = Secure::None,
                    const TimeSpan &timeout = TimeSpan::fromSeconds(30));

            Context(const Context &context);

            bool isEmpty() const;

            Context &operator=(const Context &value);

            bool operator==(const Context &value) const;

            bool operator!=(const Context &value) const;
        };

        HttpServer();

        ~HttpServer();

        bool startHttpServer(const Context &context, const Actions &actions);

        bool startHttpsServer(const Context &context, const Actions &actions);

        void stop();

        bool isAlive() const;

    public:
        static bool getMessage(struct evhttp_request *req, String &buffer);

        static bool getFileName(struct evhttp_request *req, String &fileName);

    private:
        bool startHttpServer(const Context &context);

        bool startHttpsServer(const Context &context);

    private:
        static char *find_http_header(struct evhttp_request *req, struct evkeyvalq *params, const char *query_char);

        static bool server_setup_certs(SSL_CTX *ctx,
                                       const char *certificate_chain,
                                       const char *private_key);

        /**
         * This callback is responsible for creating a new SSL connection
         * and wrapping it in an OpenSSL bufferevent.  This is the way
         * we implement an https server instead of a plain old http server.
         */
        static struct bufferevent *bevcb(struct event_base *base, void *arg);

        static void httpServerStart(void *parameter);

        static void send_document_cb(struct evhttp_request *req, void *arg);

        static bool isIllegal(struct evhttp_request *req, const String &str);

        static void returnIllegalInfo(struct evhttp_request *req);

    private:
        /* Instead of casting between these types, create a union with all of them,
         * to avoid -Wstrict-aliasing warnings. */
        typedef union {
            struct sockaddr_storage ss;
            struct sockaddr sa;
            struct sockaddr_in in;
            struct sockaddr_in6 i6;
        } sock_hop;

        Thread *_httpThread;
        Thread *_httpsThread;

    public:
        class ContextEntry {
        public:
            Context context;
            Actions actions;

            struct event_base *base;
            bool loopExit;

            ContextEntry();
        };

        ContextEntry _httpContext;
        ContextEntry _httpsContext;
    };
}

#endif  // HttpServer_h
