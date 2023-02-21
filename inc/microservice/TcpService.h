//
//  TcpService.h
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef TcpService_h
#define TcpService_h

#include "data/String.h"
#include "net/TcpServer.h"
#include "system/ServiceFactory.h"
#include "rpc/RpcServer.h"
#include <event2/http.h>

using namespace Data;
using namespace Rpc;

namespace Microservice {
    enum TcpStatus {
        TcpOk = HTTP_OK,    /**< request completed ok */
        TcpNoContent = HTTP_NOCONTENT,    /**< request does not have content */
        TcpMovePerm = HTTP_MOVEPERM,    /**< the uri moved permanently */
        TcpMoveTemp = HTTP_MOVETEMP,    /**< the uri moved temporarily */
        TcpNotMdified = HTTP_NOTMODIFIED,    /**< page was not modified from last */
        TcpBadRequest = HTTP_BADREQUEST,    /**< invalid http request was made */
        TcpNotFound = HTTP_NOTFOUND,    /**< could not find content for uri */
        TcpBadMethod = HTTP_BADMETHOD,     /**< method not allowed for this uri */
        TcpEntityTooLarge = HTTP_ENTITYTOOLARGE,    /**<  */
        TcpExpectationFailed = HTTP_EXPECTATIONFAILED,    /**< we can't handle this expectation */
        TcpInternal = HTTP_INTERNAL,     /**< internal error */
        TcpNotImplemented = HTTP_NOTIMPLEMENTED,     /**< not implemented */
        TcpServuavail = HTTP_SERVUNAVAIL,    /**< the server is not available */
    };

    class ITcpAction {
    public:
        ITcpAction();

        virtual ~ITcpAction();

        virtual TcpStatus onGetAction(const StringArray &texts, String &result);

        virtual TcpStatus onPostAction(const StringArray &texts, const String &body, String &result);
    };

    typedef PList<ITcpAction> TcpActions;

    class ITcpService : public IService {
    public:
        virtual void registerMethod(RpcMethod *method) = 0;

        virtual void registerAction(ITcpAction *action) = 0;

        virtual void deregisterAction(ITcpAction *action) = 0;
    };

    class TcpService : public ITcpService {
    public:
        TcpService();

        ~TcpService() override;

        bool initialize();

        bool unInitialize();

        void registerMethod(RpcMethod *method) override;

        void registerAction(ITcpAction *action) override;

        void deregisterAction(ITcpAction *action) override;

    private:
        static bool onStringResponse(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response);

        bool onStringResponseInner(const IRpcSyncRequestData *request, IRpcSyncResponseData *response);

    private:
        RpcServer *_server;

        Mutex _actionsMutex;
        TcpActions _actions;
    };
}

#endif /* TcpService_h */
