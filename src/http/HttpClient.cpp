//
//  HttpClient.cpp
//  common
//
//  Created by baowei on 2016/5/14.
//  Copyright © 2016 com. All rights reserved.
//

#include "http/HttpClient.h"
#include "diag/Trace.h"
#include "data/StringArray.h"
#include "IO/FileStream.h"
#include "system/PoolService.h"
#include "curl/curl.h"

namespace Common {
    class HttpRequestEntry : public IPoolEntry {
    public:
        HttpRequest request;
        HttpSendCallback callback;

        HttpRequestEntry(const HttpRequest &request, HttpSendCallback callback) : request(request), callback(callback) {
        }
    };

    HttpClient::HttpClient() {
        connectionTimeout = TimeSpan::fromSeconds(30);
        receiveTimeout = TimeSpan::fromSeconds(30);

        /* In windows, this will init the winsock stuff */
        curl_global_init(CURL_GLOBAL_ALL);
    }

    HttpClient::~HttpClient() {
        curl_global_cleanup();
    }

    bool HttpClient::get(const Url &url, const HttpHeaders &headers, String &response) {
        HttpRequest httpRequest(url, HttpMethod::Get, headers);
        HttpStringContent *content = new HttpStringContent();
        HttpResponse httpResponse(content);
        bool result = send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
        if (result) {
            response = content->value();
        }
        return result;
    }

    bool HttpClient::get(const Url &url, const HttpHeaders &headers, ByteArray &response) {
        HttpRequest httpRequest(url, HttpMethod::Get, headers);
        HttpByteArrayContent *content = new HttpByteArrayContent();
        HttpResponse httpResponse(content);
        bool result = send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
        if (result) {
            response = content->value();
        }
        return result;
    }

    bool HttpClient::get(const Url &url, const HttpHeaders &headers, JsonNode &response) {
        String str;
        return get(url, headers, str) && JsonNode::parse(str, response);
    }

    void HttpClient::getAsync(const Url &url, const HttpHeaders &headers, HttpSendCallback callback) {
        HttpRequest request(url, HttpMethod::Get, headers);
        sendAsync(request, callback);
    }

    bool HttpClient::post(const Url &url, const HttpHeaders &headers, const String &request, String &response) {
        HttpRequest httpRequest(url, HttpMethod::Post, headers, new HttpStringContent(request));
        HttpStringContent *content = new HttpStringContent();
        HttpResponse httpResponse(content);
        bool result = send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
        if (result) {
            response = content->value();
        }
        return result;
    }

    bool HttpClient::post(const Url &url, const HttpHeaders &headers, const String &request) {
        String response;
        return post(url, headers, request, response);
    }

    void HttpClient::postAsync(const Url &url, const HttpHeaders &headers, const String &request,
                               HttpSendCallback callback) {
        if(!isStarted()) {
            start();
        }
        HttpRequest httpRequest(url, HttpMethod::Post, headers, new HttpStringContent(request));
        sendAsync(httpRequest, callback);
    }

    bool HttpClient::put(const Url &url, const HttpHeaders &headers, const String &request, String &response) {
        HttpRequest httpRequest(url, HttpMethod::Put, headers, new HttpStringContent(request));
        HttpStringContent *content = new HttpStringContent();
        HttpResponse httpResponse(content);
        bool result = send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
        if (result) {
            response = content->value();
        }
        return result;
    }

    bool HttpClient::put(const Url &url, const HttpHeaders &headers, const String &request) {
        String response;
        return put(url, headers, request, response);
    }

    void
    HttpClient::putAsync(const Url &url, const HttpHeaders &headers, const String &request, HttpSendCallback callback) {
        if(!isStarted()) {
            start();
        }
        HttpRequest httpRequest(url, HttpMethod::Put, headers, new HttpStringContent(request));
        sendAsync(httpRequest, callback);
    }

    bool HttpClient::download(const Url &url, const HttpHeaders &headers, const String &fileName) {
        HttpRequest httpRequest(url, HttpMethod::Get, HttpHeaders::JsonTypeHeaders);
        FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
        HttpStreamContent *content = new HttpStreamContent(&fs);
        HttpResponse httpResponse(content);
        return send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
    }

