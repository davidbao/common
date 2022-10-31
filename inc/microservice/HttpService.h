//
//  HttpService.h
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef HttpService_h
#define HttpService_h

#include "data/ValueType.h"
#include "database/SqlSelectFilter.h"
#include "system/ServiceFactory.h"
#include "http/HttpServer.h"

using namespace Common;
using namespace Database;

namespace Microservice
{
    class IHttpAction
    {
    public:
        IHttpAction();
        virtual ~IHttpAction();
        
    public:
        virtual HttpStatus onAction(const HttpRequest& request, HttpResponse& response);
        
    protected:
        virtual bool hasPostAction(const HttpRequest& request);
        virtual bool query(const HttpRequest& request, const SqlSelectFilter& filter, DataTable& table);
    };
    typedef Vector<IHttpAction> HttpActions;

    template <class T>
    class HttpCallback
    {
    public:
        typedef HttpStatus (T::*Method)(const HttpRequest&, HttpResponse&);
        
        HttpCallback() : HttpCallback(nullptr, nullptr)
        {
        }
        HttpCallback(T* instance, Method method)
        {
            _instance = instance;
            _method = method;
        };
        HttpCallback(const HttpCallback& callback)
        {
            this->operator=(callback);
        }
        
        HttpStatus operator()(const HttpRequest& request, HttpResponse& response)
        {
            return (_instance->*_method)(request, response);
        };
        
        HttpStatus execute(const HttpRequest& request, HttpResponse& response)
        {
            return operator()(request, response);
        };
        
        void operator=(const HttpCallback& value)
        {
            _instance = value._instance;
            _method = value._method;
        }
        bool operator==(const HttpCallback& value) const
        {
            return _instance == value._instance && _method == value._method;
        }
        bool operator!=(const HttpCallback& value) const
        {
            return !operator==(value);
        }
        
        T* instance() const
        {
            return _instance;
        }
        
    private:
        T* _instance;
        Method _method;
    };

    template <class T>
    class HttpQueryCallback
    {
    public:
        typedef bool (T::*Method)(const HttpRequest&, const SqlSelectFilter&, DataTable&);
        
        HttpQueryCallback() : HttpQueryCallback(nullptr, nullptr)
        {
        }
        HttpQueryCallback(T* instance, Method method)
        {
            _instance = instance;
            _method = method;
        };
        HttpQueryCallback(const HttpQueryCallback& callback)
        {
            this->operator=(callback);
        }
        
        bool operator()(const HttpRequest& request, const SqlSelectFilter& filter, DataTable& table)
        {
            return (_instance->*_method)(request, filter, table);
        };
        
        bool execute(const HttpRequest& request, const SqlSelectFilter& filter, DataTable& table)
        {
            return operator()(request, filter, table);
        };
        
        void operator=(const HttpQueryCallback& value)
        {
            _instance = value._instance;
            _method = value._method;
        }
        bool operator==(const HttpQueryCallback& value) const
        {
            return _instance == value._instance && _method == value._method;
        }
        bool operator!=(const HttpQueryCallback& value) const
        {
            return !operator==(value);
        }
        
        T* instance() const
        {
            return _instance;
        }
        
    private:
        T* _instance;
        Method _method;
    };

    class BaseHttpMapping
    {
    public:
        BaseHttpMapping(const HttpMethod& method, const String& path);
        virtual ~BaseHttpMapping();
        
        bool match(const HttpRequest& request) const;
        
        const HttpMethod& method() const;

        virtual HttpStatus execute(const HttpRequest& request, HttpResponse& response) = 0;
        
    protected:
        HttpMethod _method;
        String _path;
    };

    template <class T>
    class HttpMapping : public BaseHttpMapping
    {
    public:
        HttpMapping(const HttpMethod& method, const String& path, const HttpCallback<T>& callback) : BaseHttpMapping(method, path), _callback(callback)
        {
        }
        
        HttpMapping* clone() const
        {
            return new HttpMapping(_method, _path, _callback);
        }
        
