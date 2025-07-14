#ifndef SIP_ADAPTER_H
#define SIP_ADAPTER_H

#include <string>
#include <memory>
#include "tinyxml2.h"

using namespace tinyxml2;

struct SipAdapterMessage;

class UA;

class SipUserMessage
{
private:
    std::shared_ptr<SipAdapterMessage> m_adapter;

public:
    SipUserMessage();
    ~SipUserMessage();
    void print() const;

    bool setAdapter(const SipAdapterMessage& adapter);
    const std::shared_ptr<SipAdapterMessage>& getAdapter() const;

    const char* getMethod() const;
    int getCode() const;
    const char* getReasonPhrase() const;

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

    bool setSdpMediaNum(int32_t num);
    bool setSdpMediaPort(int32_t index, int32_t port);
    bool setSdpMediaTransport(int32_t index, const char *transport);
    bool setSdpMediaPayloadType(int32_t index, int32_t type);
    bool setSdpMediaSSRC(int32_t index, uint32_t value);

    bool setExpires(int32_t expires);
    bool addExtensionField(const std::string& name, const std::string& value);
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
    UA *m_user;

public:
    SipUserAgent() {}
    virtual ~SipUserAgent() {}

public:
    virtual bool init() = 0;
    virtual bool makeRegistrationRequest(SipUserMessage& req) = 0;
    virtual bool sendRegistration(const SipUserMessage& req) = 0;
    virtual bool sendKeepaliveRequest(const XMLDocument& notify) = 0;
    virtual bool sendMANSCDPResponse(const XMLDocument& res) = 0;
    virtual bool makeSessionResponse(const SipUserMessage& req, SipUserMessage& res, int32_t code) = 0;
    virtual bool sendSessionResponse(const SipUserMessage& res) = 0;

protected:
    bool postRegistrationResponse(const SipUserMessage& res);
    bool postKeepaliveResponse(int32_t code);
    bool postMANSCDPRequest(const XMLDocument& req);
    bool postSessionRequest(const SipUserMessage& req);

public:
    static std::shared_ptr<SipUserAgent> create(UA *user, const ClientInfo& client, const ServerInfo& server);
    bool destroy();
};

#endif