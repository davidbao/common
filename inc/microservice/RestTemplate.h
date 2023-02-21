//
//  RestTemplate.h
//  common
//
//  Created by baowei on 2020-02-17.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef RestTemplate_h
#define RestTemplate_h

#include "data/ByteArray.h"
#include "IO/Stream.h"
#include "data/TimeSpan.h"
#include "data/PList.h"
#include "data/String.h"
#include "http/HttpClient.h"

using namespace Data;
using namespace Http;

namespace Microservice {
    class RestTemplate {
    public:
        RestTemplate();

        template<class T>
        bool get(const String &url, T &result, const HttpHeaders &headers = DefaultHeaders) {
            return T::parseJson(getContent(url, headers), result);
        }

        bool get(const String &url, String &result, const HttpHeaders &headers = DefaultHeaders);

        template<class T, class K>
        bool post(const String &url, const T &body, K &result, const HttpHeaders &headers = DefaultHeaders) {
            return K::parseJson(postContent(url, body.toJsonString(), headers), result);
        }

        bool post(const String &url, const String &body, String &result, const HttpHeaders &headers = DefaultHeaders);

        template<class T, class K>
        bool put(const String &url, const T &body, K &result, const HttpHeaders &headers = DefaultHeaders) {
            return K::parseJson(putContent(url, body.toJsonString(), headers), result);
        }

        bool put(const String &url, const String &body, String &result, const HttpHeaders &headers = DefaultHeaders);

        Url parseUrl(const String &original);

    private:
        String getContent(const String &url, const HttpHeaders &headers = DefaultHeaders);

        String postContent(const String &url, const String &body, const HttpHeaders &headers = DefaultHeaders);

        String putContent(const String &url, const String &body, const HttpHeaders &headers = DefaultHeaders);

    private:
        Mutex _clientMutex;
        HttpClient _client;

    public:
        static const HttpHeaders DefaultHeaders;
    };
}

#endif /* RestTemplate_h */