        HttpStatus execute(const HttpRequest& request, HttpResponse& response) override
        {
            return _callback(request, response);
        }
        
        T* instance() const
        {
            return _callback.instance();
        }
        
    private:
        HttpCallback<T> _callback;
    };

    template <class T>
    class HttpQueryMapping : public BaseHttpMapping
    {
    public:
        HttpQueryMapping(const HttpMethod& method, const String& path, const HttpQueryCallback<T>& callback) : BaseHttpMapping(method, path), _callback(callback)
        {
        }
        
        HttpQueryMapping* clone() const
        {
            return new HttpQueryMapping(_method, _path, _callback);
        }
        
        HttpStatus execute(const HttpRequest& request, HttpResponse& response) override
        {
            if(request.method == HttpMethod::Post)
            {
                JsonNode node;
                SqlSelectFilter filter;
                if(SqlSelectFilter::parse(request.text(), filter))
                {
                    DataTable table;
                    if(_callback(request, filter, table))
                    {
                        JsonNode pagination;
                        int totalCount = table.totalCount();
                        pagination.add(JsonNode("dataTotal", ((Int32)totalCount).toString()));
#if DEBUG
                        if(filter.pageSize() <= 0)
                            Debug::writeFormatLine("url: '%s', request: '%s'", request.url.toString().c_str(), request.text().c_str());
#endif
                        pagination.add(JsonNode("maxPageNum", filter.pageSize() > 0 ? ((Int32)(totalCount / filter.pageSize() + 1)).toString() : "0"));
                        pagination.add(JsonNode("pageCount", ((Int32)filter.pageSize()).toString()));
                        pagination.add(JsonNode("pageNum", ((Int32)filter.page()).toString()));
                        pagination.add(JsonNode("pageStart", ((Int32)((filter.page() - 1) * filter.pageSize())).toString()));
                        JsonNode list;
                        table.toJsonNode(list);
                        pagination.add(JsonNode("list", list));
                        node.add(JsonNode("code", "200"));
                        node.add(JsonNode("pagination", pagination));
                    }
                    else
                    {
                        node.add(JsonNode("code", "601"));
                        node.add(JsonNode("msg", "Failed to query."));
                    }
                }
                else
                {
                    node.add(JsonNode("code", "600"));
                    node.add(JsonNode("msg", "The filter is incorrect."));
                }
                response.setContent(node);
                return HttpStatus::HttpOk;
            }
            else if(request.method == HttpMethod::Get)
            {
                JsonNode node;
                SqlSelectFilter filter(1, 1000);
                DataTable table;
                if(_callback(request, filter, table))
                {
                   JsonNode dataNode;
                   table.toJsonNode(dataNode);
                   node.add(JsonNode("code", "200"));
                   node.add(JsonNode("data", dataNode));
                }
                else
                {
                   node.add(JsonNode("code", "601"));
                   node.add(JsonNode("msg", "Failed to query."));
                }
                response.setContent(node);
                return HttpStatus::HttpOk;
            }
            return HttpStatus::HttpNotFound;
        }
        
        T* instance() const
        {
            return _callback.instance();
        }
        
    private:
        HttpQueryCallback<T> _callback;
    };

    typedef Vector<BaseHttpMapping> HttpMappings;

    class IHttpRegister : public IService
    {
    public:
        virtual void registerAction(IHttpAction* action) = 0;
        virtual void deregisterAction(IHttpAction* action) = 0;
        
        virtual void registerWebServer(const String& webPath, const StringArray& homePages = StringArray::Empty) = 0;
        
        template <class T>
        void registerMapping(const HttpMapping<T>& mapping)
        {
            Locker locker(&_mappingsMutex);
            _mappings.add(mapping.clone());
        }
        template <class T>
        void registerMapping(const HttpMethod& method, const String& path, const HttpCallback<T>& callback)
        {
            registerMapping(HttpMapping<T>(method, path, callback));
            if(method != HttpMethod::Options)
                registerMapping(HttpMapping<T>(HttpMethod::Options, path, callback));
        }
        
