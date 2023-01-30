//
//  TcpTemplate.h
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef TcpTemplate_h
#define TcpTemplate_h

#include "data/ByteArray.h"
#include "IO/Stream.h"
#include "data/TimeSpan.h"
#include "data/PList.h"
#include "data/String.h"
#include "rpc/RpcClient.h"

using namespace Data;
using namespace Rpc;

namespace Microservice {
    class TcpStringRequest : public IRpcSyncRequestData {
    public:
        String body;

        TcpStringRequest(const String &body = String::Empty);

        void write(Stream *stream) const override;

        void read(Stream *stream) override;

        void copyFrom(const IRpcSyncRequestData *value) override;

        IRpcSyncRequestData *clone() const override;
    };

    class TcpStringResponse : public IRpcSyncResponseData {
    public:
        String content;

        TcpStringResponse();

        void write(Stream *stream) const override;

        void read(Stream *stream) override;

        void copyFrom(const IRpcSyncResponseData *value) override;

        IRpcSyncResponseData *clone() const override;
    };

    class TcpTemplate {
    public:
        TcpTemplate();

        template<class T>
        bool get(const String &url, T &result) {
            return T::parseJson(getContent(url), result);
        }

        bool get(const String &url, String &content);

        template<class T, class K>
        bool post(const String &url, const T &body, K &result) {
            return K::parseJson(postContent(url, body.toJsonString()), result);
        }

        bool invoke(const String &url, const IRpcSyncRequestData &request, IRpcSyncResponseData &response);

        bool invokeAsync(const String &url, const IRpcAsyncRequestData &request, const IRpcAsyncResponseData &response,
                         async_callback action, void *owner = nullptr);

        bool notify(const String &url, const IRpcNotifyInfo &info);

    private:
        String getContent(const String &url);

        String postContent(const String &url, const String &body);

        Url parseUrl(const String &original);

        RpcClient *getOrNewClient(const Url &url);

    private:
        static Mutex _clientsMutex;
        static RpcClients _clients;

    public:
        static const char *DefaultMethodName;
    };
}

#endif /* TcpTemplate_h */
