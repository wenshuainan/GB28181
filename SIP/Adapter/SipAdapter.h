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
    void print() const;

    const std::shared_ptr<SipMessageAdapter>& getAdapter() const;
    Type getType() const;
    const char* getMethod() const;
    int getCode() const;
    const char* getReasonPhrase() const;
    const char* getContentType() const;

    const char* getCallID() const;
    const char* getBody() const;

    int32_t getSdpSessionVersion() const;
    const char* getSdpSessionOwner() const;
    const char* getSdpSessionName() const;
    const char* getSdpSessionIpv4() const;
    int32_t getSdpMediaNum() const;
    int32_t getSdpMediaPort(int32_t index) const;
    const char* getSdpMediaTransport(int32_t index) const;
    int32_t getSdpMediaPayloadType(int32_t index) const;
    const char* getSdpMediaIpv4(int32_t index) const;
    uint32_t getSdpMediaSSRC(int32_t index) const;

    void setAdapter(const SipMessageAdapter& adapter);

    void addField(const std::string& name, const std::string& value);
    void addParameter(const std::string& fName, const std::string& pName, const std::string& value);

    void setBody(const std::string& body);

    bool setSdp(int32_t version, const std::string& owner, const std::string& name);
    bool setSdpConnectionIpv4(const std::string& ipv4);
    bool addSdpMedia(int32_t num);
    bool setSdpMediaPort(int32_t index, int32_t port);
    bool setSdpMediaTransport(int32_t index, const std::string& transport);
    bool setSdpMediaPayloadType(int32_t index, int32_t payloadType);
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
    static std::shared_ptr<SipUserAgent> create(UA *app, const Info& info);
    bool destroy();
};

#endif