        template <class T>
        void registerQuery(const HttpQueryMapping<T>& mapping)
        {
            Locker locker(&_mappingsMutex);
            _mappings.add(mapping.clone());
        }
        template <class T>
        void registerQuery(const HttpMethod& method, const String& path, const HttpQueryCallback<T>& callback)
        {
            registerQuery(HttpQueryMapping<T>(method, path, callback));
            if(method != HttpMethod::Options)
                registerQuery(HttpQueryMapping<T>(HttpMethod::Options, path, callback));
        }
        
        template <class T>
        void removeMapping(T* instance)
        {
            Locker locker(&_mappingsMutex);
            HttpMappings removed(false);
            for (ssize_t i=_mappings.count()-1; i>=0; i--)
            {
                BaseHttpMapping* mapping = _mappings[i];
                HttpMapping<T>* mapping1 = dynamic_cast<HttpMapping<T>*>(mapping);
                if(mapping1 != nullptr && mapping1->instance() == instance)
                {
                    removed.add(mapping);
                }
                HttpQueryMapping<T>* mapping2 = dynamic_cast<HttpQueryMapping<T>*>(mapping);
                if(mapping2 != nullptr && mapping2->instance() == instance)
                {
                    removed.add(mapping);
                }
            }
            for (uint i=0; i<removed.count(); i++)
            {
                _mappings.remove(removed[i]);
            }
        }
        
        void clearMapping()
        {
            Locker locker(&_mappingsMutex);
            _mappings.clear();
        }
        
    protected:
        Mutex _mappingsMutex;
        HttpMappings _mappings;
    };

    class IHttpSession : public IService
    {
    public:
        virtual String addSession(const String& name, const TimeSpan& expiredTime = TimeSpan::Zero, bool kickout = false) = 0;
        virtual bool removeSession(const HttpRequest& request) = 0;
        virtual bool removeSession(const String& token) = 0;
        
        void registerTokenId(const String& tokenId);
        
    protected:
        String _tokenId;
    };

    class IHttpInterceptor : public IService
    {
    public:
        void addWhitelist(const StringArray& list);
        
    protected:
        StringMap _whitelist;
    };

    class HttpService : public IHttpRegister, public IHttpSession, public IHttpInterceptor
    {
    public:
        HttpService();
        ~HttpService() override;
        
        bool initialize();
        bool unInitialize();

        void registerAction(IHttpAction* action) override;
        void deregisterAction(IHttpAction* action) override;
        void registerWebServer(const String& webPath, const StringArray& homePages = StringArray::Empty) override;
        
        String addSession(const String& name, const TimeSpan& expiredTime = TimeSpan::Zero, bool kickout = false) override;
        bool removeSession(const HttpRequest& request) override;
        bool removeSession(const String& token) override;
        
    private:
        HttpStatus onAction(const HttpRequest& request, HttpResponse& response);
        HttpStatus onActionProcess(const HttpRequest& request, HttpResponse& response);
        HttpStatus onMappingProcess(const HttpRequest& request, HttpResponse& response);
        HttpStatus onWebServerProcess(const HttpRequest& request, HttpResponse& response);
        
        String existHomePage() const;
        
        void checkSessions();
        bool isAuthorized(const HttpRequest& request, const String& tokenId);
        void updateSession(const HttpRequest& request, const String& tokenId);
        
        bool isInWhitelist(const HttpRequest& request);
        
        bool getMimeType(const String& name, String& value);
        
        void sessionTimeUp();
        
    private:
        static HttpStatus onAction(void* parameter, const HttpRequest& request, HttpResponse& response);
        
    private:
        HttpServer _httpServer;
        HttpHeaders _httpHeaders;
        
        Mutex _actionsMutex;
        HttpActions _actions;
        
        String _webPath;
        StringArray _homePages;
        
        HttpSessions _sessions;
        Mutex _sessionsMutex;
        Timer* _sessionTimer;
        
        StringMap _extMineTypes;
    };
}

#endif /* HttpService_h */
