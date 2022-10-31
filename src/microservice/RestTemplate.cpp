//
//  RestTemplate.cpp
//  common
//
//  Created by baowei on 2020-02-17.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/RestTemplate.h"
#include "diag/Trace.h"
#include "IO/MemoryStream.h"
#include "data/StringArray.h"
#include "microservice/LoadBalancerClient.h"

namespace Microservice {
    const HttpHeaders RestTemplate::DefaultHeaders(new HttpHeader("Content-Type", "application/json"), nullptr);

    RestTemplate::RestTemplate() {
    }

    bool RestTemplate::get(const String &url, String &result, const HttpHeaders &headers) {
        Locker locker(&_clientMutex);
        return _client.get(parseUrl(url), headers, result);
    }

    String RestTemplate::getContent(const String &url, const HttpHeaders &headers) {
        String result;
        get(url, result, headers);
        return result;
    }

    bool RestTemplate::post(const String &url, const String &body, String &result, const HttpHeaders &headers) {
        Locker locker(&_clientMutex);
        return _client.post(parseUrl(url), headers, body, result);
    }

    String RestTemplate::postContent(const String &url, const String &body, const HttpHeaders &headers) {
        String result;
        post(url, body, result, headers);
        return result;
    }

    bool
    RestTemplate::RestTemplate::put(const String &url, const String &body, String &result, const HttpHeaders &headers) {
        Locker locker(&_clientMutex);
        return _client.put(parseUrl(url), headers, body, result);
    }

    String RestTemplate::putContent(const String &url, const String &body, const HttpHeaders &headers) {
        String result;
        put(url, body, result, headers);
        return result;
    }

    Url RestTemplate::parseUrl(const String &original) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        LoadBalancerClient *service = factory->getService<LoadBalancerClient>();
        if (service != nullptr) {
            Url url = service->reconstructUrl(original);
            if (!url.isEmpty() && (url.scheme() == Url::SchemeHttp || url.scheme() == Url::SchemeHttps))
                return url;
        }
        return Url::Empty;
    }
}

