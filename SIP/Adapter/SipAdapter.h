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
    const char* getMANSCDPContents() const;

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

    bool setAdapter(const SipMessageAdapter& adapter);

    bool setContentType(const std::string& type, const std::string& subtype);
    bool setExpires(int32_t expires);
    bool addExtensionField(const std::string& name, const std::string& value);
    bool setMANSCDPContents(const std::string& data);

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
        std::string ipv4;
        int32_t port;
        std::string passwd;
        Transport transport;
        int32_t expire;
        int32_t interval;
    };

    struct ServerInfo
    {
        std::string id;
        std::string domain;
        std::string ipv4;
        int32_t port;
    };

protected:
    UA *app;

protected:
    bool postRegistrationResponse(const SipMessageApp& res);
    bool postOutDialogRequest(const SipMessageApp& req);
    bool postOutDialogResponse(const SipMessageApp& res, const SipMessageApp& req);
    bool postSessionRequest(const SipMessageApp& req);

public:
    virtual bool init() = 0;
    virtual bool recv(SipMessageApp& message) = 0;
    virtual bool send(const SipMessageApp& message) = 0;

    /* 根据method和requestUri生成请求消息，自动填充头域，输出req */
    virtual bool makeReqMessage(SipMessageApp& req, const std::string& method) = 0;

    /* 根据req生成响应消息，自动填充头域，输出res */
    virtual bool makeResMessage(SipMessageApp& res, const SipMessageApp& req, int code, const std::string& reasonPhrase = "") = 0;

public:
    SipUserAgent() {}
    virtual ~SipUserAgent() {}

public:
    static std::shared_ptr<SipUserAgent> create(UA *app, const ClientInfo& client, const ServerInfo& server);
    bool destroy();
};

#endif