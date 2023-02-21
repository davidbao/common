//
//  SsoService.h
//  common
//
//  Created by baowei on 2022/5/31.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef SsoService_h
#define SsoService_h

#include "data/String.h"
#include "microservice/HttpService.h"

using namespace Data;

namespace Microservice {
    class SsoService {
    public:
        SsoService();

        virtual ~SsoService();

        virtual bool initialize();

        virtual bool unInitialize();

    protected:
        HttpStatus onEncrypt(const HttpRequest &request, HttpResponse &response);

        HttpStatus onLogin(const HttpRequest &request, HttpResponse &response);

        HttpStatus onLogout(const HttpRequest &request, HttpResponse &response);

        HttpStatus onModifyPassword(const HttpRequest &request, HttpResponse &response);

        enum ErrorCode {
            Success = 0,
            ParameterIncorrect = 1,
            NotFoundPrivateKey = 2,
            Sm2DecodeError = 3,
            JsonError = 4,
            HashError = 5,
            Sm4DecodeError = 6
        };

        ErrorCode decode(const HttpRequest &request, String &content);

        ErrorCode decode(const HttpRequest &request, JsonNode &content);

    private:
        bool addCache(const String &name, const String &value);

        bool getCache(const String &name, String &value);

        bool check(const String &name, const String &password);

        bool checkByYml(const String &name, const String &password);

        bool check(const String &name);

        static bool checkByYml(const String &name);

        bool modifyPassword(const String &name, const String &oldPassword, const String &newPassword);

        static bool modifyPasswordByYml(const String &name, const String &oldPassword, const String &newPassword);

    private:
        static void errorNode(ErrorCode code, JsonNode &node);

    private:
        StringMap _caches;

    private:
        static const char *AccessTokenId;
        static const char *Sm2String;
    };
}

#endif /* SsoService_h */
