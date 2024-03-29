//
//  HttpClient.h
//  common
//
//  Created by baowei on 2016/5/14.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef HttpClient_h
#define HttpClient_h

#include "data/ByteArray.h"
#include "IO/Stream.h"
#include "data/TimeSpan.h"
#include "data/PList.h"
#include "data/String.h"
#include "data/Dictionary.h"
#include "data/StringArray.h"
#include "system/PoolService.h"
#include "http/HttpContent.h"

using namespace System;

namespace Http {
    typedef void (*HttpSendCallback)(HttpResponse &);

    class HttpClient : public PoolService {
    public:
        TimeSpan connectionTimeout;
        TimeSpan receiveTimeout;

    public:
        HttpClient();

        HttpClient(std::initializer_list<KeyValuePair<String, String>> list);

        ~HttpClient() override;

        bool get(const Url &url, const HttpHeaders &headers, String &response) const;

        bool get(const Url &url, const HttpHeaders &headers, ByteArray &response) const;

        bool get(const Url &url, const HttpHeaders &headers, JsonNode &response) const;

        void getAsync(const Url &url, const HttpHeaders &headers, HttpSendCallback callback = nullptr);

        bool post(const Url &url, const HttpHeaders &headers, const String &request, String &response) const;

        bool post(const Url &url, const HttpHeaders &headers, const String &request = String::Empty) const;

        void postAsync(const Url &url, const HttpHeaders &headers, const String &request,
                       HttpSendCallback callback = nullptr);

        bool put(const Url &url, const HttpHeaders &headers, const String &request, String &response) const;

        bool put(const Url &url, const HttpHeaders &headers, const String &request = String::Empty) const;

        void putAsync(const Url &url, const HttpHeaders &headers, const String &request,
                      HttpSendCallback callback = nullptr);

        bool del(const Url &url, const HttpHeaders &headers, String &response) const;

        bool del(const Url &url, const HttpHeaders &headers) const;

        void delAsync(const Url &url, const HttpHeaders &headers, HttpSendCallback callback = nullptr);

        //
        // Summary:
        //     Download a file from a http server, use GET method.
        //
        // Parameters:
        //   url:
        //     The Uri the request is sent to.
        //   headers:
        //     The collection of HTTP request headers.
        //   fileName:
        //     The name of downloading file name.
        //
        // Returns:
        //    Return false if downloading failed.
        bool download(const Url &url, const HttpHeaders &headers, const String &fileName) const;

        //
        // Summary:
        //     Upload a file from a http server, use PUT method.
        //
        // Parameters:
        //   url:
        //     The Uri the request is sent to.
        //   headers:
        //     The collection of HTTP request headers.
        //   fileName:
        //     The name of uploading file name.
        //   response:
        //     Response is not empty if uploading failed.
        //
        // Returns:
        //    Return false if uploading failed.
        bool upload(const Url &url, const HttpHeaders &headers, const String &fileName, String &response) const;

        bool send(const HttpRequest &request, HttpResponse &response) const;

        void sendAsync(const HttpRequest &request, HttpSendCallback callback = nullptr);

    public:
        static String escape(const String &str);

        static String unescape(const String &str);

    protected:
        void process(const IPoolEntry *value) override;

    private:
        static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userdata);

        static size_t write_header(char *buffer, size_t size, size_t nitems, void *userdata);

        static size_t read_data(void *buffer, size_t size, size_t nmemb, void *userdata);
    };
}
#endif  // HttpClient_h
