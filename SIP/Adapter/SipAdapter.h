/*
 * sip adapter for resiprocate
 */

#ifndef SIP_ADAPTER_H
#define SIP_ADAPTER_H

#include <string>
#include <memory>

struct SipMessageAdapter;

class SipGenericMessage
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
    SipGenericMessage();
    ~SipGenericMessage();
    const std::shared_ptr<SipMessageAdapter>& getAdapter() const;
    Type getType() const;
    const std::string& getMethod() const;
    const std::string& getRequestUri() const;
    int getCode() const;
    const std::string& getReasonPhrase() const;
    const std::string& getFieldValue(const std::string& name) const;
    const std::string& getParameterValue(const std::string& fieldName, const std::string& parameterName) const;
    const std::string& getBody() const;
    void setAdapter(const SipMessageAdapter& adapter);
    void addField(const std::string& name, const std::string& value);
    void addParameter(const std::string& fieldName, const std::string& parameterName, const std::string& value);
    void setBody(const std::string& body);
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
    };

    struct ServerInfo
    {
        std::string domain;
        std::string passwd;
        Transport transport;
        int port;
    };

    struct Info
    {
        ClientInfo client;
        ServerInfo server;
    };

public:
    virtual bool init() = 0;
    virtual bool recv(SipGenericMessage& message) = 0;
    virtual bool send(const SipGenericMessage& message) = 0;
    virtual bool genReqMessage(SipGenericMessage& req, const std::string& method, const std::string& requestUri = "") = 0;
    virtual bool genResMessage(SipGenericMessage& res, const SipGenericMessage& req, int code, const std::string& reasonPhrase = "") = 0;

public:
    SipUserAgent() {}
    virtual ~SipUserAgent() {}

public:
    static SipUserAgent* create(const Info& info);
};

#endif