    bool HttpClient::upload(const Url &url, const HttpHeaders &headers, const String &fileName, String &response) {
        FileStream stream(fileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        HttpRequest httpRequest(url, HttpMethod::Put, headers, new HttpStreamContent(&stream));
        HttpStringContent *content = new HttpStringContent();
        HttpResponse httpResponse(content);
        bool result = send(httpRequest, httpResponse) && httpResponse.status == HttpStatus::HttpOk;
        if (result) {
            response = content->value();
        }
        return result;
    }

    bool HttpClient::send(const HttpRequest &request, HttpResponse &response) {
        if (request.url.isEmpty())
            return false;

        Url url = request.url;
        if (request.properties.count() > 0) {
            url = Url(url, String::format("?%s", request.properties.toString().c_str()));
        }

        CURLcode res = CURL_LAST;
        CURL *curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.toString().c_str());
            if (!request.userAgent.isNullOrEmpty())
                curl_easy_setopt(curl, CURLOPT_USERAGENT, request.userAgent.c_str());
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, (int) connectionTimeout.totalSeconds());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int) receiveTimeout.totalSeconds());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
//            curl_easy_setopt(curl, CURLOPT_SSLVERSION, 1);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, request.verb);
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

            // request method.
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.method.method.c_str());

            // request headers.
            struct curl_slist *headerList = nullptr;
            if (request.headers.count() > 0) {
                for (uint32_t i = 0; i < request.headers.count(); i++) {
                    const HttpHeader *header = request.headers[i];
                    headerList = curl_slist_append(headerList, header->toString().c_str());
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
            }

            // request data.
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);
            curl_easy_setopt(curl, CURLOPT_READDATA, &request);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, request.contentSize());
            if (request.method == HttpMethod::Post) {
                curl_easy_setopt(curl, CURLOPT_UPLOAD, true);
//                curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.contentStr().c_str());
            } else if (request.method == HttpMethod::Put) {
                curl_easy_setopt(curl, CURLOPT_UPLOAD, true);
            } else if (request.method == HttpMethod::Delete) {
                curl_easy_setopt(curl, CURLOPT_UPLOAD, true);
            }

            // response headers.
            curl_easy_setopt(curl, CURLOPT_HEADER, false);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

            // response data.
            response.request = &request;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK) {
                Debug::writeFormatLine("curl_easy_perform(%s) failed: %s", url.toString().c_str(),
                                       curl_easy_strerror(res));
                if (res == CURLE_OPERATION_TIMEDOUT)
                    response.status = (HttpStatus) HttpRequestTimeout;
            } else {
                /* Check for errors */
                int responseCode = HttpStatus::HttpInternalServerError;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
                response.status = (HttpStatus) responseCode;
            }

            if (headerList != nullptr)
                curl_slist_free_all(headerList); /* free the header list */

            /* always cleanup */
            curl_easy_cleanup(curl);
        }

        return res == CURLE_OK;
    }

    void HttpClient::sendAsync(const HttpRequest &request, HttpSendCallback callback) {
        if(!isStarted()) {
            start();
        }
        add(new HttpRequestEntry(request, callback));
    }

    void HttpClient::process(const IPoolEntry *value) {
        const HttpRequestEntry *entry = dynamic_cast<const HttpRequestEntry *>(value);
        assert(entry);
        HttpResponse response(new HttpStringContent());
        send(entry->request, response);
        if (entry->callback != nullptr) {
            entry->callback(response);
        }
    }

    size_t HttpClient::write_data(void *buffer, size_t size, size_t nmemb, void *userdata) {
        HttpResponse *response = (HttpResponse *) userdata;
        assert(response);

        if (response->content != nullptr)
            response->content->write(buffer, size, nmemb);

        return size * nmemb;
    }

    size_t HttpClient::write_header(char *buffer, size_t size, size_t nitems, void *userdata) {
        /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
        /* 'userdata' is set with CURLOPT_HEADERDATA */
        HttpHeaders *headers = (HttpHeaders *) userdata;
        String str = String((const char *) buffer, (uint32_t) (size * nitems));
        HttpHeaders::parse(str, *headers);
        return size * nitems;
    }

    size_t HttpClient::read_data(void *buffer, size_t size, size_t nmemb, void *userdata) {
        HttpRequest *request = (HttpRequest *) userdata;
        assert(request);

        if (request->content != nullptr)
            return request->content->read(buffer, size, nmemb);

        return 0;
    }

    String HttpClient::escape(const String &str) {
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

    String HttpClient::unescape(const String &str) {
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
}
