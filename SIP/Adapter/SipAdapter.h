/*
 * sip adapter for resiprocate
 */

#ifndef SIP_ADAPTER_H
#define SIP_ADAPTER_H

#include <string>
#include <memory>

struct SipMessageAdapter;

class UA;

class SipMessageApp
{
public:
    enum Type
     {
        Request,
        Response,
     };

private:
     std::shared_ptr<SipMessageAdapter> m_adapter;

public:
    SipMessageApp();
    ~SipMessageApp();
    const std::shared_ptr<SipMessageAdapter>& getAdapter() const;
    Type getType() const;
    const char* getMethod() const;
    int getCode() const;
    const char* getReasonPhrase() const;
    const char* getContentType() const;
    const char* getCallID() const;
    const char* getBody() const;
    // const char* getSDPxxx() const;
    // SDP ...
    void setAdapter(const SipMessageAdapter& adapter);
    void addField(const std::string& name, const std::string& value);
    void addParameter(const std::string& fName, const std::string& pName, const std::string& value);
    void setBody(const std::string& body);
    void print() const;
};

class SipUserAgent
{
public:
    enum Transport
    {
        TRANSPORT_UDP,
        TRANSPORT_TCP,
        TRANSPORT_TLS
    };

    struct ClientInfo
    {
        std::string id;
        std::string passwd;
        int expire;
    };

    struct ServerInfo
    {
        std::string id;
        std::string domain;
        Transport transport;
        int port;
    };

    struct Info
    {
        ClientInfo client;
        ServerInfo server;
        int interval;
    };

protected:
    UA *app;

protected:
    bool postApp(UA *app, const SipMessageApp& message);

public:
    virtual bool init() = 0;
    virtual bool recv(SipMessageApp& message) = 0;
    virtual bool send(const SipMessageApp& message) = 0;

    /* 根据method和requestUri生成请求消息，自动填充头域，输出req */
    virtual bool genReqMessage(SipMessageApp& req, const std::string& method, const std::string& requestUri = "") = 0;

    /* 根据req生成响应消息，自动填充头域，输出res */
    virtual bool genResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase = "") = 0;

public:
    SipUserAgent() {}
    virtual ~SipUserAgent() {}

public:
    static SipUserAgent* create(UA *app, const Info& info);
};

#endif